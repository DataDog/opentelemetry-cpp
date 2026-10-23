# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

#
# MAINTAINER
#
# This file is used for the Bazel build.
#
# When changing (add, upgrade, remove) dependencies
# please update:
# - the CMake build, see file
#   <root>/third_party_release
# - git submodule, see command
#   git submodule status
#

def opentelemetry_cpp_deps():
    """Loads dependencies need to compile the opentelemetry-cpp library."""

    # Google Benchmark library.
    # Only needed for benchmarks, not to build the OpenTelemetry library.
    maybe(
        http_archive,
        name = "com_github_google_benchmark",
        sha256 = "6bc180a57d23d4d9515519f92b0c83d61b05b5bab188961f36ac7b06b0d9e9ce",
        strip_prefix = "benchmark-1.8.3",
        urls = [
            "https://github.com/google/benchmark/archive/v1.8.3.tar.gz",
        ],
    )

    # GoogleTest framework.
    # Only needed for tests, not to build the OpenTelemetry library.
    maybe(
        http_archive,
        name = "com_google_googletest",
        sha256 = "8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7",
        strip_prefix = "googletest-1.14.0",
        urls = [
            "https://github.com/google/googletest/archive/v1.14.0.tar.gz",
        ],
    )

    # Load abseil dependency(optional)
    maybe(
        http_archive,
        name = "com_google_absl",
        sha256 = "733726b8c3a6d39a4120d7e45ea8b41a434cdacde401cba500f14236c49b39dc",
        strip_prefix = "abseil-cpp-20240116.2",
        urls = [
            "https://github.com/abseil/abseil-cpp/archive/refs/tags/20240116.2.tar.gz",
        ],
    )

    # Bazel 8 needs toolchain helpers absent from gRPC's older rules_cc pin.
    maybe(
        http_archive,
        name = "rules_cc",
        sha256 = "712d77868b3152dd618c4d64faaddefcc5965f90f5de6e6dd1d5ddcd0be82d42",
        strip_prefix = "rules_cc-0.1.1",
        urls = [
            "https://github.com/bazelbuild/rules_cc/releases/download/0.1.1/rules_cc-0.1.1.tar.gz",
        ],
    )

    # Bazel 8's WORKSPACE setup needs the Java and protobuf Starlark providers.
    maybe(
        http_archive,
        name = "rules_java",
        sha256 = "bbe7d94360cc9ed4607ec5fd94995fd1ec41e84257020b6f09e64055281ecb12",
        urls = [
            "https://github.com/bazelbuild/rules_java/releases/download/8.14.0/rules_java-8.14.0.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "com_google_protobuf",
        sha256 = "2e442d21839ec9dbafda4cc9083239aa04e78fc9c27dfa59b5374e968050cd22",
        strip_prefix = "protobuf-29.0",
        urls = [
            "https://github.com/protocolbuffers/protobuf/releases/download/v29.0/protobuf-29.0.zip",
        ],
    )

    # Preserve Python rule autoloading with Bazel 8's provider definitions.
    maybe(
        http_archive,
        name = "rules_python",
        sha256 = "690e0141724abb568267e003c7b6d9a54925df40c275a870a4d934161dc9dd53",
        strip_prefix = "rules_python-0.40.0",
        urls = [
            "https://github.com/bazelbuild/rules_python/releases/download/0.40.0/rules_python-0.40.0.tar.gz",
        ],
    )

    # Bazel 8's protobuf rules provide proto_common outside the native module.
    maybe(
        http_archive,
        name = "rules_proto",
        sha256 = "0e5c64a2599a6e26c6a03d6162242d231ecc0de219534c38cb4402171def21e8",
        strip_prefix = "rules_proto-7.0.2",
        urls = [
            "https://github.com/bazelbuild/rules_proto/releases/download/7.0.2/rules_proto-7.0.2.tar.gz",
        ],
    )

    # gRPC transitively depends on apple_support and rules_apple at older
    # versions. Bazel 7.x requires newer versions of these rules. By loading
    # them before grpc, these newer versions are preferrred.
    maybe(
        http_archive,
        name = "build_bazel_apple_support",
        sha256 = "c4bb2b7367c484382300aee75be598b92f847896fb31bbd22f3a2346adf66a80",
        url = "https://github.com/bazelbuild/apple_support/releases/download/1.15.1/apple_support.1.15.1.tar.gz",
    )

    maybe(
        http_archive,
        name = "build_bazel_rules_apple",
        sha256 = "b4df908ec14868369021182ab191dbd1f40830c9b300650d5dc389e0b9266c8d",
        url = "https://github.com/bazelbuild/rules_apple/releases/download/3.5.1/rules_apple.3.5.1.tar.gz",
    )

    # Load gRPC dependency
    maybe(
        http_archive,
        name = "com_github_grpc_grpc",
        sha256 = "f40bde4ce2f31760f65dc49a2f50876f59077026494e67dccf23992548b1b04f",
        strip_prefix = "grpc-1.62.0",
        urls = [
            "https://github.com/grpc/grpc/archive/refs/tags/v1.62.0.tar.gz",
        ],
    )

    # OTLP Protocol definition
    maybe(
        http_archive,
        name = "com_github_opentelemetry_proto",
        build_file = "@io_opentelemetry_cpp//bazel:opentelemetry_proto.BUILD",
        sha256 = "11330d850f5e24d34c4246bc8cb21fcd311e7565d219195713455a576bb11bed",
        strip_prefix = "opentelemetry-proto-1.7.0",
        urls = [
            "https://github.com/open-telemetry/opentelemetry-proto/archive/v1.7.0.tar.gz",
        ],
    )

    # JSON library
    maybe(
        http_archive,
        name = "github_nlohmann_json",
        build_file = "@io_opentelemetry_cpp//bazel:nlohmann_json.BUILD",
        sha256 = "b8cb0ef2dd7f57f18933997c9934bb1fa962594f701cd5a8d3c2c80541559372",
        urls = [
            "https://github.com/nlohmann/json/releases/download/v3.12.0/include.zip",
        ],
    )

    # C++ Prometheus Client library.
    maybe(
        http_archive,
        name = "com_github_jupp0r_prometheus_cpp",
        sha256 = "ac6e958405a29fbbea9db70b00fa3c420e16ad32e1baf941ab233ba031dd72ee",
        strip_prefix = "prometheus-cpp-1.3.0",
        urls = [
            "https://github.com/jupp0r/prometheus-cpp/archive/refs/tags/v1.3.0.tar.gz",
        ],
    )

    # bazel platforms
    maybe(
        http_archive,
        name = "platforms",
        sha256 = "218efe8ee736d26a3572663b374a253c012b716d8af0c07e842e82f238a0a7ee",
        urls = [
            "https://mirror.bazel.build/github.com/bazelbuild/platforms/releases/download/0.0.10/platforms-0.0.10.tar.gz",
            "https://github.com/bazelbuild/platforms/releases/download/0.0.10/platforms-0.0.10.tar.gz",
        ],
    )

    # libcurl (optional)
    maybe(
        http_archive,
        name = "curl",
        build_file = "@io_opentelemetry_cpp//bazel:curl.BUILD",
        sha256 = "816e41809c043ff285e8c0f06a75a1fa250211bbfb2dc0a037eeef39f1a9e427",
        strip_prefix = "curl-8.4.0",
        urls = [
            "https://curl.haxx.se/download/curl-8.4.0.tar.gz",
            "https://github.com/curl/curl/releases/download/curl-8_4_0/curl-8.4.0.tar.gz",
        ],
    )

    # rules foreign cc
    maybe(
        http_archive,
        name = "rules_foreign_cc",
        sha256 = "69023642d5781c68911beda769f91fcbc8ca48711db935a75da7f6536b65047f",
        strip_prefix = "rules_foreign_cc-0.6.0",
        url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.6.0.tar.gz",
    )

    # bazel skylib
    maybe(
        http_archive,
        name = "bazel_skylib",
        sha256 = "bc283cdfcd526a52c3201279cda4bc298652efa898b10b4db0837dc51652756f",
        urls = [
            "https://github.com/bazelbuild/bazel-skylib/releases/download/1.7.1/bazel-skylib-1.7.1.tar.gz",
            "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.7.1/bazel-skylib-1.7.1.tar.gz",
        ],
    )

    # Opentracing
    maybe(
        http_archive,
        name = "com_github_opentracing",
        sha256 = "5b170042da4d1c4c231df6594da120875429d5231e9baa5179822ee8d1054ac3",
        strip_prefix = "opentracing-cpp-1.6.0",
        urls = [
            "https://github.com/opentracing/opentracing-cpp/archive/refs/tags/v1.6.0.tar.gz",
        ],
    )

    # Zlib (optional)
    maybe(
        http_archive,
        name = "zlib",
        build_file = "@io_opentelemetry_cpp//bazel:zlib.BUILD",
        sha256 = "d14c38e313afc35a9a8760dadf26042f51ea0f5d154b0630a31da0540107fb98",
        strip_prefix = "zlib-1.2.13",
        urls = [
            "https://github.com/madler/zlib/releases/download/v1.2.13/zlib-1.2.13.tar.xz",
            "https://zlib.net/zlib-1.2.13.tar.xz",
        ],
    )
