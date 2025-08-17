// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>

#include <iostream>
#include "opentelemetry/exporters/datadog/tracer_provider.h"
#include "opentelemetry/sdk/trace/provider.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/tracer_provider.h"

#ifdef BAZEL_BUILD
#  include "examples/common/foo_library/foo_library.h"
#else
#  include "foo_library/foo_library.h"
#endif

namespace trace     = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace dd        = opentelemetry::exporter::datadog;

namespace
{

void InitTracer()
{
  // Create datadog exporter instance
  std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
      std::make_shared<dd::TracerProvider>();
  // Set the global trace provider
  trace_sdk::Provider::SetTracerProvider(provider);
}

void CleanupTracer()
{
  std::shared_ptr<opentelemetry::trace::TracerProvider> none;
  trace_sdk::Provider::SetTracerProvider(none);
}
}  // namespace

int main(int argc, char *argv[])
{
  /*if (argc == 2)*/
  /*{*/
  /*  opts.endpoint = argv[1];*/
  /*}*/
  // Removing this line will leave the default noop TracerProvider in place.
  InitTracer();

  foo_library();

  CleanupTracer();
  return 0;
}
