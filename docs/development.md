# Розробка

## Інструмент `./dev`

Усі щоденні дії виконуються через `./dev`. CI на Linux викликає ті самі команди, тож `./dev ci`, що пройшов локально,
означає зелений CI.

| Команда | Що робить |
|---------|-----------|
| `./dev setup [--install]` | перевіряє інструменти, вмикає git-хуки; з `--install` ставить системні пакети |
| `./dev build [preset] [target]` | конфігурує за потреби і збирає |
| `./dev test [preset]` | збирає і запускає модульні тести |
| `./dev run [preset] [-- args]` | збирає і запускає sandbox |
| `./dev smoke [preset]` | sandbox без дисплея на 120 кадрів; падає на помилках (див. нижче) |
| `./dev format [--check]` | `clang-format` для `engine/`, `apps/`, `tests/` |
| `./dev tidy [preset] [files]` | `clang-tidy`, зокрема стиль імен |
| `./dev docs` | генерує документацію Doxygen у `build/docs/html` |
| `./dev ci` | усе, що перевіряє CI: формат, збирання `ci-debug`, тести, smoke, clang-tidy |
| `./dev p <profile>` | профіль запуску з `scripts/runs/` |
| `./dev vendor-update <dir> <url> <tag>` | оновлює бібліотеку у `vendor/` (див. [vendor/README.md](../vendor/README.md)) |

Пресет за замовчуванням — `debug` або значення змінної `ADVENT_PRESET`.

## Пресети CMake

| Пресет | Призначення |
|--------|-------------|
| `debug` | Debug, validation layers, `ADVENT_ASSERT` |
| `release` | Release |
| `relwithdebinfo` | Release з налагоджувальною інформацією, для профілювання |
| `asan` | Debug з AddressSanitizer і UBSan (Linux) |
| `ci-debug`, `ci-release` | як `debug` і `release`, попередження вважаються помилками |

Результат збирання: `build/<preset>/`, виконувані файли в `build/<preset>/bin/`.

## Профілі запуску

Профіль — bash-файл у `scripts/runs/` зі змінними `PRESET`, `TARGET`, `ARGS`, `RUNNER`, `HEADLESS`
і, за потреби, власною функцією `run()`. Шаблон — [`scripts/runs/_template.sh`](../scripts/runs/_template.sh).

```bash
./dev profiles                 # список
./dev p sandbox-gdb            # sandbox під gdb
./dev new-profile my-test      # особистий профіль у scripts/runs/local/ (не комітиться)
```

Готові профілі: `sandbox`, `sandbox-release`, `sandbox-gdb`, `sandbox-asan`, `sandbox-headless`, `tests-asan`,
`vulkan-loader-debug`.

## Перевірки

### Збирання і тести
Пресети `ci-*` вмикають `-Werror` (`/WX` на MSVC). Код має збиратися без попереджень на GCC, Clang і MSVC.

### Smoke-тест і validation layers
Повідомлення Vulkan потрапляють у лог із позначкою джерела:

| Позначка | Джерело | Чи падає `./dev smoke` |
|----------|---------|------------------------|
| `[vulkan:validation]` | неправильне використання API, помилка в коді | так |
| `[vulkan:performance]` | неоптимальне використання API | так |
| `[vulkan:general]` | завантажувач і драйвери, напр. пропущений ICD | лише на рівні `error` |

Будь-який рядок `[error]` теж зупиняє тест. Мета для Debug: нуль помилок і нуль попереджень validation.

### clang-format і clang-tidy
Еталонні версії — ті, що в CI (зараз 18). Новіший `clang-tidy` локально може знайти нові попередження,
бо `.clang-tidy` вмикає групи правил цілком. У такому разі вирішується одне з двох:
виправити код так, щоб він проходив обидві версії, або вимкнути правило в `.clang-tidy` з поясненням у PR.

### Git-хуки
Вмикаються `./dev setup` (`core.hooksPath = .githooks`):

| Хук | Що перевіряє |
|-----|--------------|
| `pre-commit` | форматування змінених C++-файлів |
| `commit-msg` | формат повідомлення коміту (див. [CONTRIBUTING.md](../CONTRIBUTING.md)) |
| `pre-push` | заборона прямого push у `main` і `dev`; завантаження файлів Git LFS |
| `post-checkout`, `post-commit`, `post-merge` | передають керування Git LFS |

Разовий обхід: `git commit --no-verify`.

## CI

Workflow `.github/workflows/ci.yml` запускається на push у `main`, `dev`, `release/**` і на Pull Request у них.

| Job | Що робить |
|-----|-----------|
| `linux` | GCC (`ci-debug`, `ci-release`) і Clang (`ci-debug`): збирання, тести, smoke-тест на програмному Vulkan (lavapipe) |
| `windows` | MSVC (`ci-debug`, `ci-release`): збирання і тести |
| `lint` | `./dev format --check`, `./dev tidy` |
| `docs` | збирання документації Doxygen без попереджень |
| `ci-ok` | успішний, лише якщо успішні всі job-и вище; саме його вимагають правила гілок |

Новий job додається в `needs` job-а `ci-ok`, і тоді правила гілок не треба змінювати.

`release.yml` збирає архіви для Linux і Windows за тегом `vX.Y.Z`. `docs.yml` публікує документацію
на GitHub Pages після кожного злиття в `dev`.

## Документація

- Ручна документація — Markdown у `docs/`, `README.md`, `CONTRIBUTING.md`.
- Довідник API — коментарі `///` у публічних заголовках (див. [стиль коду](coding-style.md), розділ «Документування коду»).
- `./dev docs` збирає обидві частини в один сайт Doxygen: розділи з `docs/`, список класів і діаграми класів.
