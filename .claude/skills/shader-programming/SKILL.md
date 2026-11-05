---
name: shader-programming
description: Основи шейдерів для AdventEngine — етапи конвеєра, простори координат, нормалі, UV, типові ефекти (тонування, прокрутка UV, dissolve, rim light) у GLSL 460 для Vulkan. Use when writing shader math or visual effects, reasoning about coordinate spaces, normals, UVs, or translating an effect from another engine or HLSL.
---

# Шейдери: основи й ефекти

Файли, компіляція, дескриптори й пайплайни — скіл `shader-pipeline`. Тут — зміст шейдерів.

## Простори координат
- Модель → світ (`model`) → вид (`view`) → кліп (`proj`). Вихід вершинного шейдера — `gl_Position` у кліп-просторі.
- Нормалі — матрицею `transpose(inverse(mat3(model)))` або `mat3(model)` при рівномірному масштабі;
  у фрагментному шейдері — `normalize`, бо інтерполяція їх вкорочує.
- Усі вектори в одному обчисленні — в одному просторі. Змішування просторів — найчастіша помилка.
- Vulkan: глибина [0, 1], вісь Y кліп-простору вниз, UV (0, 0) — верхній лівий кут.

## Vulkan-особливості GLSL
- `#version 460`, явні `layout(location = N)` для входів і виходів, `layout(set = S, binding = B)` для ресурсів.
- Немає окремих uniform-змінних: дані — в uniform-буферах, SSBO або push constants.
- Час для анімації ефектів — з даних кадру (set 0), а не накопичений лічильник.

## Продуктивність
- `mix`, `step`, `smoothstep`, `clamp` замість розгалужень, умова яких різна для сусідніх пікселів.
  Розгалуження за uniform-значенням дешеве.
- Вибірки з текстур — основна вартість; менше вибірок.
- `discard` вимикає ранній тест глибини.
- `smoothstep(e0, e1, x)` при `e0 >= e1` не визначений: пиши `1.0 - smoothstep(e1, e0, x)`.

## Ефекти
```glsl
vec4 c = texture(albedo, uv) * tint;                               // тонування
vec2 scrolled = fract(uv + scroll_speed * frame.time);             // прокрутка UV
float cut = amount * (1.0 + edge) - edge;                          // dissolve
if (noise < cut) discard;
c = mix(edge_color, c, smoothstep(cut, cut + edge, noise));
float rim = pow(1.0 - clamp(dot(n, v), 0.0, 1.0), power);          // rim light, n і v нормалізовані, один простір
```

## GLSL і HLSL
`mix`/`lerp`, `fract`/`frac`, `texture()`/`.Sample()`, `vec2`/`float2`, `M * v`/`mul(M, v)`,
`mod`/`fmod` (різні для від'ємних чисел).

Адаптовано з `shader-programming` (awesome-gamedev-agent-skills, Apache-2.0), див. `.claude/third-party-skills/README.md`.
