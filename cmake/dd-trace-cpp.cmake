# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set(dd_trace_cpp_VERSION "1.0.0+52597cf6c073848fb1196c85fefc1daa3e0b11ba")

FetchContent_Declare(
  "dd-trace-cpp"
  GIT_REPOSITORY https://github.com/DataDog/dd-trace-cpp
  GIT_TAG        v2.0.0
  FIND_PACKAGE_ARGS NAMES dd-trace-cpp
)

FetchContent_MakeAvailable(dd-trace-cpp)

if(FetchContent_dd_trace_cpp_POPULATED)
  set(dd-trace-cpp_PROVIDER "fetch_repository")
else()
  set(dd-trace-cpp_PROVIDER "find_package")
endif()

# Set the prometheus-cpp_VERSION variable from the git tag.
# string(REGEX REPLACE "^v([0-9]+\\.[0-9]+\\.[0-9]+)$" "\\1" prometheus-cpp_VERSION "${prometheus-cpp_GIT_TAG}")

# Disable include-what-you-use and clang-tidy
foreach(_dd_trace_cpp_target objects static shared)
  set_target_properties(dd-trace-cpp-${_dd_trace_cpp_target} 
    PROPERTIES
      CXX_INCLUDE_WHAT_YOU_USE ""
      CXX_CLANG_TIDY ""
  )
endforeach()

if(NOT TARGET dd-trace-cpp::obj)
  message(FATAL_ERROR "A required dd-trace-cpp target was not imported")
endif()

