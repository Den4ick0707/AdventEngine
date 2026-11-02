---
name: vulkan-renderer
description: Правила та патерни для Vulkan-коду AdventEngine (VulkanContext, volk, VMA, swapchain, синхронізація, dynamic rendering, кадри в польоті). Use when writing or reviewing any Vulkan code, render loop, buffers, images, pipelines, swapchain or GPU synchronization in engine/renderer.
---

# Vulkan-рендерер AdventEngine

Цільова версія — **Vulkan 1.3**, обов'язкові можливості: `dynamicRendering` і `synchronization2`.
Render pass і framebuffer-об'єкти **не використовуємо**, лише `vkCmdBeginRendering`.

## Що вже є (engine/renderer)
- `VulkanContext` (`vulkan_context.hpp/.cpp`) створює:
  - instance через volk;
  - debug messenger, коли validation увімкнено;
  - surface з `platform::Window`;
  - вибір GPU з оцінкою (discrete > integrated > cpu);
  - device з увімкненими 1.3-можливостями та `samplerAnisotropy`, якщо він підтримується;
  - черги graphics і present;
  - `VmaAllocator`.
- Гетери: `instance()`, `device()`, `physical_device()`, `surface()`, `allocator()`, `graphics_queue()`, `present_queue()`, `queue_families()`, `device_properties()`, `validation_enabled()`.
- `set_debug_name(type, handle, name)` — імена об'єктів для validation і RenderDoc.
- `ADVENT_VK_CHECK(expr)` з `vulkan_check.hpp` кидає `VulkanError`, якщо `VkResult < 0`.
- `Application` (`engine/runtime`) володіє `Window` і `VulkanContext`, а наприкінці `run()` викликає `wait_idle()`.

## Наступні кроки за дорожньою картою
1. Клас `Swapchain`: формат `B8G8R8A8_SRGB` або перший доступний, present mode FIFO (vsync), перебудова за `window.consume_resize()`, при `OUT_OF_DATE` і `SUBOPTIMAL`. Поки `window.is_minimized()`, нічого не робити.
2. Кадри в польоті та команди: див. «Цикл кадру».
3. Очищення екрана кольором через `vkCmdBeginRendering` з переходами layout через `vkCmdPipelineBarrier2`.
4. Після цього приберіть `max_fps` у sandbox, бо кадри обмежуватиме FIFO.

## Обов'язкові правила
- Кожен `VkResult` перевіряється через `ADVENT_VK_CHECK`. Виняток — `vkAcquireNextImageKHR` і `vkQueuePresentKHR`: `VK_ERROR_OUT_OF_DATE_KHR` і `VK_SUBOPTIMAL_KHR` там означають «перебудуй swapchain».
- Усі хендли — в RAII-класах або deletion queue; знищення у зворотному порядку створення. Якщо конструктор кидає виняток, він сам прибирає вже створене (зразок — `VulkanContext::destroy()`).
- Буфери й зображення створюються лише через VMA (`vmaCreateBuffer`, `vmaCreateImage`), без ручного `vkAllocateMemory`.
- Бар'єри ставляться лише через `vkCmdPipelineBarrier2` і `VkDependencyInfo`. `ALL_COMMANDS` не використовувати як «латку».
- Vulkan-заголовки підключаються лише через `<volk.h>`, ніколи через `<vulkan/vulkan.h>` напряму. `<vk_mem_alloc.h>` — після `<volk.h>`.
- Vulkan-функції не викликаються до `volkLoadInstanceOnly` і `volkLoadDevice`: це вже зроблено у `VulkanContext`.

## Цикл кадру
- `k_frames_in_flight = 2`. На кожен кадр: command pool і buffer, `VkFence` (in-flight), семафор image-available.
- Семафори render-finished — **по одному на зображення swapchain**, а не на кадр.
- Порядок: wait fence → acquire → reset fence → record → `vkQueueSubmit2` → present.
- Ресурси, що оновлюються щокадру (uniform-буфери, SSBO матриць), дублюються на кожен кадр у польоті.

## Ресурси
- Статичні вершинні та індексні буфери: staging-буфер (`VMA_MEMORY_USAGE_AUTO` + `HOST_ACCESS_SEQUENTIAL_WRITE`) → копіювання в device-local пам'ять.
- Буфери, що оновлюються щокадру: host-visible, постійно змаплені (`VMA_ALLOCATION_CREATE_MAPPED_BIT`).
- Depth: `VK_FORMAT_D32_SFLOAT`. GLM уже налаштований на діапазон [0, 1] (`GLM_FORCE_DEPTH_ZERO_TO_ONE`).

## Межі модуля
Vulkan-типи живуть лише в `engine/renderer`. Інші модулі передають CPU-дані (`assets::MeshData`, `animation::Pose`), а рендерер сам завантажує їх на GPU.

## Типові помилки
- Зображення swapchain не переведене в `PRESENT_SRC_KHR`.
- Вирівнювання std140: `vec3` займає 16 байт. Краще `vec4` або std430 у SSBO.
- Вісь Y у Vulkan спрямована вниз: обери один спосіб (інвертувати `proj[1][1]` або від'ємна висота viewport) і зафіксуй через ADR.

## Перевірка
- `ctest --preset debug`, потім `./build/debug/bin/sandbox --frames 300`. У лозі не має бути жодного `[vulkan]` warn чи error.
- Для синхронізації — `vkconfig` з увімкненою «Synchronization validation». Для аналізу кадру — RenderDoc.
- Довідка: vkguide.dev, docs.vulkan.org, специфікація (шукай за `VUID-...` з повідомлення).
