DESCRIPTION="Unit tests with AddressSanitizer + UBSan"

run() {
    cmd_test asan "$@"
}
