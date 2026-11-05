---
name: skeletal-animation
description: Імпорт glTF 2.0 через cgltf (меші, матеріали, скелет, анімації), семплінг кліпів, матриці суглобів, GPU-скінінг в AdventEngine. Use when importing models, working with skins, joints, bones, animation clips, keyframes, skinning shaders or animated characters — "імпорт моделі", "анімація", "скелет".
---

# glTF і скелетна анімація

## Розподіл між модулями
| Модуль | Відповідальність | Залежності |
|--------|------------------|------------|
| `assets` | файл glTF у CPU-дані: `MeshData`, `SkeletonData`, `AnimationClipData`, `MaterialData`, `ImageData` | `PRIVATE advent::cgltf advent::stb` |
| `animation` | `Skeleton`, `AnimationClip`, `Pose`, семплінг, матриці суглобів; чиста математика GLM | `core` |
| `renderer` | вершини на GPU, SSBO матриць суглобів, пайплайн скінінгу | `vulkan` |

Типи cgltf не виходять за межі `assets/src`.

## cgltf
```cpp
cgltf_options options{};
cgltf_data* data = nullptr;
cgltf_parse_file(&options, path, &data);        // одразу в unique_ptr з cgltf_free як deleter
cgltf_load_buffers(&options, data, path);
cgltf_validate(data);                           // у Debug
```
- Атрибути: `cgltf_accessor_read_float`, `cgltf_accessor_read_uint`, `cgltf_accessor_unpack_floats` — враховують
  stride, нормалізацію і sparse.
- Індекс вузла: `cgltf_node_index(data, node)`.
- Текстури: `image->uri` відносно файлу моделі; у `.glb` — `image->buffer_view`.

## Дані
- Вершини скінованого меша: `POSITION`, `NORMAL`, `TEXCOORD_0`, `JOINTS_0` (u8/u16 в `uvec4`),
  `WEIGHTS_0` (у `vec4` float). Сума ваг нормалізується до 1.
- Skin: `joints` і `inverse_bind_matrices`; якщо матриць немає — одиничні.
- Скелет: масив суглобів, де батько йде перед нащадком, з індексом батька.
- Локальна трансформація: TRS (`has_translation`, `has_rotation`, `has_scale`) або `matrix` (`has_matrix`).
- Анімація: channels (вузол і шлях: translation, rotation, scale, weights) і samplers
  (час, значення, інтерполяція `LINEAR`, `STEP`, `CUBICSPLINE`).

## Програвання
1. Ключі для часу `t` — бінарним пошуком.
2. Інтерполяція: translation і scale — `lerp`; rotation — `slerp` нормалізованих кватерніонів;
   `STEP` — без інтерполяції; `CUBICSPLINE` — по три значення на ключ (in-tangent, value, out-tangent).
3. Кватерніон у glTF — x, y, z, w; `glm::quat(w, x, y, z)`. Використовуй `glm::make_quat` або явні поля.
4. Суглоби без каналів зберігають rest-трансформацію.
5. `global[i] = global[parent[i]] * local[i]` у порядку масиву.
6. `joint_matrix[i] = inverse(global_of_mesh_node) * global[i] * inverse_bind[i]`.
7. Анімація — системи `animation.sample` і `animation.joints` у фазі `update` (ADR 0020), не `fixed_update`.

## GPU-скінінг (compute, ADR 0021)
Прохід `skinning` на початку кадру пише деформовані вершини в буфер екземпляра; далі меш малюється як статичний,
а `raytracing` оновлює з того ж буфера BLAS.
```glsl
mat4 skin = w.x * joints[j.x] + w.y * joints[j.y] + w.z * joints[j.z] + w.w * joints[j.w];
out_vertices[i].position = (skin * vec4(in_vertices[i].position, 1.0)).xyz;   // простір моделі
out_vertices[i].normal   = normalize(mat3(skin) * in_vertices[i].normal);
```
Буфер `JointMatrices` — окремий на кожен кадр у польоті.

## Тестові моделі
Khronos glTF-Sample-Assets у `assets/models/` (Git LFS), по порядку:
`Box`, `Duck` (статика) → `AnimatedCube` (анімація вузла) → `RiggedSimple`, `RiggedFigure`, `CesiumMan` (скінінг)
→ `Fox` (кілька кліпів) → `InterpolationTest` (усі типи інтерполяції).

## Налагодження
- Модель «вибухає» або сплющена: порядок компонент кватерніона, порядок множення матриць, `inverse_bind_matrices`.
- Модель лежить на боці: glTF використовує +Y вгору і праву систему координат.
- Відображення кісток лініями — найшвидший спосіб знайти помилку.
- Тести в `tests/animation/`: семплінг на межах і між ключами, глобальні трансформації для 2–3 суглобів.
