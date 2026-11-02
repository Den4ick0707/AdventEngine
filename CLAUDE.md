# AdventEngine — 3D-рушій (C++20 / Vulkan 1.3 / CMake)

Навчальний 3D-рушій для курсового проєкту.
Мінімальна мета курсового: 3D-сцена та імпорт моделей зі скелетною анімацією (glTF).

## Мова спілкування
- Відповідай українською.
- Ідентифікатори, коментарі в коді, повідомлення комітів і назви файлів ADR — англійською.

## Поточний етап
Етап 2 дорожньої карти (див. README): є вікно та ініціалізований Vulkan (`VulkanContext`), наступний крок — swapchain і очищення екрана.
- Перш ніж писати нову підсистему, запропонуй дизайн. Рішення, що впливають на архітектуру, фіксуй через `/adr`.
- Рухайся маленькими кроками, кожен з яких збирається, проходить тести й запускається.
- Якщо рішення ще не прийняте (див. «Відкриті рішення»), запитай, а не обирай мовчки.

## Технологічний стек
| Що | Рішення |
|----|---------|
| Мова | C++20 (без C++20-модулів) |
| Платформи | Windows (MSVC 2022), Linux (GCC 13+ / Clang 17+) |
| Збирання | CMake ≥ 3.25 + Ninja, `CMakePresets.json` |
| Залежності | Git submodules у `third_party/`, зафіксовані на тегах (ADR 0001) |
| Графіка | Vulkan 1.3: dynamic rendering, synchronization2 |
| Vulkan | `Vulkan-Headers` + **volk** (завантажувач) + **VMA** (пам'ять) |
| Вікно / ввід | **GLFW** 3.4 (статично) |
| Математика | **GLM** (`GLM_FORCE_DEPTH_ZERO_TO_ONE`, `GLM_FORCE_RADIANS`) |
| Моделі / текстури | **cgltf** (glTF 2.0), **stb_image** |
| Конфіги / сцени | **nlohmann/json** (`JSON_USE_IMPLICIT_CONVERSIONS=0`) |
| Шейдери | GLSL → SPIR-V через `glslc`, функція `advent_compile_shaders` |

## Модулі
| Модуль | Статус | Залежить від | Зміст |
|--------|--------|--------------|-------|
| `core` | ✅ | glm | `log`, `ADVENT_ASSERT`, `FrameClock`, `FixedTimestep` |
| `platform` | ✅ | core, (glfw приватно) | `Window`: події, розмір framebuffer, Vulkan-розширення |
| `renderer` | 🟡 | core, platform, volk, vma | `VulkanContext`, `ADVENT_VK_CHECK`. Далі: swapchain, пайплайни, меші |
| `runtime` | ✅ | core, platform, renderer | `Application` (головний цикл), `Module`, `run_application` |
| `assets` | ⬜ | core, (cgltf, stb приватно) | glTF і текстури → CPU-дані |
| `animation` | ⬜ | core | скелет, кліпи, семплінг, матриці суглобів |
| `scene` | ⬜ | core, animation | сутності, трансформації, ієрархія |

Застосунки — в `apps/` (зараз `apps/sandbox`), тести — в `tests/<module>/`, шейдери — в `shaders/`, моделі й текстури — в `assets/` (Git LFS).

### Правила залежностей
- `core` не залежить ні від чого, крім GLM.
- `assets` і `animation` повертають CPU-дані й нічого не знають про Vulkan.
- Vulkan-типи живуть лише в `renderer` (і в публічному API `runtime` через `VulkanContext&`).
- GLFW видно лише в `platform/src` і `renderer/src`.
- Циклічні залежності заборонені. Якщо вони знадобилися, перегляньте дизайн через ADR.

## Конвенції коду (перевіряються `.clang-tidy`)
- Файли — `snake_case.hpp` / `.cpp`. Публічні заголовки лежать у `engine/<module>/include/advent/<module>/` і підключаються як `<advent/<module>/x.hpp>`.
- Типи, enum і concept — `PascalCase`; функції, методи, змінні та значення enum — `snake_case`.
- Члени класу — префікс `m_`, `constexpr`-константи — `k_`, глобальні змінні — `g_`, макроси — `ADVENT_UPPER_CASE`.
- Простори імен: `advent` для core, `advent::<module>` для решти.
- `#pragma once`. Кожен заголовок сам підключає все, що використовує (PCH немає).
- Форматування — `.clang-format`. Після змін запускай `clang-format -i`.
- RAII для всіх ресурсів. Сирі `new`/`delete` заборонені.
- **Помилки:**
  - ініціалізація кидає `std::runtime_error` або `renderer::VulkanError`; `run_application` ловить, логує і повертає код 1;
  - у гарячому шляху кадру винятків немає, крім фатальних Vulkan-помилок;
  - помилки програміста перевіряються через `ADVENT_ASSERT`.
- Кожен `VkResult` перевіряється через `ADVENT_VK_CHECK` (винятки з цього правила — у скілі `/vulkan-renderer`).
- Лог: `advent::log::info("x = {}", x)` — синтаксис `std::format`. `std::cout` не використовуємо.
- Глобальний стан заборонений, крім явно позначеного (`g_`).

## Команди
```bash
git submodule update --init --recursive --depth 1   # після клонування
cmake --preset debug                       # конфігурація (Debug + validation layers)
cmake --build --preset debug               # збирання
ctest --preset debug                       # тести
./build/debug/bin/sandbox --frames 120     # запуск sandbox на 120 кадрів
```
Пресети `ci-debug` і `ci-release` збирають так само, як CI (попередження = помилки). Подробиці — у скілі `/build-test`.

## Git і GitHub
- Гілки: `main` — стабільна; `dev` — інтеграційна; `feature/<коротко>` — робочі гілки. Злиття — через Pull Request у `dev`.
- CI (`.github/workflows/ci.yml`) має бути зеленим перед злиттям.
- Коміти англійською в наказовому способі: "Add swapchain recreation".
- Реліз: тег `vX.Y.Z` запускає `.github/workflows/release.yml` (чернетка GitHub Release).

## Скіли
### Скіли проєкту (головні правила)
- `/engine-module` — новий модуль рушія.
- `/vulkan-renderer` — правила Vulkan-коду і наступні кроки рендерера.
- `/shader-pipeline` — шейдери, `advent_compile_shaders`, дескриптори.
- `/skeletal-animation` — glTF через cgltf, скелет, анімації, скінінг.
- `/build-test` — збирання, тести, CI, submodules.
- `/adr` — фіксація архітектурних рішень (`docs/adr/`).

### Сторонні скіли (перевірені, див. `.claude/third-party-skills/README_UA.md`)
- Мова: `cpp-pro`
- Ядро рушія: `physics-tuning` (фіксований крок + інтерполяція), `performance-optimization`
- Підсистеми: `camera-systems`, `input-systems`, `shader-programming`

### Пріоритет при конфліктах
1. Цей файл.
2. Скіли проєкту.
3. Сторонні скіли — лише джерело концепцій. Приклади на GDScript, C#, TypeScript чи OpenGL-GLSL переноси на C++20 і Vulkan 1.3 за правилами проєкту, а не копіюй.

Залежності додаються лише як git submodules, навіть якщо сторонній скіл радить FetchContent, Conan чи vcpkg.

## Прийняті рішення (docs/adr/)
- 0001 — залежності через git submodules, зафіксовані на тегах.
- 0002 — набір бібліотек: GLFW, GLM, Vulkan-Headers, volk, VMA, stb, cgltf, nlohmann/json.
- 0003 — стиль коду: snake_case, `namespace advent`.
- 0004 — головний цикл з фіксованим кроком симуляції.

## Відкриті рішення (запитай перед вибором)
- Організація сцени: простий граф сцени чи ECS (EnTT або власний).
- Тестовий фреймворк: doctest чи Catch2. Поки що використовуються мінімальні хелпери з `tests/test_support.hpp`.
- Налагоджувальний UI (Dear ImGui): поки не обрано.
- Напрям осі Y і спосіб її інвертування в проєкції (див. `/vulkan-renderer`).

## Чого не робити
- Не додавай залежності без запиту. Кожна нова бібліотека — це новий submodule і запис в ADR.
- Не вимикай validation layers у Debug, щоб «прибрати помилки», — виправляй причину.
- Не змінюй структуру модулів і правила залежностей без ADR.
- Не копіюй сторонній код у репозиторій (стара версія мала 2 500 файлів бібліотек в історії Git — див. `docs/legacy-review.md`).
