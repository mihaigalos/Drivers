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
]

DEFAULT_TEST_DEPS = [
    ":sut",
    "@gtest",
    "@gtest//:gtest_main",
]

cc_library(
    name = "simple_tea",
    srcs = [
        "AVR/Encryption/simple_tea.cpp",
    ],
    hdrs = glob(["AVR/Encryption/*.h"]),
    #copts = DEFAULT_COMPILER_OPTIONS,
    strip_include_prefix = "AVR/Encryption",
)

cc_test(
    name = "unit_test_simple_tea",
    srcs = [
        "AVR/Encryption/test_unit.cpp",
    ],

    # copts = DEFAULT_COMPILER_OPTIONS,
    tags = ["unit"],
    deps = [
        ":simple_tea",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
