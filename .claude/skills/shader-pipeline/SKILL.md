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

## Ресурси (bindless, ADR 0021)
- Глобальні масиви текстур і семплерів, прив'язуються раз на кадр; доступ — `textures[nonuniformEXT(index)]`.
- Дані кадру, матеріалів, об'єктів — буфери через `buffer device address` (`GL_EXT_buffer_reference`).
- Push constants — лише індекси й адреси буферів, до 128 байт.
- Спільний код (матеріали, освітлення, доступ до масивів) — лише в `shaders/common/`, не дублюється.
- SSBO і буфери за адресою — std430. C++-структури: `alignas(16)` і `static_assert(sizeof(T) == N)`.

## Пайплайни
- `VkPipelineRenderingCreateInfo` з форматами color і depth, `renderPass = VK_NULL_HANDLE`.
- Dynamic state: щонайменше `VIEWPORT` і `SCISSOR`.
- Скінінг — compute-прохід, тож окремих пайплайнів для скінованих мешів немає. Створення — через builder у `renderer`.
- Reverse-Z: `compareOp = GREATER`, глибина очищається в 0.

## Перевірка
- `glslc` без попереджень, validation без помилок інтерфейсу шейдерів і дескрипторів.
