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

Рішення — [ADR 0008](adr/0008-data-oriented-memory.md).

- RAII для всіх ресурсів: пам'ять, файли, хендли Vulkan, вікна.
- Кожен модуль виділяє пам'ять через свій `TrackedResource`. У коді `engine/`:

  | Замість | Використовувати |
  |---------|-----------------|
  | `std::vector<T>` | `std::pmr::vector<T>` з ресурсом модуля |
  | `std::string` | `std::pmr::string`; для параметрів — `std::string_view` |
  | `std::make_unique<T>`, `new` | `advent::make_unique<T>(resource, ...)` |
  | тимчасові дані кадру | `FrameArena` |

  Перевіряє `./dev` (і CI). Обґрунтований виняток — коментар `// advent-allow: std-alloc` у рядку.
  Тести й застосунки правило не зачіпає. Перевірка вмикається, щойно в `core` з'являться ресурси пам'яті
  (етап 2 плану); до того нові `std::vector` у `engine/` не додаються без потреби.
- Гарячі масові дані — SoA (`SoaPool`), поодинокі об'єкти й конфігурації — звичайні структури.
- Між модулями — handles або `std::span`, не сирі вказівники на чужі дані.
- У гарячому шляху кадру — без виділень пам'яті, крім `FrameArena`.
- Класи, що володіють ресурсом, забороняють копіювання; переміщення — лише якщо воно коректне.
- Маленькі значення (результати, вектори, handles) повертаються за значенням.
- Глобальний стан заборонений, крім явно позначеного префіксом `g_` і обґрунтованого.

## Помилки

Рішення — [ADR 0010](adr/0010-error-handling.md).

| Ситуація | Механізм |
|----------|----------|
| Провал запуску (немає GPU, драйвера, обов'язкового файлу) | виняток `std::runtime_error` або `renderer::VulkanError` |
| Помилка в нормальній роботі (файл, імпорт, парсинг, модуль) | `Result<T>`; помилка передається вгору явно |
| Помилка програміста (порушений інваріант, неправильний аргумент) | `ADVENT_ASSERT(condition, "message")`, лише в Debug |
| Результат Vulkan-виклику | `ADVENT_VK_CHECK(expr)`, кидає `VulkanError` при `VkResult < 0` |
| Гарячий шлях кадру | без винятків, крім фатальних помилок Vulkan |

```cpp
Result<MeshData> load_mesh(std::string_view path);

auto mesh = load_mesh(path);
if (!mesh) {
    return mesh.error();
}
```

`run_application<App>()` ловить винятки, пише їх у лог і повертає код 1.
У `ADVENT_ASSERT` не можна писати вирази з побічними ефектами: у Release умова не обчислюється.

## Логування

Рішення — [ADR 0011](adr/0011-logging.md).

```cpp
inline constexpr log::Category k_log{"renderer"};           // одна категорія на модуль
log::info(k_log, "Window created: {}x{}", width, height);   // синтаксис std::format
```

- Рівні: `trace`, `debug`, `info`, `warn`, `error`. `trace` і `debug` вирізаються в Release.
- `std::cout` і `printf` для діагностики не використовуються.
- До переходу на категорії (етап 2 плану) діє поточний API без категорії: `log::info("...")`.

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
