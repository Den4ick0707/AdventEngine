# AdventEngine

Модульний 3D-рушій на C++20 і Vulkan 1.3. Легке ядро, до якого підключаються модулі: рендерер, сцена, ассети,
анімація, у майбутньому апаратне трасування променів. Редактор, демо й ігри — окремі застосунки поверх рушія.

[![CI](https://github.com/Den4ick0707/AdventEngine/actions/workflows/ci.yml/badge.svg?branch=dev)](https://github.com/Den4ick0707/AdventEngine/actions/workflows/ci.yml)

## Стан

Етап 2 із 8: Vulkan-основа. Є вікно, ініціалізований Vulkan і головний цикл з фіксованим кроком симуляції;
наступний крок — swapchain і очищення екрана. Повний план — [docs/roadmap.md](docs/roadmap.md).

## Швидкий старт

Linux:

```bash
git clone git@github.com:Den4ick0707/AdventEngine.git
cd AdventEngine
./dev setup --install
./dev build && ./dev run
```

Вимоги, Windows і CLion — [docs/getting-started.md](docs/getting-started.md).

## Документація

| Документ | Зміст |
|----------|-------|
| [Встановлення](docs/getting-started.md) | вимоги, перше збирання, CLion, Windows |
| [Розробка](docs/development.md) | `./dev`, пресети, перевірки, CI, документація |
| [Архітектура](docs/architecture.md) | шари, модулі, правила залежностей, відкриті питання |
| [План розвитку](docs/roadmap.md) | етапи і критерії готовності |
| [Стиль коду](docs/coding-style.md) | імена, помилки, документування |
| [Процес роботи](CONTRIBUTING.md) | гілки, коміти, Pull Request, релізи |
| [Рішення (ADR)](docs/adr/README.md) | архітектурні рішення з обґрунтуванням |
| [Бібліотеки](vendor/README.md) | сторонні бібліотеки, версії, локальні правки |
| [Зміни](CHANGELOG.md) | історія версій |

Довідник API з діаграмами класів генерується Doxygen: `./dev docs` або
[онлайн-версія](https://den4ick0707.github.io/AdventEngine/).

## Ліцензія

[MIT](LICENSE)
