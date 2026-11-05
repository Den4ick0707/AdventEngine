---
name: shader-pipeline
description: Шейдери AdventEngine — GLSL, компіляція в SPIR-V через advent_compile_shaders (glslc), розкладка дескрипторів, push constants, графічні пайплайни. Use when writing or changing shaders, descriptor set layouts, push constants or pipeline creation.
---

# Шейдери і пайплайни

## Файли
- Вихідний код: `shaders/<name>.vert|frag|comp`, GLSL 460.
- Спільне: `shaders/common/*.glsl`, підключення через `GL_GOOGLE_include_directive` і `#include`.
- Результат: `build/<preset>/bin/shaders/<name>.<stage>.spv`.

## Компіляція
```cmake
advent_compile_shaders(sandbox SOURCES ${PROJECT_SOURCE_DIR}/shaders/triangle.vert ${PROJECT_SOURCE_DIR}/shaders/triangle.frag)
```
- `glslc --target-env=vulkan1.3`, у Debug `-g`; зміни в `#include` відстежуються.
- Цілі передається `ADVENT_SHADER_DIR`. Релізне завантаження також шукає `shaders/` поруч із виконуваним файлом.
- Windows CI не має `glslc`. Коли з'являться перші шейдери, потрібно встановлення Vulkan SDK у
  `ci.yml` і `release.yml` — погодь з користувачем.

## Дескриптори
Організація (класичні набори чи bindless) — відкрите питання теми «Рендерер», вирішується через ADR.
Тимчасова конвенція до рішення:

| set | Дані | Частота |
|-----|------|---------|
| 0 | кадр: камера, світло, час | раз на кадр |
| 1 | матеріал: текстури, параметри | на матеріал |
| 2 | об'єкт: матриці суглобів (SSBO) | на об'єкт |

- Push constants до 128 байт: матриця моделі, індекси.
- Uniform — std140, SSBO — std430. C++-структури: `alignas(16)` і `static_assert(sizeof(T) == N)`.

## Пайплайни
- `VkPipelineRenderingCreateInfo` з форматами color і depth, `renderPass = VK_NULL_HANDLE`.
- Dynamic state: щонайменше `VIEWPORT` і `SCISSOR`.
- Окремі пайплайни для статичних і скінованих мешів. Створення — через builder у `renderer`.

## Перевірка
- `glslc` без попереджень, validation без помилок інтерфейсу шейдерів і дескрипторів.
