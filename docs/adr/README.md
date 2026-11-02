# Architecture Decision Records

Кожне суттєве архітектурне рішення AdventEngine фіксується окремим файлом. Як писати новий запис — див. скіл `/adr` (`.claude/skills/adr/SKILL.md`).

| № | Рішення | Статус |
|---|---------|--------|
| [0001](0001-git-submodules-for-dependencies.md) | Залежності через git submodules, зафіксовані на тегах | Superseded by 0005 |
| [0002](0002-third-party-libraries.md) | Набір сторонніх бібліотек | Accepted |
| [0003](0003-code-style.md) | Стиль коду: snake_case і `namespace advent` | Accepted |
| [0004](0004-main-loop-fixed-timestep.md) | Головний цикл із фіксованим кроком симуляції | Accepted |
| [0005](0005-vendored-dependencies.md) | Залежності — копії в `vendor/` (офлайн, локальні правки) | Accepted |
