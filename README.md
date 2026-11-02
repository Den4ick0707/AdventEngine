# AdventEngine

Навчальний 3D-рушій на **C++20** і **Vulkan 1.3**, що розробляється як курсовий проєкт.
Мета першої версії — 3D-сцена з імпортом моделей glTF зі скелетною анімацією.

[![CI](https://github.com/Den4ick0707/AdventEngine/actions/workflows/ci.yml/badge.svg?branch=dev)](https://github.com/Den4ick0707/AdventEngine/actions/workflows/ci.yml)

## Стан

| Етап | Результат | Стан |
|------|-----------|------|
| 1. Каркас | CMake-пресети, submodules, CI, `core` (лог, assert, час кадру), вікно GLFW | ✅ |
| 2. Vulkan-основа | instance, device, VMA ✅ · swapchain, очищення екрана ⬜ | 🟡 |
| 3. Перший трикутник | шейдери, пайплайн, вершинний буфер | ⬜ |
| 4. 3D | камера, depth buffer, uniform-буфери, куб, що обертається | ⬜ |
| 5. Статичні моделі | імпорт glTF, текстури, матеріали | ⬜ |
| 6. Скелетна анімація | скелет, кліпи, GPU-скінінг | ⬜ |
| 7. Демо | сцена в sandbox, перемикання анімацій, освітлення | ⬜ |

## Вимоги

| | Windows | Linux |
|-|---------|-------|
| Компілятор | Visual Studio 2022 (Desktop development with C++) | GCC 13+ або Clang 17+ |
| Збирання | CMake ≥ 3.25 і Ninja (вбудовані в CLion) | `cmake`, `ninja-build` |
| Vulkan | [Vulkan SDK](https://vulkan.lunarg.com) | Vulkan SDK або пакети `glslc`, `vulkan-validationlayers` |
| Інше | Git, Git LFS | Git, Git LFS, `libwayland-dev libxkbcommon-dev wayland-protocols xorg-dev` |

Потрібна відеокарта з підтримкою **Vulkan 1.3** — це більшість GPU NVIDIA, AMD та Intel останніх поколінь з оновленим драйвером. Перевірити можна командою `vulkaninfo --summary` з Vulkan SDK.

## Швидкий старт

```bash
git clone https://github.com/Den4ick0707/AdventEngine.git
cd AdventEngine
git submodule update --init --recursive --depth 1
git lfs install

cmake --preset debug
cmake --build --preset debug
ctest --preset debug
./build/debug/bin/sandbox          # Windows: build\debug\bin\sandbox.exe
```

`--depth 1` завантажує лише потрібні версії бібліотек (~35 МБ) замість їхньої повної історії (~370 МБ). Тому не використовуйте `git clone --recursive`: так Git завантажить усю історію бібліотек.

### CLion
1. **File → Open** і оберіть теку проєкту. CLion сам підхопить `CMakePresets.json`.
2. **Settings → Build, Execution, Deployment → Toolchains:** на Windows оберіть **Visual Studio** (архітектура `amd64`).
3. **Settings → … → CMake:** увімкніть профілі `debug` і `release` (Enable profile), а стандартний `Debug` вимкніть.
4. Оберіть конфігурацію `sandbox` і запустіть її.

### Пресети

| Пресет | Призначення |
|--------|-------------|
| `debug` | Debug, Vulkan validation layers, `ADVENT_ASSERT` |
| `release` | Release |
| `asan` | Debug + AddressSanitizer/UBSan (Linux) |
| `ci-debug`, `ci-release` | Як у CI: попередження = помилки |

## Структура

```
engine/
  core/        лог, assert, FrameClock, FixedTimestep
  platform/    вікно (GLFW)
  renderer/    VulkanContext (volk + VMA)
  runtime/     Application: головний цикл, модулі
apps/sandbox/  демо-застосунок
tests/         модульні тести (CTest)
third_party/   залежності (git submodules) + CMakeLists.txt з їхніми цілями
cmake/         попередження компілятора, компіляція шейдерів
docs/adr/      архітектурні рішення
.claude/       скіли Claude Code; CLAUDE.md — правила проєкту для Claude
```

## Залежності

Усі залежності підключені як git submodules і зафіксовані на тегах (`third_party/`):

| Бібліотека | Версія | Призначення |
|------------|--------|-------------|
| [GLFW](https://github.com/glfw/glfw) | 3.4 | вікно, ввід |
| [GLM](https://github.com/g-truc/glm) | 1.0.3 | математика |
| [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers) | 1.4.363 | заголовки Vulkan |
| [volk](https://github.com/zeux/volk) | 1.4.363 | завантажувач функцій Vulkan |
| [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) | 3.5.0 | керування пам'яттю GPU |
| [stb](https://github.com/nothings/stb) | d14d46f | завантаження зображень |
| [cgltf](https://github.com/jkuhlmann/cgltf) | 1.15 | імпорт glTF 2.0 |
| [nlohmann/json](https://github.com/nlohmann/json) | 3.12.0 | JSON |

## Розробка

- **Гілки:** `main` — стабільна, `dev` — інтеграційна, `feature/*` — робочі. Злиття в `dev` йде через Pull Request із зеленим CI.
- **CI:**
  - Linux (GCC і Clang) і Windows (MSVC): збирання, тести;
  - smoke-запуск sandbox з програмним Vulkan;
  - перевірки `clang-format` і `clang-tidy`.
- **Реліз:** `git tag v0.1.0 && git push origin v0.1.0` створює чернетку GitHub Release з архівами для Windows і Linux.
- **Стиль:** `.clang-format` і `.clang-tidy`, правила описані в [CLAUDE.md](CLAUDE.md).
- **Рішення:** [docs/adr](docs/adr/README.md).
- **Claude Code:** запускайте `claude` з кореня проєкту; правила й скіли підхопляться автоматично.

## Ліцензія

[MIT](LICENSE) © 2026 Daniil_Daryev
