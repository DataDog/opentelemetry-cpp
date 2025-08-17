# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(opentelemetry_exporter_datadog_trace)

# TODO: Move the compile features on the dd-trace-cpp dependency
target_compile_features(opentelemetry_exporter_datadog_trace
  PUBLIC
    cxx_std_17
)

target_include_directories(
  opentelemetry_exporter_datadog_trace 
  PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

target_sources(opentelemetry_exporter_datadog_trace
  PRIVATE
    src/tracer.cc
    src/tracer_provider.cc
    src/span.cc
)

target_link_libraries(opentelemetry_exporter_datadog_trace
  PUBLIC
    opentelemetry_trace 
    dd-trace-cpp::static
    # opentelemetry_http_client_curl
)

set_target_version(opentelemetry_exporter_datadog_trace)

set_target_properties(opentelemetry_exporter_datadog_trace
  PROPERTIES 
    EXPORT_NAME datadog_trace_exporter
)

# Register Datadog component
otel_add_component(
  COMPONENT
  exporters_datadog
  TARGETS
  opentelemetry_exporter_datadog_trace
  FILES_DIRECTORY
  "include/opentelemetry/exporters/datadog"
  FILES_DESTINATION
  "include/opentelemetry/exporters"
  FILES_MATCHING
  PATTERN
  "*.h"
  PATTERN
  "recordable.h"
  EXCLUDE)

if(BUILD_TESTING)
  add_executable(datadog_test
    test/provider_test.cc
    test/tracer_test.cc
  )

  target_link_libraries(datadog_test
    PRIVATE
      ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      opentelemetry_exporter_datadog_trace
  )

  gtest_add_tests(
    TARGET datadog_test
  )
endif()
