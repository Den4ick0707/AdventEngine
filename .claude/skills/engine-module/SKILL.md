---
name: engine-module
description: Створення нового модуля AdventEngine (CMake-ціль advent_<module>, include-структура, namespace, тести). Use when adding a new engine module/subsystem such as assets, animation or scene, creating a CMake library target, or restructuring module boundaries — "новий модуль", "додай підсистему", "add module".
---

# Новий модуль AdventEngine

Модуль — статична бібліотека CMake з чіткими публічними заголовками в `engine/<module>/`.
Зразки для наслідування: `engine/core` (без залежностей) і `engine/platform` (з приватною залежністю GLFW).

## Перед початком
1. Перевір правила залежностей у `CLAUDE.md`. Якщо новий модуль їх порушує — зупинись і запропонуй ADR (`/adr`).
2. Уточни в користувача: назву, відповідальність модуля (одне речення), від яких модулів він залежить.

## Структура
```
engine/<module>/
  CMakeLists.txt
  include/advent/<module>/   # публічні заголовки
  src/                       # реалізація та приватні заголовки
tests/<module>/              # тести модуля
```

## CMakeLists.txt модуля (шаблон)
```cmake
add_library(advent_<module> STATIC)
add_library(advent::<module> ALIAS advent_<module>)

target_sources(advent_<module>
    PRIVATE
        src/<file>.cpp
)

target_include_directories(advent_<module>
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(advent_<module>
    PUBLIC  advent::core     # лише те, що видно в публічних заголовках
    PRIVATE                  # реалізаційні залежності, напр. advent::cgltf
)
advent_set_warnings(advent_<module>)   # cmake/AdventWarnings.cmake
```
Файли перелічуй явно в `target_sources` — без `file(GLOB ...)`.

## Правила
- Публічні заголовки підключаються як `#include <advent/<module>/foo.hpp>`; `#pragma once`.
- Кожен заголовок сам підключає все, що використовує (PCH немає).
- Залежність `PUBLIC` — лише якщо її типи з'являються в публічних заголовках, інакше `PRIVATE`.
- Сторонні бібліотеки підключай через цілі з `vendor/CMakeLists.txt` (`advent::glm`, `advent::stb`, `advent::cgltf`, `advent::json`, `glfw`, `advent::volk`, `advent::vma`).
- Vulkan-типи дозволені лише в `renderer`; GLFW — лише в `platform` і (приватно) `renderer`.
- Код у `namespace advent::<module>`; для `core` — просто `advent`.
- Додай `add_subdirectory(<module>)` в `engine/CMakeLists.txt` з урахуванням порядку залежностей і прибери модуль зі списку «Planned modules» там.
- Створи тест у `tests/<module>/` і зареєструй його через `advent_add_test(...)` у `tests/CMakeLists.txt`.

## Перевірка
- `cmake --preset ci-debug && cmake --build --preset ci-debug` — без попереджень (у пресетах `ci-*` вони вважаються помилками).
- `ctest --preset ci-debug` проходить.
- Онови в `CLAUDE.md` таблицю модулів (статус і залежності).
