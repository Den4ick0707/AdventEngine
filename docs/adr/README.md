# Architecture Decision Records

Кожне суттєве архітектурне рішення фіксується окремим файлом: контекст, розглянуті варіанти, рішення, наслідки.
Прийнятий ADR не редагується по суті; нове рішення оформлюється новим ADR, а старий отримує статус `Superseded by NNNN`.
Поточна картина з усіх рішень — [архітектура](../architecture.md).

| № | Рішення | Статус |
|---|---------|--------|
| [0001](0001-git-submodules-for-dependencies.md) | Залежності через git submodules, зафіксовані на тегах | Superseded by 0005 |
| [0002](0002-third-party-libraries.md) | Набір сторонніх бібліотек | Accepted |
| [0003](0003-code-style.md) | Стиль коду: snake_case і `namespace advent` | Accepted |
| [0004](0004-main-loop-fixed-timestep.md) | Головний цикл із фіксованим кроком симуляції | Accepted |
| [0005](0005-vendored-dependencies.md) | Залежності — копії в `vendor/` (офлайн, локальні правки) | Accepted |
| [0006](0006-modular-architecture.md) | Модульна архітектура: шари, kernel, статичні модулі, редактор окремим застосунком | Accepted |
| [0007](0007-documentation-tooling.md) | Документація: сайт Doxygen із ручними розділами, довідником класів і діаграмами | Accepted |
