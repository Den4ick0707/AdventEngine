---
name: engine-module
description: Створення нового модуля AdventEngine — CMake-ціль advent_<module>, структура include/src, namespace, тести, місце в шарах. Use when adding an engine module or subsystem (assets, animation, scene, editor, raytracing), creating a library target or changing module boundaries — "новий модуль", "додай підсистему".
---

# Новий модуль

## Перед початком
1. Звір із `docs/architecture.md`: шар модуля, від кого він залежить, чи не порушує правил залежностей.
   Порушення або новий шар — зупинись і запропонуй ADR.
2. Узгодь з користувачем: назву, відповідальність одним реченням, залежності, публічний API.

## Структура
```
engine/<module>/
  CMakeLists.txt
  include/advent/<module>/   публічні заголовки
  src/                       реалізація і приватні заголовки
tests/<module>/
```

## CMakeLists.txt
```cmake
add_library(advent_<module> STATIC)
add_library(advent::<module> ALIAS advent_<module>)

target_sources(advent_<module> PRIVATE src/<file>.cpp)

target_include_directories(advent_<module>
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(advent_<module>
    PUBLIC  advent::core     # лише те, чиї типи видно в публічних заголовках
    PRIVATE                  # реалізація: advent::cgltf, glfw, ...
)
advent_set_warnings(advent_<module>)
```

## Правила
- Простір імен `advent::<module>`. Публічні заголовки документовані `///` (Doxygen).
- Залежність `PUBLIC` — лише якщо її типи є в публічному API.
- Межа модуля груба: обмін масивами даних, без віртуальних викликів на кожен об'єкт.
- Поки система модулів (маніфести, реєстр у kernel) не готова, модуль підключається через
  `add_subdirectory` в `engine/CMakeLists.txt` у порядку залежностей.

## Тести
- `tests/<module>/<name>_test.cpp`, реєстрація: `advent_add_test(<module>_<name>_test <module>/<name>_test.cpp)`
  у `tests/CMakeLists.txt`. Якщо тесту потрібен модуль, додай його в `target_link_libraries` тесту.

## Готово, коли
- `./dev ci` проходить.
- `docs/architecture.md`: модуль у таблиці «Поточний стан».
