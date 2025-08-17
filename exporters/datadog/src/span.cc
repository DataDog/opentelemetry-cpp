// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/datadog/span.h"
#include "overload.h"
#include "util.h"

#include <datadog/dict_writer.h>
#include <datadog/trace_segment.h>

namespace dd    = ::datadog::tracing;
namespace trace = opentelemetry::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace datadog
{

namespace
{

struct ContextSetter : public dd::DictWriter
{
  nostd::shared_ptr<trace::TraceState> tracestate;

  ContextSetter() : tracestate(nullptr) {}
  ~ContextSetter() override = default;

  void set(dd::StringView key, dd::StringView value) override
  {
    if (key == "tracestate")
    {
      tracestate = trace::TraceState::FromHeader(value.data());
    }
  };
};

std::unique_ptr<opentelemetry::trace::SpanContext> make_context(
    const dd::Span &span,
    const nostd::shared_ptr<trace::TraceState> &ts)
{
  auto tid      = TraceId(span.trace_id());
  auto sid      = SpanId(span.id());
  auto decision = span.trace_segment().sampling_decision();
  trace::TraceFlags flags;
  if (decision && decision->priority > 0)
  {
    flags = trace::TraceFlags(trace::TraceFlags::kIsSampled);
  }
  return std::make_unique<opentelemetry::trace::SpanContext>(tid, sid, flags, false, ts);
}

}  // namespace

Span::Span(::datadog::tracing::Span span) : span_(std::move(span)), context_(nullptr) {}

void Span::SetAttribute(nostd::string_view key, const common::AttributeValue &attribute) noexcept
{
  nostd::visit(details::Overload{
                   [&](const char *cstr) { span_.set_tag(key.data(), cstr); },
                   [&](nostd::string_view sv) {
                     span_.set_tag(key.data(), dd::StringView{sv.data(), sv.size()});
                   },
                   [](auto &) {},
               },
               attribute);
}

// Adds an event to the Span.
void Span::AddEvent(nostd::string_view name) noexcept
{
  // TBD
}

// Adds an event to the Span, with a custom timestamp.
void Span::AddEvent(nostd::string_view name, common::SystemTimestamp timestamp) noexcept
{
  // TBD
}

// Adds an event to the Span, with a custom timestamp, and attributes.
void Span::AddEvent(nostd::string_view name,
                    common::SystemTimestamp timestamp,
                    const common::KeyValueIterable &attributes) noexcept
{
  // Not supported
}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
void Span::AddLink(const SpanContext &target, const common::KeyValueIterable &attrs) noexcept
{
  // Not supported
}

void Span::AddLinks(const SpanContextKeyValueIterable &links) noexcept
{
  // Not supported.
}
#endif

void Span::SetStatus(opentelemetry::trace::StatusCode code, nostd::string_view description) noexcept
{
  if (code == trace::StatusCode::kError)
  {
    span_.set_error(true);
    span_.set_error_message(description.data());
  }
}

void Span::UpdateName(nostd::string_view name) noexcept
{
  span_.set_name(name.data());
}

void Span::End(const trace::EndSpanOptions &options) noexcept
{
  // TODO: Check if valid?
  span_.set_end_time(options.end_steady_time);
}

trace::SpanContext Span::GetContext() const noexcept
{
  if (context_ == nullptr)
  {
    ContextSetter ctx;
    span_.inject(ctx);
    context_ = make_context(span_, ctx.tracestate);
  }
  return *context_;
}

bool Span::IsRecording() const noexcept
{
  return true;
}

}  // namespace datadog
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
