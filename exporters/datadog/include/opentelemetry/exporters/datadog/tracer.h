#pragma once

#include <datadog/tracer.h>
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/tracer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace datadog
{

struct TraceIdHasher
{
  std::size_t operator()(const opentelemetry::trace::SpanId &span_id) const noexcept
  {
    const uint8_t *bytes   = span_id.Id().data();
    std::size_t hash_value = 0;

    for (size_t i = 0; i < opentelemetry::trace::SpanId::kSize; ++i)
    {
      hash_value ^= std::hash<uint8_t>{}(bytes[i]) << (i % 8);
    }

    return hash_value;
  }
};

class Span;

class Tracer : public opentelemetry::trace::Tracer,
               public std::enable_shared_from_this<opentelemetry::trace::Tracer>
{
  ::datadog::tracing::Tracer tracer_;
  std::unordered_map<opentelemetry::trace::SpanId, std::weak_ptr<Span>, TraceIdHasher> parents_map_;

public:
  Tracer(const ::datadog::tracing::FinalizedTracerConfig &);
  nostd::shared_ptr<opentelemetry::trace::Span> StartSpan(
      nostd::string_view name,
      const common::KeyValueIterable &attributes,
      const opentelemetry::trace::SpanContextKeyValueIterable &links,
      const opentelemetry::trace::StartSpanOptions &options = {}) noexcept override;

#if OPENTELEMETRY_ABI_VERSION_NO == 1
  void ForceFlushWithMicroseconds(uint64_t timeout) noexcept override;

  void CloseWithMicroseconds(uint64_t timeout) noexcept override;
#endif /* OPENTELEMETRY_ABI_VERSION_NO */
};

}  // namespace datadog
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
