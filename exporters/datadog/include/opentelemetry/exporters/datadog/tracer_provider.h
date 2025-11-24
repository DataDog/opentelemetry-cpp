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

struct TracerKey
{
  std::string name;
  std::string version;
  std::string schema_url;

  bool operator==(const TracerKey &rhs) const
  {
    return name == rhs.name && version == rhs.version && schema_url == rhs.schema_url;
  }
};

struct TracerKeyHasher
{
  std::size_t operator()(const TracerKey &key) const noexcept
  {
    std::size_t hash_value = std::hash<std::string>{}(key.name);
    hash_value ^= (std::hash<std::string>{}(key.version) << 1) >> 1;
    hash_value ^= std::hash<std::string>{}(key.schema_url) << 2;
    return hash_value;
  }
};

class TracerProvider : public opentelemetry::trace::TracerProvider
{
  TracerConfiguration config_;
  std::unordered_map<TracerKey, nostd::shared_ptr<opentelemetry::trace::Tracer>, TracerKeyHasher>
      tracers_;

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
