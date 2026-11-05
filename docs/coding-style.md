# Стиль коду

Правила перевіряються автоматично: форматування — `.clang-format`, імена й типові помилки — `.clang-tidy`.
Обґрунтування стилю — [ADR 0003](adr/0003-code-style.md).

## Мова і стандарт

- C++20 без модулів C++20. Компілятори: GCC 13+, Clang 17+, MSVC 2022.
- Ідентифікатори, коментарі, повідомлення комітів — англійською.

## Імена

| Що | Стиль | Приклад |
|----|-------|---------|
| Типи, enum, concept | `PascalCase` | `VulkanContext`, `Level` |
| Функції, методи, змінні, параметри | `snake_case` | `poll_events()`, `frame_count` |
| Значення enum | `snake_case` | `Level::warn` |
| Члени класу | префікс `m_` | `m_window` |
| `constexpr`-константи | префікс `k_` | `k_frames_in_flight` |
| Глобальні змінні | префікс `g_` | `g_window_count` |
| Макроси | `ADVENT_UPPER_CASE` | `ADVENT_ASSERT` |
| Простори імен | `advent` для `core`, `advent::<module>` для решти | `advent::renderer` |
| Файли | `snake_case.hpp`, `snake_case.cpp` | `vulkan_context.hpp` |

## Файли і заголовки

- Публічні заголовки модуля: `engine/<module>/include/advent/<module>/`, підключаються як `<advent/<module>/x.hpp>`.
- Приватні заголовки і реалізація: `engine/<module>/src/`.
- `#pragma once`. Кожен заголовок сам підключає все, що використовує; спільного PCH немає.
- У публічних заголовках — лише ті залежності, типи яких видно в API. Решта підключається в `.cpp`.
- Файли перелічуються в `target_sources` явно, без `file(GLOB)`.

## Ресурси і пам'ять

- RAII для всіх ресурсів: пам'ять, файли, хендли Vulkan, вікна.
- Сирі `new` і `delete` заборонені. Власність — `std::unique_ptr`, спільна власність — лише за явної потреби.
- Класи, що володіють ресурсом, забороняють копіювання; переміщення — лише якщо воно коректне.
- Глобальний стан заборонений, крім явно позначеного префіксом `g_` і обґрунтованого.

## Помилки

| Ситуація | Механізм |
|----------|----------|
| Помилка ініціалізації (немає GPU, файлу, драйвера) | виняток `std::runtime_error` або `renderer::VulkanError` |
| Помилка програміста (порушений інваріант, неправильний аргумент) | `ADVENT_ASSERT(condition, "message")`, лише в Debug |
| Результат Vulkan-виклику | `ADVENT_VK_CHECK(expr)`, кидає `VulkanError` при `VkResult < 0` |
| Гарячий шлях кадру | без винятків, крім фатальних помилок Vulkan |

`run_application<App>()` ловить винятки, пише їх у лог і повертає код 1.
У `ADVENT_ASSERT` не можна писати вирази з побічними ефектами: у Release умова не обчислюється.

## Логування

```cpp
advent::log::info("Window created: {}x{}", width, height);   // синтаксис std::format
```

Рівні: `trace`, `debug`, `info`, `warn`, `error`. `std::cout` і `printf` для діагностики не використовуються.

## Документування коду

Публічний API документується коментарями `///` у заголовках. Doxygen збирає їх у довідник (`./dev docs`).

```cpp
/// Owns the core Vulkan objects shared by the whole renderer.
///
/// Construction throws VulkanError or std::runtime_error if no suitable GPU is found.
class VulkanContext
{
public:
    /// Gives a Vulkan object a readable name for validation messages and RenderDoc.
    /// Does nothing when validation is disabled.
    void set_debug_name(VkObjectType type, std::uint64_t handle, const char* name) const;
};
```

- Перше речення — короткий опис (стає заголовком у довіднику), далі деталі.
- Описується що робить і за яких умов, а не як.
- Звичайні `//` — для пояснень усередині реалізації. Коментар пояснює чому, а не повторює код.

## Форматування

- `.clang-format`: відступ 4 пробіли, рядок до 120 символів, фігурні дужки класів і функцій з нового рядка.
- Перед комітом: `./dev format`. Хук `pre-commit` перевіряє змінені файли.
- Код у `vendor/` не форматується і не перевіряється.
