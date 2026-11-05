# Процес роботи

Як влаштовано роботу з Git і GitHub: задачі, гілки, коміти, Pull Request, релізи.
Встановлення — [docs/getting-started.md](docs/getting-started.md), інструменти й перевірки —
[docs/development.md](docs/development.md).

## Задачі

- Кожна задача — issue на GitHub (шаблони «Помилка» і «Задача / можливість»).
- Issue належить milestone — етапу з [плану розвитку](docs/roadmap.md).
- Прогрес видно на дошці [GitHub Projects](https://github.com/users/Den4ick0707/projects) (шаблон Kanban):

  | Колонка | Що в ній |
  |---------|----------|
  | Backlog | усі задачі, які колись треба зробити |
  | Ready | наступні на черзі, зрозуміло, що робити |
  | In progress | задача в роботі, є гілка |
  | In review | відкритий Pull Request, чекає на `ci-ok` і злиття |
  | Done | злито; issue переходить сюди автоматично |
- Pull Request посилається на issue (`Closes #N`), і issue закривається автоматично після злиття.

## Гілки

```
feature/*  --squash PR-->  dev  -->  release/X.Y.Z  --merge PR-->  main  --tag vX.Y.Z
                            ^                                        |
                            +------------ merge PR (back-merge) -----+
hotfix/X.Y.Z (з main)  --merge PR-->  main  --tag-->  back-merge у dev
```

| Гілка | Звідки | Куди | Злиття | Призначення |
|-------|--------|------|--------|-------------|
| `main` | — | — | — | лише випущені версії; кожен коміт має тег `vX.Y.Z` |
| `dev` | — | — | — | інтеграційна гілка за замовчуванням, завжди збирається |
| `feature/<коротко>` | `dev` | `dev` | squash | одна задача: `feature/swapchain` |
| `release/X.Y.Z` | `dev` | `main` | merge commit | підготовка релізу: версія, CHANGELOG, виправлення |
| `hotfix/X.Y.Z` | `main` | `main` | merge commit | термінове виправлення випущеної версії |

Squash для `feature/*` дає один коміт на задачу в історії `dev`. Merge commit для `release/*` і `hotfix/*`
зберігає спільну історію `main` і `dev`, без чого back-merge давав би хибні конфлікти.

## Щоденна робота

```bash
git switch dev && git pull
git switch -c feature/swapchain
# зміни
./dev ci
git commit -am "Add swapchain creation"
git push
```

Після `git push` GitHub пропонує створити Pull Request (кнопка **Compare & pull request**) у `dev`.

## Коміти

- Англійською, наказовий спосіб, з великої літери, до 72 символів, без крапки в кінці:
  `Add swapchain recreation`, `Fix validation error on resize`.
- Зміни у `vendor/` — окремими комітами з префіксом: `vendor(glfw): Fix resize event on X11`.
- Порожній рядок між заголовком і описом. Опис пояснює, що змінено і чому.

Правила перевіряє хук `commit-msg`.

## Pull Request

- Назва PR стає назвою коміту після squash, тож пишеться за правилами комітів.
- Мітка визначає розділ у нотатках до релізу:

  | Мітка | Для чого |
  |-------|----------|
  | `feature` | нова можливість |
  | `bug` | виправлення помилки |
  | `renderer` | рендерер і шейдери |
  | `vendor`, `dependencies` | сторонні бібліотеки, оновлення залежностей |
  | `ci`, `build` | CI, CMake, `./dev` |
  | `docs` | документація, ADR |
  | `skip-changelog` | не показувати в нотатках до релізу |

- Злиття можливе, лише коли перевірка `ci-ok` зелена і всі обговорення в PR вирішено.

## Реліз

```bash
git switch dev && git pull
git switch -c release/0.2.0
# project(VERSION 0.2.0) у CMakeLists.txt, перенести пункти Unreleased у CHANGELOG.md
git commit -am "Prepare release 0.2.0"
git push                                   # PR у main, злиття через merge commit
git switch main && git pull
git tag -a v0.2.0 -m "AdventEngine 0.2.0"
git push origin v0.2.0                     # release.yml створює чернетку GitHub Release
# PR main -> dev (merge commit), щоб повернути зміни релізу в dev
```

- Тег має збігатися з `VERSION` у `CMakeLists.txt`, інакше `release.yml` зупиниться.
- Тег із суфіксом (`v0.2.0-rc.1`) стає pre-release.
- Нотатки до релізу GitHub складає з назв злитих PR, згрупованих за мітками (`.github/release.yml`).

## Налаштування GitHub

Налаштовано вручну один раз. Якщо репозиторій доведеться налаштовувати заново:

- **Settings > General.** Гілка за замовчуванням `dev`. Увімкнено merge commits і squash, назва коміту — з назви
  й опису PR; rebase вимкнено. Увімкнено *Always suggest updating PR branches*, *Allow auto-merge*,
  *Automatically delete head branches*.
- **Settings > Rules > Rulesets.** Усі в статусі *Active*, без bypass, крім тегів:

  | Ruleset | Ціль | Правила | Способи злиття |
  |---------|------|---------|----------------|
  | main | `main` | Restrict deletions; PR (0 approvals, conversation resolution); status check `ci-ok`; Block force pushes | Merge |
  | dev | `dev` | так само | Merge, Squash |
  | release | `release/**` | так само, без Restrict deletions (гілка видаляється після злиття) | Merge, Squash |
  | version tags | теги `v*` | Restrict updates, Restrict deletions, Block force pushes; bypass — Repository admin | — |

- **Мітки** — таблиця в розділі «Pull Request».
- **Settings > Pages.** Source: GitHub Actions (публікація документації, `docs.yml`).
- **Milestones** — етапи з [плану розвитку](docs/roadmap.md).
- **Projects.** Проєкт `AdventEngine` (шаблон Kanban), прив'язаний до репозиторію, default repository —
  `AdventEngine`. Поля: Status, Priority, Size, Start date, Target date. Workflows: *Item closed* і
  *Pull request merged* переводять у Done; *Auto-add to project* (`is:issue`) додає нові issues у Backlog.
