load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

DEFAULT_COMPILER_OPTIONS = [
    "-fdiagnostics-color",
    "-DTESTING",
    "-DLOGGING",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Werror",
    "-std=c++14",
    "-fmax-errors=3"
]

DEFAULT_TEST_DEPS = [
    "@gtest",
    "@gtest//:gtest_main",
]

cc_library(
    name = "simple_tea",
    srcs = [
    ],
    hdrs = glob([
        "AVR/Encryption/simple_tea.h",
        "AVR/Encryption/simple_tea_v2.h",
        "AVR/Encryption/common.h"
    ]),
    copts = DEFAULT_COMPILER_OPTIONS,
    strip_include_prefix = "AVR/Encryption",
)

cc_test(
    name = "unit_test_simple_tea_2bytes_payload",
    srcs = [
        "AVR/Encryption/test_unit_tea_2bytes_payload.cpp",
    ],
    copts = DEFAULT_COMPILER_OPTIONS,
    tags = ["unit"],
    deps = DEFAULT_TEST_DEPS + [":simple_tea"],
)

cc_test(
    name = "unit_test_simple_tea_4bytes_payload",
    srcs = [
        "AVR/Encryption/test_unit_tea_4bytes_payload.cpp",
    ],
    copts = DEFAULT_COMPILER_OPTIONS,
    tags = ["unit"],
    deps = DEFAULT_TEST_DEPS + [":simple_tea"],
)

cc_test(
    name = "unit_test_simple_tea_8bytes_payload",
    srcs = [
        "AVR/Encryption/test_unit_tea_8bytes_payload.cpp",
    ],
    copts = DEFAULT_COMPILER_OPTIONS,
    tags = ["unit"],
    deps = DEFAULT_TEST_DEPS + [":simple_tea"],
)
