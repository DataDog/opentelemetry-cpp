#pragma once

#include <datadog/span.h>
#include "opentelemetry/trace/span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace datadog
{

class Span : public opentelemetry::trace::Span
{
  mutable std::unique_ptr<opentelemetry::trace::SpanContext> context_;

public:
  ::datadog::tracing::Span span_;

  Span(::datadog::tracing::Span);
  void SetAttribute(nostd::string_view key, const common::AttributeValue &value) noexcept override;

  // Adds an event to the Span.
  void AddEvent(nostd::string_view name) noexcept override;

  // Adds an event to the Span, with a custom timestamp.
  void AddEvent(nostd::string_view name, common::SystemTimestamp timestamp) noexcept override;

  // Adds an event to the Span, with a custom timestamp, and attributes.
  void AddEvent(nostd::string_view name,
                common::SystemTimestamp timestamp,
                const common::KeyValueIterable &attributes) noexcept override;

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  void AddLink(const SpanContext &target, const common::KeyValueIterable &attrs) noexcept override;

  /**
   * Add links (ABI).
   *
   * See comments about sampling in @ref opentelemetry::trace::Span
   *
   * @since ABI_VERSION 2
   */
  void AddLinks(const SpanContextKeyValueIterable &links) noexcept override;
#endif

  void SetStatus(opentelemetry::trace::StatusCode code,
                 nostd::string_view description = "") noexcept override;

  // Updates the name of the Span. If used, this will override the name provided
  // during creation.
  void UpdateName(nostd::string_view name) noexcept override;

  /**
   * Mark the end of the Span.
   * Only the timing of the first End call for a given Span will be recorded,
   * and implementations are free to ignore all further calls.
   * @param options can be used to manually define span properties like the end
   * timestamp
   */
  void End(const trace::EndSpanOptions &options = {}) noexcept override;

  trace::SpanContext GetContext() const noexcept override;

  // Returns true if this Span is recording tracing events (e.g. SetAttribute,
  // AddEvent).
  bool IsRecording() const noexcept override;
};

}  // namespace datadog
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
