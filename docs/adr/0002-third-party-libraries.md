# 0002. Third-party libraries

- Статус: Accepted
- Дата: 2026-11-02

## Контекст
Для курсового потрібні вікно, Vulkan-рендеринг, математика, імпорт glTF-моделей з анімацією і текстур, а також формат для конфігів і сцен. Бібліотек має бути мінімум, і всі вони повинні збиратися на Windows (MSVC) і Linux без додаткових інструментів.

## Розглянуті варіанти
- **Вікно:** GLFW чи SDL3. GLFW менший і має пряму підтримку Vulkan-поверхонь.
- **Виклики Vulkan:** пряме лінкування з `vulkan-1` чи **volk**. volk завантажує функції динамічно, а функції пристрою викликаються без зайвого рівня диспетчеризації.
- **Заголовки Vulkan:** з SDK чи **Vulkan-Headers** як submodule. Submodule робить збирання незалежним від версії SDK і дозволяє збирати CI без SDK.
- **Пам'ять GPU:** вручну чи **VMA** (Vulkan Memory Allocator), де виділення пам'яті та вибір її типу вже вирішено.
- **glTF:** cgltf, fastgltf чи Assimp.
  - Assimp величезний і підтримує FBX, який курсовому не потрібен.
  - fastgltf потребує simdjson.
  - **cgltf** — один заголовок на C із повною підтримкою скінінгу та анімацій.
- **Текстури:** **stb_image**.
- **JSON:** **nlohmann/json**.

## Рішення
GLFW 3.4, GLM 1.0.3, Vulkan-Headers і volk (обидва vulkan-sdk-1.4.363.0), VMA 3.5.0, stb, cgltf 1.15, nlohmann/json 3.12.0.

Деталі підключення:
- Header-only бібліотеки підключаються як власні INTERFACE-цілі (`advent::glm`, `advent::vma`, `advent::json`). Так ми контролюємо макроси: у старій версії GLM примусово вмикав AVX2 на будь-якому процесорі.
- Реалізації stb і cgltf компілюються один раз у `vendor/impl/` (до ADR 0005 — `third_party/impl/`).
- VMA компілюється в `engine/renderer/src/vma_impl.cpp` і бере функції Vulkan з volk (`vmaImportVulkanFunctionsFromVolk`).
- GLFW збирається статично (`BUILD_SHARED_LIBS OFF`), тож не потрібно копіювати DLL поруч з exe.
- Для JSON вимкнено неявні перетворення (`JSON_USE_IMPLICIT_CONVERSIONS=0`) однаково в Debug і Release.

## Наслідки
- Vulkan SDK потрібен лише для validation layers і `glslc`, а не для заголовків.
- Dear ImGui поки не підключено. Якщо знадобиться для демо, його додають окремим ADR.
