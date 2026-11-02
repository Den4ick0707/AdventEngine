DESCRIPTION="Sandbox with Vulkan loader logs (layers/driver problems)"
PRESET=debug
ARGS=(--frames 60)
export VK_LOADER_DEBUG=error,warn,info
