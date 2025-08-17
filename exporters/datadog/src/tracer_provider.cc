// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/datadog/tracer_provider.h"
#include "opentelemetry/exporters/datadog/tracer.h"
#include "opentelemetry/trace/noop.h"

namespace dd = ::datadog::tracing;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace datadog
{

TracerProvider::TracerProvider(TracerConfiguration config) : config_(std::move(config)) {}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2

nostd::shared_ptr<Tracer> TracerProvider::GetTracer(
    nostd::string_view name,
    nostd::string_view version,
    nostd::string_view schema_url,
    const common::KeyValueIterable *attributes) noexcept
{
  // TODO: convert `attributes` to tags.
  return GetTracer(name, version, schema_url);
}

#endif

nostd::shared_ptr<opentelemetry::trace::Tracer> TracerProvider::GetTracer(
    nostd::string_view name,
    nostd::string_view version,
    nostd::string_view schema_url) noexcept
{
  TracerKey key{
      std::string(name),
      std::string(version),
      std::string(schema_url),
  };

  auto it = tracers_.find(key);
  if (it == tracers_.cend())
  {
    std::shared_ptr<opentelemetry::trace::Tracer> tracer_impl;

    // Enforce default configurations
    config_.injection_styles    = {dd::PropagationStyle::W3C};
    config_.integration_name    = "opentelemetry";
    config_.integration_version = "TBD";

    auto finalized_config = dd::finalize_config(config_);
    if (finalized_config)
    {
      tracer_impl = std::make_shared<Tracer>(*finalized_config);
    }
    else
    {
      tracer_impl = std::make_shared<opentelemetry::trace::NoopTracer>();
    }

    std::tie(it, std::ignore) = tracers_.emplace(std::move(key), tracer_impl);
  }

  return it->second;
}

}  // namespace datadog
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
