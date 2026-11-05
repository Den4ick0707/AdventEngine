# Сторонні скіли: джерела і ліцензії

Шість скілів у `.claude/skills/` створено на основі сторонніх. Оригінали було перевірено (див. нижче),
а потім переписано під проєкт: C++20, GLM, Vulkan 1.3, GLFW, правила з `docs/`. Приклади для Godot, Unity
і Unreal, посилання на відсутні скіли й довідкові файли прибрано. Кожен адаптований скіл у кінці
посилається на джерело.

| Скіл | Джерело | Ліцензія |
|------|---------|----------|
| `cpp-pro` | Jeffallan/claude-skills | MIT |
| `physics-tuning` | awesome-gamedev-agent-skills | Apache-2.0 |
| `performance-optimization` | awesome-gamedev-agent-skills | Apache-2.0 |
| `camera-systems` | awesome-gamedev-agent-skills | Apache-2.0 |
| `input-systems` | awesome-gamedev-agent-skills | Apache-2.0 |
| `shader-programming` | awesome-gamedev-agent-skills | Apache-2.0 |

## Джерела

| Репозиторій | Коміт | Ліцензія |
|-------------|-------|----------|
| github.com/Jeffallan/claude-skills | `143243cb2da0e52b02a766fa69c1ff77a71cc73c` | MIT |
| github.com/gamedev-skills/awesome-gamedev-agent-skills | `19d93f97c9c1ea0f8b306e77aa81e0d3e352d3e9` | Apache-2.0 |

Тексти ліцензій і NOTICE — у `licenses/`. Вони зберігаються, доки в проєкті є похідні від цих скілів тексти.

## Перевірка оригіналів (2026-10-30)

- Кожен репозиторій завантажено в ізольовану теку; жоден файл не запускався.
- Автоматично перевірено: приховані символи Unicode, prompt injection, `curl | bash`, `eval`, доступ до ключів
  і `.env`, мережеві виклики, руйнівні команди, обхід дозволів, символьні посилання, виконувані файли.
- Кожен `SKILL.md` прочитано вручну.
- Результат: лише Markdown, без скриптів і хуків, без поля `allowed-tools`, без прихованих інструкцій.

## Що не взято

| Що | Причина |
|----|---------|
| miles990/claude-software-skills (`game-development`, `cpp`) | приклади на TypeScript і Dart; дублює `cpp-pro`; у репозиторії скрипти `curl \| bash` і CI з `--dangerously-skip-permissions` |
| Jeffallan `game-developer` | орієнтований на Unity і Unreal, суперечить власному рушію |
| Claude-Code-Game-Studios | фреймворк із хуками на кожну дію й мережевими перевірками; скіли окремо не працюють |
| awesome-gamedev `game-ai`, `ai-behavior-trees-utility-ai` | ігровий AI поки не потрібен; повернутися, коли з'явиться модуль AI |

## Оновлення

Автоматично не оновлювати. Нову версію оригіналу перевірити так само, як вище, порівняти з попередньою
і перенести в адаптований скіл лише корисні зміни.
