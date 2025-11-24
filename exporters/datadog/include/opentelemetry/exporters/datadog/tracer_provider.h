// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/exporters/datadog/tracer_configuration.h"
#include "opentelemetry/trace/tracer_provider.h"

#include <unordered_map>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace datadog
{

class TracerProvider : public opentelemetry::trace::TracerProvider
{
  TracerConfiguration config_;
  std::shared_ptr<opentelemetry::trace::Tracer> tracer_impl_;

public:
  TracerProvider() = default;
  TracerProvider(TracerConfiguration config);
  ~TracerProvider() override = default;

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  nostd::shared_ptr<opentelemetry::trace::Tracer> GetTracer(
      nostd::string_view name,
      nostd::string_view version,
      nostd::string_view schema_url,
      const common::KeyValueIterable *attributes) noexcept override;
#endif

  nostd::shared_ptr<opentelemetry::trace::Tracer> GetTracer(
      nostd::string_view name,
      nostd::string_view version    = "",
      nostd::string_view schema_url = "") noexcept override;
};

}  // namespace datadog
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
