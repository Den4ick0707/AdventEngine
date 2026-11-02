# Run profile template. Copy with `./dev new-profile <name>` and run with `./dev p <name> [extra args]`.
# A profile is a bash file sourced by ./dev. Every variable is optional.

DESCRIPTION="What this run is for"

PRESET=debug      # CMake preset: debug, release, relwithdebinfo, asan, ci-debug, ci-release
TARGET=sandbox    # executable target in build/<preset>/bin/
ARGS=()           # arguments, e.g. ARGS=(--frames 600 --max-fps 60)
RUNNER=()         # wrapper, e.g. RUNNER=(gdb -q --args) or RUNNER=(valgrind --leak-check=full)
HEADLESS=0        # 1 = use xvfb-run when there is no display

# Environment variables for the run:
# export VK_LOADER_DEBUG=error
# export ADVENT_SMOKE_FRAMES=300

# Full control: define run() and ./dev calls it instead of the default "build + start".
# Helpers from ./dev are available: cmd_build, cmd_test, with_display, step, ok, warn, die.
# run() {
#     cmd_build "$PRESET" "$TARGET"
#     "build/$PRESET/bin/$TARGET" --frames 100 "$@"
# }
