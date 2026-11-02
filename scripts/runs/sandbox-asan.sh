DESCRIPTION="Sandbox with AddressSanitizer + UBSan, 600 frames"
PRESET=asan
ARGS=(--frames 600)
export ASAN_OPTIONS="detect_leaks=1"
export UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=1"
