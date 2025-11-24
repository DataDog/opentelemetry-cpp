# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "datadog_exporter",
    srcs = [
        "src/span.cc",
        "src/tracer.cc",
        "src/tracer_provider.cc",
    ],
    hdrs = [
        "include/opentelemetry/exporters/datadog/span.h",
        "include/opentelemetry/exporters/datadog/tracer.h",
        "include/opentelemetry/exporters/datadog/tracer_configuration.h",
        "include/opentelemetry/exporters/datadog/tracer_provider.h",
    ],
    strip_include_prefix = "include",
    tags = ["datadog"],
    deps = [
        "//api",
        "@com_github_DataDog_dd_trace_cpp//:dd-trace-cpp",
    ],
)

cc_test(
    name = "datadog_exporter_test",
    srcs = [
        "test/provider_test.cc",
        "test/tracer_test.cc",
    ],
    tags = [
        "datadog",
        "test",
    ],
    deps = [
        ":datadog_exporter",
        "@com_google_googletest//:gtest_main",
    ],
)
