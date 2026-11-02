# Engine modules in dependency order (see "Module dependency rules" in CLAUDE.md).
add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(renderer)
add_subdirectory(runtime)

# Planned modules (add with the /engine-module skill when their stage starts):
#   assets     - glTF / texture import into CPU data (cgltf, stb)
#   animation  - skeleton, clips, sampling, joint matrices
#   scene      - entities, transforms, hierarchy
