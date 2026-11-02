# Changelog

Формат — [Keep a Changelog](https://keepachangelog.com/uk/1.1.0/), версії — [SemVer](https://semver.org/lang/uk/).
Перед релізом перенесіть пункти з «Unreleased» у нову версію (див. CONTRIBUTING.md, «Реліз»).

## [Unreleased]

### Added
- `apps/sandbox` з аргументами `--frames`, `--max-fps`, `--no-validation`.
- `CMakePresets.json`: `debug`, `release`, `relwithdebinfo`, `asan`, `ci-debug`, `ci-release`.
- `./dev`: налаштування, збирання, тести, запуск, профілі запуску, перевірки як у CI.
- Git-хуки, шаблони PR та issues, Dependabot для GitHub Actions, `scripts/github/setup-repo.sh`.

### Changed
- Залежності — копії в `vendor/` замість submodules (ADR 0005).

### Fixed
- Конфігурація CMake (синтаксична помилка, шлях до `vendor/`).
