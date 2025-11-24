# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cc_binary(
    name = "example_datadog",
    srcs = [
        "main.cc",
    ],
    tags = [
        "datadog",
        "examples",
    ],
    deps = [
        "//api",
        "//exporters/datadog:datadog_exporter",
    ],
)
