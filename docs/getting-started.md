# Встановлення та перший запуск

## Вимоги

| | Linux | Windows |
|-|-------|---------|
| Компілятор | GCC 13+ або Clang 17+ | Visual Studio 2022 (Desktop development with C++) |
| Збирання | CMake 3.25+, Ninja | CMake 3.25+, Ninja (є в Visual Studio і CLion) |
| Vulkan | пакети `glslc`, `vulkan-validationlayers` або [Vulkan SDK](https://vulkan.lunarg.com) | [Vulkan SDK](https://vulkan.lunarg.com) |
| Інше | Git, Git LFS, dev-пакети Wayland і X11 для GLFW | Git, Git LFS |

Відеокарта має підтримувати Vulkan 1.3 з `dynamicRendering` і `synchronization2`. Це більшість GPU NVIDIA, AMD та Intel
останніх поколінь з оновленим драйвером. Перевірка: `vulkaninfo --summary`.

Сторонні бібліотеки лежать у `vendor/`, тож після клонування мережа для збирання не потрібна.

## Linux

```bash
git clone git@github.com:Den4ick0707/AdventEngine.git      # або https://github.com/Den4ick0707/AdventEngine.git
cd AdventEngine
./dev setup --install    # системні пакети, git-хуки, налаштування git
./dev build              # Debug з validation layers
./dev run                # sandbox
```

`./dev setup --install` визначає менеджер пакетів сам: apt (Ubuntu, Debian), pacman (Arch, Manjaro), dnf (Fedora),
zypper (openSUSE). Без `--install` команда лише перевіряє інструменти й вмикає git-хуки.

`./dev setup` потрібно запускати в кожному новому клоні: саме він вмикає git-хуки (`core.hooksPath`).

## Windows

Скрипта `./dev` для Windows немає. Команди виконуються з «Developer PowerShell for VS 2022», щоб Ninja знайшов `cl.exe`:

```powershell
git clone git@github.com:Den4ick0707/AdventEngine.git
cd AdventEngine
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
build\debug\bin\sandbox.exe
```

## CLion

1. **File > Open**, вибрати теку проєкту. CLion підхопить `CMakePresets.json`.
2. **Settings > Build, Execution, Deployment > CMake**: увімкнути профілі `debug` і `release`, стандартний `Debug` вимкнути.
3. На Windows у **Toolchains** вибрати Visual Studio, архітектура `amd64`.
4. Для налагодження використовувати конфігурацію `sandbox` (ціль CMake).
   Команди `./dev` доступні як готові конфігурації в групі `dev` (файли `.run/`).

## Що далі

- [Розробка](development.md): команди `./dev`, пресети, перевірки, CI.
- [Процес роботи з Git](../CONTRIBUTING.md): гілки, коміти, Pull Request, релізи.
- [Архітектура](architecture.md).
