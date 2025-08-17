# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_executable(example_datadog main.cc)

target_link_libraries(example_datadog
  PRIVATE 
    common_foo_library
    opentelemetry-cpp::datadog_trace_exporter
)
