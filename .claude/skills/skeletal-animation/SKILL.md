---
name: skeletal-animation
description: Імпорт 3D-моделей glTF 2.0 через cgltf (меші, матеріали, скелет, анімації), семплінг кліпів, матриці суглобів, GPU-скінінг в AdventEngine. Use when importing models, working with skins/joints/bones, animation clips, keyframes, skinning shaders or animated characters — "імпорт моделі", "анімація", "скелет".
---

# Імпорт моделей і скелетна анімація

Головна вимога курсового. Формат — **glTF 2.0** (`.gltf` + `.bin` або `.glb`), бібліотека — **cgltf**: ціль `advent::cgltf`, реалізація вже скомпільована в `third_party/impl/cgltf_impl.c`. Текстури завантажуються через **stb_image** (`advent::stb`).

## Розподіл відповідальності (модулі ще треба створити скілом /engine-module)
- `assets` (`PRIVATE advent::cgltf advent::stb`) перетворює файл на CPU-структури: `MeshData`, `SkeletonData`, `AnimationClipData`, `MaterialData`, `ImageData`. Про Vulkan нічого не знає. Типи cgltf не виходять за межі `src/`.
- `animation` містить `Skeleton`, `AnimationClip`, `Pose`, семплінг і обчислення матриць суглобів. Це чиста математика на GLM, яку легко покрити тестами.
- `renderer` завантажує вершини на GPU, тримає SSBO з матрицями суглобів і пайплайн скінінгу.

## Робота з cgltf
```cpp
cgltf_options options{};
cgltf_data* data = nullptr;
if (cgltf_parse_file(&options, path, &data) != cgltf_result_success) { /* error */ }
// RAII: одразу загорни data в std::unique_ptr з cgltf_free як deleter
if (cgltf_load_buffers(&options, data, path) != cgltf_result_success) { /* error */ }
cgltf_validate(data); // корисно в Debug
```
- Читай атрибути через `cgltf_accessor_read_float` / `cgltf_accessor_read_uint` або `cgltf_accessor_unpack_floats`. Вони самі враховують stride, нормалізацію та sparse-доступ.
- Індекси вузлів: `cgltf_node_index(data, node)`. Ієрархію обходь через `node->parent` і `node->children`.
- Шляхи до текстур (`image->uri`) — відносно файлу моделі. Зображення всередині `.glb` лежать у `image->buffer_view`.

## Дані з glTF
- **Вершини скінованого меша:** `POSITION`, `NORMAL`, `TEXCOORD_0`, `JOINTS_0` (u8 або u16, конвертуй у `uvec4`), `WEIGHTS_0` (можуть бути нормалізовані u8 чи u16, конвертуй у `vec4` float). Перевір, що сума ваг ≈ 1, і нормалізуй.
- **Skin:** `joints` (вузли) та `inverse_bind_matrices`. Якщо матриць немає — одиничні.
- **Скелет:** суглоби зберігай у масиві, **упорядкованому так, щоб батько йшов перед нащадком**, разом з індексом батька.
- **Анімація:** channels (target node + path: translation / rotation / scale / weights) і samplers (input — час, output — значення, interpolation: `LINEAR`, `STEP`, `CUBICSPLINE`).
- Локальна трансформація вузла задається або через TRS (`has_translation` / `has_rotation` / `has_scale`), або через `matrix` (`has_matrix`). Підтримуй обидва варіанти.

## Програвання
1. Для часу `t` (з циклічністю) знайди ключі бінарним пошуком по `input`.
2. Інтерполяція:
   - translation і scale — `lerp`;
   - rotation — `slerp` нормалізованих кватерніонів. **Увага:** у glTF порядок **x, y, z, w**, а конструктор `glm::quat(w, x, y, z)`. Найнадійніше — `glm::make_quat` або явне присвоєння полів.
   - `STEP` — без інтерполяції;
   - `CUBICSPLINE` — у output по 3 елементи на ключ: tangent-in, value, tangent-out.
3. Суглоби без каналів анімації зберігають свою rest-трансформацію.
4. Глобальні трансформації: `global[i] = global[parent[i]] * local[i]`, обхід у порядку масиву.
5. Матриця суглоба: `joint_matrix[i] = inverse(global_of_mesh_node) * global[i] * inverse_bind[i]`.
6. Анімацію постав у `on_update(dt)` (змінний крок), а не в `on_fixed_update`.

## GPU-скінінг (вершинний шейдер)
```glsl
layout(set = 2, binding = 0) readonly buffer JointMatrices { mat4 joints[]; };

mat4 skin = in_weights.x * joints[in_joints.x]
          + in_weights.y * joints[in_joints.y]
          + in_weights.z * joints[in_joints.z]
          + in_weights.w * joints[in_joints.w];
vec4 world_pos = model * skin * vec4(in_position, 1.0);
vec3 world_nrm = normalize(mat3(model * skin) * in_normal);
```
SSBO з матрицями — окремий для кожного кадру в польоті (див. `/vulkan-renderer`).

## Тестові моделі (Khronos glTF-Sample-Assets)
Клади в `assets/models/`: `.glb` і `.bin` автоматично йдуть у Git LFS. Перевіряй поступово:
1. `Box`, `Duck` — статичні меші, матеріали, текстури.
2. `AnimatedCube` — анімація вузла без скінінгу.
3. `RiggedSimple` → `RiggedFigure` → `CesiumMan` — скінінг.
4. `Fox` — кілька кліпів (перемикання анімацій).
5. `InterpolationTest` — STEP, LINEAR і CUBICSPLINE.

## Налагодження та тести
- Модель «вибухає» або сплющена: перевір порядок кватерніонів, порядок множення матриць і `inverse_bind_matrices`.
- Модель лежить на боці: glTF має +Y вгору і праву систему координат.
- Debug-відображення кісток (лінії між суглобами) — найшвидший спосіб знайти помилку.
- Unit-тести в `tests/animation/`: семплінг на межах і посередині ключів, обчислення глобальних трансформацій на 2–3 суглобах.
