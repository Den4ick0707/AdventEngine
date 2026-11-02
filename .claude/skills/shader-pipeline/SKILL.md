---
name: shader-pipeline
description: Шейдери AdventEngine — GLSL, компіляція в SPIR-V через advent_compile_shaders (glslc), розкладка дескрипторів, push constants, графічні пайплайни. Use when writing or changing shaders, descriptor set layouts, push constants or pipeline creation.
---

# Шейдери та пайплайни

## Файли
- Вихідний код: `shaders/<name>.vert|frag|comp` (GLSL 460).
- Спільні визначення: `shaders/common/*.glsl`, підключаються через `#extension GL_GOOGLE_include_directive : require` і `#include "file.glsl"`.
- Результат: `build/<preset>/bin/shaders/<name>.<stage>.spv`, поруч із виконуваними файлами.

## Компіляція (вже готово в cmake/AdventShaders.cmake)
```cmake
advent_compile_shaders(sandbox SOURCES
    ${PROJECT_SOURCE_DIR}/shaders/triangle.vert
    ${PROJECT_SOURCE_DIR}/shaders/triangle.frag
)
```
- Викликає `glslc --target-env=vulkan1.3`; у Debug додає `-g` для RenderDoc.
- Відстежує `#include`: зміна файлу з `shaders/common/` перезбирає залежні шейдери.
- Передає цілі макрос `ADVENT_SHADER_DIR` з абсолютним шляхом до `.spv`. Для релізних збірок завантаження має також шукати `shaders/` поруч із виконуваним файлом.
- `glslc` береться з Vulkan SDK (`VULKAN_SDK`), на Linux також із пакета `glslc`.
- **CI:** на Windows-раннері немає `glslc`. Коли з'являться шейдери, додай у `.github/workflows/ci.yml` і `release.yml` встановлення Vulkan SDK для Windows, наприклад окремий крок із завантаженням SDK. Це нова зовнішня дія, тож погодь її з користувачем.

## Розкладка дескрипторів (конвенція)
| set | Призначення | Частота оновлення |
|-----|-------------|-------------------|
| 0 | Глобальні дані кадру: камера (view, proj), світло, час | раз на кадр |
| 1 | Матеріал: текстури, параметри | на матеріал |
| 2 | Об'єкт: матриці суглобів (SSBO) для скінінгу | на об'єкт |

Push constants (≤ 128 байт): матриця моделі та індекси.

- Uniform-буфери — std140, SSBO — std430. Структури на C++ стороні дублюють розкладку з `alignas(16)` і `static_assert(sizeof(...) == N)`.
- Вершинні атрибути мають збігатися з `VkVertexInputAttributeDescription`. Перехід на vertex pulling — лише через ADR.

## Пайплайни
- Dynamic rendering: `VkPipelineRenderingCreateInfo` з форматами color і depth, `renderPass = VK_NULL_HANDLE`.
- Dynamic state: щонайменше `VIEWPORT` і `SCISSOR`.
- Окремі пайплайни для статичних і скінованих мешів (різні вершинні атрибути).
- Створення пайплайнів збери в builder-клас у `engine/renderer`.

## Перевірка
- `glslc` не видає попереджень.
- Validation layers без помилок про невідповідність інтерфейсів шейдерів чи дескрипторів.
- Концепції ефектів (простори координат, UV, rim light) — у скілі `shader-programming`. Його приклади написані в стилі OpenGL, тож переносити їх треба за правилами цього скілу.
