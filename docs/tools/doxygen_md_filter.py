#!/usr/bin/env python3
"""Doxygen input filter for Markdown files (FILTER_PATTERNS in docs/Doxyfile).

The Markdown files are written for GitHub. This filter adapts them for the Doxygen site
without changing the files themselves:
- the root README.md becomes the main page;
- nested README.md files get their own page ids (Doxygen would otherwise treat them as main pages);
- relative links to other Markdown files become links to their pages;
- relative links to other repository files and folders point to GitHub.
"""

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GITHUB = "https://github.com/Den4ick0707/AdventEngine/blob/dev/"
PAGE_IDS = {"docs/adr/README.md": "adr_index", "vendor/README.md": "vendor_readme"}
LINK = re.compile(r"\]\(([^)\s]+)\)")


def page_ref(repo_path: str) -> str:
    if repo_path == "README.md":
        return "index.html"
    if repo_path in PAGE_IDS:
        return "@ref " + PAGE_IDS[repo_path]
    # Doxygen names Markdown pages md_<path>, with "/" escaped as "_2".
    return "@ref md_" + repo_path[: -len(".md")].replace("/", "_2")


def rewrite_link(match: re.Match, file_dir: Path) -> str:
    target = match.group(1)
    if re.match(r"^[a-z]+:", target) or target.startswith(("#", "@")):
        return match.group(0)
    path, _, anchor = target.partition("#")
    resolved = (file_dir / path).resolve()
    try:
        repo_path = resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return match.group(0)
    if repo_path.endswith(".md") and not anchor:
        return "](" + page_ref(repo_path) + ")"
    return "](" + GITHUB + repo_path + ("#" + anchor if anchor else "") + ")"


def main() -> None:
    source = Path(sys.argv[1]).resolve()
    repo_path = source.relative_to(ROOT).as_posix()
    lines = source.read_text(encoding="utf-8").splitlines()

    heading_done = False
    in_code = False
    for index, line in enumerate(lines):
        if line.startswith("```"):
            in_code = not in_code
        if in_code:
            continue
        if not heading_done and line.startswith("# "):
            if repo_path == "README.md":
                line = "@mainpage " + line[2:]
            elif repo_path in PAGE_IDS:
                line = f"{line} {{#{PAGE_IDS[repo_path]}}}"
            heading_done = True
        lines[index] = LINK.sub(lambda m: rewrite_link(m, source.parent), line)

    sys.stdout.write("\n".join(lines) + "\n")


if __name__ == "__main__":
    main()
