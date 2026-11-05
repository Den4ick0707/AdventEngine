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
| Залежності | Копії бібліотек у `vendor/` — офлайн-збирання, локальні правки (ADR 0005) |
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

Застосунки — в `apps/` (зараз `apps/sandbox`), тести — в `tests/<module>/`, шейдери — в `shaders/`, моделі й текстури — в `assets/` (Git LFS), сторонні бібліотеки — у `vendor/`, профілі запуску `./dev` — у `scripts/runs/`.

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
Головний інструмент — `./dev` (`./dev help`). CI викликає ті самі команди.
```bash
./dev build [preset]          # configure за потреби + збирання (за замовчуванням debug)
./dev test [preset]           # збирання + ctest
./dev run [preset] -- --frames 120
./dev smoke [preset]          # sandbox без дисплея; падає на [error] і [vulkan:validation|performance]-попередженнях
./dev format | ./dev tidy     # clang-format / clang-tidy лише для engine/, apps/, tests/
./dev ci                      # усе, що перевіряє CI — запускай перед комітом
./dev p <profile>             # профілі запуску з scripts/runs/
```
Пресети `ci-debug` і `ci-release` збирають так само, як CI (попередження = помилки). Без скрипта: `cmake --preset debug`, `cmake --build --preset debug`, `ctest --preset debug`. Подробиці — у скілі `/build-test`.

## Git і GitHub (подробиці — `CONTRIBUTING.md`)
- Гілки: `main` — лише релізи (з тегами); `dev` — інтеграційна і за замовчуванням; `feature/<коротко>` → PR у `dev` (squash); `release/X.Y.Z` з `dev` → PR у `main` (merge commit) → тег → back-merge `main` у `dev`; `hotfix/X.Y.Z` з `main` → `main`.
- У `main`, `dev`, `release/*` — лише через PR із зеленою перевіркою `ci-ok` (rulesets, див. «Налаштування GitHub» у `CONTRIBUTING.md`).
- Коміти англійською в наказовому способі, з великої літери, ≤ 72 символів: "Add swapchain recreation". Зміни у `vendor/` — окремими комітами `vendor(<lib>): ...`. Перевіряє хук `.githooks/commit-msg`.
- Реліз: тег `vX.Y.Z` на `main` (має збігатися з `VERSION` у `CMakeLists.txt`) запускає `release.yml` (чернетка GitHub Release). Новини — у `CHANGELOG.md`.

## Скіли
### Скіли проєкту (головні правила)
- `/engine-module` — новий модуль рушія.
- `/vulkan-renderer` — правила Vulkan-коду і наступні кроки рендерера.
- `/shader-pipeline` — шейдери, `advent_compile_shaders`, дескриптори.
- `/skeletal-animation` — glTF через cgltf, скелет, анімації, скінінг.
- `/build-test` — збирання, тести, CI, `./dev`, бібліотеки у `vendor/`.
- `/adr` — фіксація архітектурних рішень (`docs/adr/`).

### Сторонні скіли (перевірені, див. `.claude/third-party-skills/README_UA.md`)
- Мова: `cpp-pro`
- Ядро рушія: `physics-tuning` (фіксований крок + інтерполяція), `performance-optimization`
- Підсистеми: `camera-systems`, `input-systems`, `shader-programming`

### Пріоритет при конфліктах
1. Цей файл.
2. Скіли проєкту.
3. Сторонні скіли — лише джерело концепцій. Приклади на GDScript, C#, TypeScript чи OpenGL-GLSL переноси на C++20 і Vulkan 1.3 за правилами проєкту, а не копіюй.

Залежності додаються лише як копії у `vendor/` (ADR 0005), навіть якщо сторонній скіл радить FetchContent, Conan чи vcpkg.

## Прийняті рішення (docs/adr/)
- 0001 — залежності через git submodules (замінено 0005).
- 0002 — набір бібліотек: GLFW, GLM, Vulkan-Headers, volk, VMA, stb, cgltf, nlohmann/json.
- 0003 — стиль коду: snake_case, `namespace advent`.
- 0004 — головний цикл з фіксованим кроком симуляції.
- 0005 — залежності як копії у `vendor/`.

## Відкриті рішення (запитай перед вибором)
- Організація сцени: простий граф сцени чи ECS (EnTT або власний).
- Тестовий фреймворк: doctest чи Catch2. Поки що використовуються мінімальні хелпери з `tests/test_support.hpp`.
- Налагоджувальний UI (Dear ImGui): поки не обрано.
- Напрям осі Y і спосіб її інвертування в проєкції (див. `/vulkan-renderer`).

## Чого не робити
- Не додавай залежності без запиту. Кожна нова бібліотека — це нова тека у `vendor/`, ціль у `vendor/CMakeLists.txt`, рядок у `vendor/README.md` і запис в ADR.
- Не вимикай validation layers у Debug, щоб «прибрати помилки», — виправляй причину.
- Не змінюй структуру модулів і правила залежностей без ADR.
- Не копіюй сторонній код поза `vendor/`. Правки у `vendor/` — окремими комітами з позначкою `// ADVENT PATCH:` і записом у `vendor/README.md`.
- Не форматуй і не лінтуй `vendor/`.
