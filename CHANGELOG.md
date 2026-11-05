# Changelog

Формат — [Keep a Changelog](https://keepachangelog.com/uk/1.1.0/), версії — [SemVer](https://semver.org/lang/uk/).
Перед релізом перенесіть пункти з «Unreleased» у нову версію (див. CONTRIBUTING.md, «Реліз»).

## [Unreleased]

### Added
- `apps/sandbox` з аргументами `--frames`, `--max-fps`, `--no-validation`.
- `CMakePresets.json`: `debug`, `release`, `relwithdebinfo`, `asan`, `ci-debug`, `ci-release`.
- `./dev`: налаштування, збирання, тести, запуск, профілі запуску, перевірки як у CI.
- Git-хуки, шаблони PR та issues, Dependabot для GitHub Actions.
- Документація в `docs/`: встановлення, розробка, архітектура, план розвитку, стиль коду.
- Сайт документації Doxygen з довідником класів і діаграмами (`./dev docs`), публікація на GitHub Pages (ADR 0007).
- ADR 0006: модульна архітектура.

### Changed
- Залежності — копії в `vendor/` замість submodules (ADR 0005); `doxygen-awesome-css` теж перенесено у `vendor/`.
- Документація без дублювання: кожна тема описана в одному файлі.
- Повідомлення Vulkan у лозі позначаються джерелом (`validation`, `performance`, `general`).

### Fixed
- Конфігурація CMake (синтаксична помилка, шлях до `vendor/`).
