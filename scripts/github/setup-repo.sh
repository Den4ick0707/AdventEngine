#!/usr/bin/env bash
# One-time (and safe to re-run) GitHub setup for AdventEngine. Needs the GitHub CLI:
#   gh auth login          # once, account with admin rights on the repository
#   ./scripts/github/setup-repo.sh [--dry-run] [owner/repo]
#
# What it does (see CONTRIBUTING.md for the workflow behind it):
#   1. creates the dev branch from main if it does not exist and makes dev the default branch;
#   2. merge settings: squash + merge commits, no rebase merges, auto-delete merged branches, auto-merge;
#   3. rulesets: main, dev and release/* change only through PRs with a green `ci-ok` check;
#      v* tags cannot be moved or deleted (admins can bypass for mistakes);
#   4. labels used by issue templates, Dependabot and release notes.
set -euo pipefail

dry_run=0
if [[ "${1:-}" == "--dry-run" ]]; then
    dry_run=1
    shift
fi

command -v gh >/dev/null || {
    echo "GitHub CLI not found: https://cli.github.com (Ubuntu: sudo apt install gh)" >&2
    exit 1
}
gh auth status >/dev/null 2>&1 || {
    echo "Run 'gh auth login' first." >&2
    exit 1
}

repo="${1:-$(gh repo view --json nameWithOwner --jq .nameWithOwner)}"
echo "==> repository: $repo"

api() {
    if ((dry_run)); then
        echo "[dry-run] gh api $*"
    else
        gh api "$@"
    fi
}

# Status check produced by the `ci-ok` job in .github/workflows/ci.yml; 15368 = GitHub Actions app.
readonly ci_check='{"context": "ci-ok", "integration_id": 15368}'

branch_ruleset() {
    local name="$1" pattern="$2" merge_methods="$3"
    cat <<EOF
{
  "name": "$name",
  "target": "branch",
  "enforcement": "active",
  "conditions": { "ref_name": { "include": ["$pattern"], "exclude": [] } },
  "bypass_actors": [],
  "rules": [
    { "type": "deletion" },
    { "type": "non_fast_forward" },
    {
      "type": "pull_request",
      "parameters": {
        "required_approving_review_count": 0,
        "dismiss_stale_reviews_on_push": false,
        "require_code_owner_review": false,
        "require_last_push_approval": false,
        "required_review_thread_resolution": true,
        "allowed_merge_methods": $merge_methods
      }
    },
    {
      "type": "required_status_checks",
      "parameters": {
        "strict_required_status_checks_policy": false,
        "required_status_checks": [ $ci_check ]
      }
    }
  ]
}
EOF
}

tag_ruleset() {
    cat <<'EOF'
{
  "name": "version tags",
  "target": "tag",
  "enforcement": "active",
  "conditions": { "ref_name": { "include": ["refs/tags/v*"], "exclude": [] } },
  "bypass_actors": [ { "actor_id": 5, "actor_type": "RepositoryRole", "bypass_mode": "always" } ],
  "rules": [ { "type": "deletion" }, { "type": "update" }, { "type": "non_fast_forward" } ]
}
EOF
}

upsert_ruleset() {
    local name="$1" body="$2" id
    id="$(gh api "repos/$repo/rulesets" --jq ".[] | select(.name == \"$name\") | .id" 2>/dev/null || true)"
    if ((dry_run)); then
        echo "[dry-run] ruleset '$name' (${id:+update $id}${id:-create}):"
        echo "$body"
        return
    fi
    if [[ -n "$id" ]]; then
        gh api --method PUT "repos/$repo/rulesets/$id" --input - <<<"$body" >/dev/null
        echo "  updated ruleset: $name"
    else
        gh api --method POST "repos/$repo/rulesets" --input - <<<"$body" >/dev/null
        echo "  created ruleset: $name"
    fi
}

# --- 1. dev branch --------------------------------------------------------------
echo "==> branches"
if gh api "repos/$repo/branches/dev" >/dev/null 2>&1; then
    echo "  dev exists"
else
    main_sha="$(gh api "repos/$repo/git/ref/heads/main" --jq .object.sha)"
    api --method POST "repos/$repo/git/refs" -f ref=refs/heads/dev -f sha="$main_sha" >/dev/null
    echo "  created dev from main ($main_sha)"
fi

# --- 2. repository settings -----------------------------------------------------
echo "==> merge settings, default branch = dev"
api --method PATCH "repos/$repo" \
    -f default_branch=dev \
    -F allow_squash_merge=true \
    -F allow_merge_commit=true \
    -F allow_rebase_merge=false \
    -F delete_branch_on_merge=true \
    -F allow_auto_merge=true \
    -F allow_update_branch=true \
    -f squash_merge_commit_title=PR_TITLE \
    -f squash_merge_commit_message=PR_BODY \
    -f merge_commit_title=PR_TITLE \
    -f merge_commit_message=PR_BODY >/dev/null
echo "  done"

# --- 3. rulesets ----------------------------------------------------------------
# main: only merge commits (release/* and hotfix/* keep their history, so main can be merged back into dev).
# dev and release/*: squash for feature PRs, merge commits for back-merges from main.
echo "==> rulesets"
upsert_ruleset "main" "$(branch_ruleset main refs/heads/main '["merge"]')"
upsert_ruleset "dev" "$(branch_ruleset dev refs/heads/dev '["squash", "merge"]')"
upsert_ruleset "release branches" "$(branch_ruleset 'release branches' 'refs/heads/release/**' '["squash", "merge"]')"
upsert_ruleset "version tags" "$(tag_ruleset)"

# --- 4. labels ------------------------------------------------------------------
echo "==> labels"
labels=(
    "bug|d73a4a|Something is broken"
    "feature|a2eeef|New engine functionality"
    "renderer|5319e7|Vulkan renderer and shaders"
    "vendor|c5def5|Changes in vendor/ (third-party copies)"
    "dependencies|0366d6|Dependency updates"
    "ci|fbca04|CI, workflows"
    "build|fef2c0|CMake, presets, ./dev tooling"
    "docs|0075ca|Documentation, ADR"
    "skip-changelog|eeeeee|Do not list in release notes"
)
for entry in "${labels[@]}"; do
    IFS='|' read -r name color description <<<"$entry"
    if ((dry_run)); then
        echo "[dry-run] label $name"
    else
        gh label create "$name" --repo "$repo" --color "$color" --description "$description" --force >/dev/null
        echo "  $name"
    fi
done

echo "==> done. Check: https://github.com/$repo/settings/rules"
