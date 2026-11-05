---
name: vulkan-renderer
description: Правила Vulkan-коду AdventEngine — VulkanContext, volk, VMA, swapchain, синхронізація, dynamic rendering, кадри в польоті, підготовка до трасування променів. Use when writing or reviewing any Vulkan code, render loop, buffers, images, pipelines, swapchain or GPU synchronization in engine/renderer.
---

# Vulkan-рендерер

Базові рішення (Vulkan 1.3, volk, VMA, кадри в польоті, трасування променів) — `docs/architecture.md`, розділ «Рендерер».

## Що вже є
- `VulkanContext`: instance, debug messenger, surface, вибір GPU (discrete > integrated > cpu), device з
  `dynamicRendering`, `synchronization2`, `samplerAnisotropy` (якщо є), черги graphics і present, `VmaAllocator`.
- `set_debug_name(type, handle, name)` — імена об'єктів для validation і RenderDoc.
- `ADVENT_VK_CHECK(expr)` кидає `VulkanError` при `VkResult < 0`.

## Правила
- Кожен `VkResult` — через `ADVENT_VK_CHECK`. Виняток: `vkAcquireNextImageKHR` і `vkQueuePresentKHR`,
  де `VK_ERROR_OUT_OF_DATE_KHR` і `VK_SUBOPTIMAL_KHR` означають перебудову swapchain.
- Хендли — у RAII-класах; знищення у зворотному порядку створення. Конструктор, що кинув виняток,
  прибирає вже створене (зразок — `VulkanContext::destroy()`).
- Буфери й зображення — лише через VMA, без `vkAllocateMemory`.
- Бар'єри — лише `vkCmdPipelineBarrier2` з `VkDependencyInfo`. `ALL_COMMANDS` не використовувати як латку.
- Заголовки — лише `<volk.h>`, потім `<vk_mem_alloc.h>`. Ніколи `<vulkan/vulkan.h>` напряму.
- Render pass і framebuffer не використовуються: лише `vkCmdBeginRendering`.
- Vulkan-типи не виходять за межі модуля `renderer`.

## Цикл кадру
- `k_frames_in_flight = 2`. На кадр: command pool і buffer, fence, семафор image-available.
- Семафори render-finished — по одному на зображення swapchain, не на кадр.
- Порядок: wait fence, acquire, reset fence, record, `vkQueueSubmit2`, present.
- Ресурси, що змінюються щокадру, дублюються на кожен кадр у польоті.

## Swapchain
- Формат `B8G8R8A8_SRGB` або перший доступний; present mode FIFO.
- Перебудова: `window.consume_resize()`, `OUT_OF_DATE`, `SUBOPTIMAL`. Поки `window.is_minimized()`, кадр не малюється.
- Після появи FIFO прибрати `max_fps` у sandbox.

## Ресурси
- Статичні буфери: staging (`VMA_MEMORY_USAGE_AUTO` + `HOST_ACCESS_SEQUENTIAL_WRITE`), копіювання в device-local.
- Буфери, що змінюються щокадру: host-visible, постійно змаплені (`VMA_ALLOCATION_CREATE_MAPPED_BIT`).
- Depth: `VK_FORMAT_D32_SFLOAT`, reverse-Z (ближня 1, дальня 0, `GREATER`); GLM з `GLM_FORCE_DEPTH_ZERO_TO_ONE`.
- Проходи — через render graph: оголосити читання і запис, бар'єри ставить граф (ADR 0021).
- Трасування променів: можливості `VK_KHR_acceleration_structure`, `VK_KHR_ray_tracing_pipeline`,
  `VK_KHR_ray_query` — необов'язкові. Коли вони є, вершинні й індексні буфери отримують
  `SHADER_DEVICE_ADDRESS` і `ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY`.

## Типові помилки
- Зображення swapchain не переведене в `PRESENT_SRC_KHR`.
- std140: `vec3` займає 16 байт; краще `vec4` або std430.
- Вісь Y у Vulkan спрямована вниз; інвертується від'ємною висотою viewport (ADR 0021).

## Перевірка
- `./dev smoke ci-debug`: нуль `[vulkan:validation]`.
- Синхронізація: `vkconfig` з Synchronization validation. Аналіз кадру: RenderDoc.
