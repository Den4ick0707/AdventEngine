---
name: cpp-pro
description: Сучасний C++20 для AdventEngine — RAII, move-семантика, concepts, розкладка даних і кеш, алокатори, конкурентність, санітайзери. Use when writing or refactoring non-trivial C++, designing class interfaces or templates, optimizing memory layout, writing multithreaded code, or debugging memory errors and undefined behavior.
---

# Сучасний C++

Базові правила стилю й помилок — `docs/coding-style.md`. Тут — техніки.

## Інтерфейси
- Шаблони обмежуються concepts, а не SFINAE: `template <std::floating_point T>`.
- Статичний поліморфізм (шаблони, CRTP) — для гарячого коду; віртуальні функції — на грубих межах (модуль, система).
- Параметри: дешеві типи за значенням, решта — `const T&`; `std::span` замість пари вказівник і розмір;
  `std::string_view` для рядків без володіння. Не зберігай `string_view` і `span` довше за джерело.
- `[[nodiscard]]` на функціях, результат яких не можна ігнорувати. `noexcept` там, де виняток неможливий.
- `const` скрізь, де можливо; `constexpr` для обчислень під час компіляції.

## Володіння і переміщення
- `std::unique_ptr` — власність; `std::shared_ptr` — лише за справжньої спільної власності.
- Клас з ресурсом: заборонене копіювання; переміщення — через `std::exchange`, з `noexcept`.
- Rule of zero: якщо члени самі керують ресурсами, спецфункції не пишуться.
- Повертай за значенням: RVO і переміщення роблять це дешевим.

## Дані і кеш
- Гарячі дані — суцільними масивами (`std::vector`), структура масивів (SoA) для систем, що проходять
  по одному полю тисяч об'єктів.
- Уникай вказівників на окремі об'єкти в гарячих циклах: handles (індекс і покоління) замість вказівників.
- Виділення пам'яті в кадрі — мінімум: перевикористовуй буфери, `reserve`, лінійні (arena) алокатори на кадр.
- `alignas` для даних, що йдуть на GPU, і для уникнення false sharing між потоками (64 байти).

## Конкурентність
- Спершу однопотоковий код; потоки — після вимірювання і рішення в темі «Потоки».
- Спільний стан — під `std::mutex` і `std::scoped_lock`; у гарячому шляху замість блокувань — дані на потік
  з подальшим об'єднанням.
- `std::atomic`: `memory_order_relaxed` лише для лічильників без залежних даних; для публікації даних —
  `release` при записі і `acquire` при читанні.
- `std::jthread` замість `std::thread`: сам приєднується при знищенні.

## Перевірка
- `./dev p tests-asan`, `./dev p sandbox-asan` — AddressSanitizer і UBSan.
- Гонки даних — ThreadSanitizer (окремий пресет за потреби, несумісний з ASan).
- Профілювання: пресет `relwithdebinfo`, `perf record` / `perf report`, `valgrind --tool=callgrind`.

## Не робити
- C-style приведення типів — лише `static_cast` і споріднені.
- `using namespace` у заголовках.
- Невизначена поведінка «бо працює»: переповнення знакових цілих, читання неініціалізованих значень,
  порушення strict aliasing (`std::bit_cast` або `std::memcpy` замість `reinterpret_cast` для перетворення типів).

Адаптовано з `cpp-pro` (Jeffallan/claude-skills, MIT), див. `.claude/third-party-skills/README.md`.
