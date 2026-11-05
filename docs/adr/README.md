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
| [0008](0008-data-oriented-memory.md) | Data-oriented design, пам'ять через `std::pmr` з обліком по модулях, `SoaPool` | Accepted |
| [0009](0009-identifiers.md) | Ідентифікатори: handles, UUID 128 біт, `StringId` | Accepted |
| [0010](0010-error-handling.md) | Помилки: винятки при запуску, `Result<T>` у роботі, `ADVENT_ASSERT` | Accepted |
| [0011](0011-logging.md) | Лог: категорії, кілька sink-ів, файл завжди, без виділень пам'яті | Accepted |
| [0012](0012-virtual-file-system-and-projects.md) | Віртуальна файлова система, файл проєкту `.advent`, налаштування користувача | Accepted |
| [0013](0013-input-layers.md) | Ввід: сирий стан пристроїв у platform, дії в модулі `input` | Accepted |
| [0014](0014-single-threaded-frame.md) | Потоки: однопотоковий кадр зараз, система задач пізніше | Accepted |
| [0015](0015-module-communication.md) | Зв'язки модулів: залежності, сервіси, події-масиви із закладками, спільні дані | Accepted |
| [0016](0016-frame-phases-and-systems.md) | Фази кадру і системи модулів з явним порядком | Accepted |
