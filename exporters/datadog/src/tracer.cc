#include "opentelemetry/exporters/datadog/tracer.h"
#include "opentelemetry/baggage/baggage.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/exporters/datadog/span.h"
#include "opentelemetry/trace/context.h"

#include <datadog/dict_reader.h>

#include "overload.h"
#include "util.h"

#include <unordered_map>

namespace dd = ::datadog::tracing;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace datadog
{
namespace
{
class SpanContextReader final : public dd::DictReader
{
  std::unordered_map<std::string, std::string> ctx_;

  std::string make_traceparent(const opentelemetry::trace::SpanContext &span_context)
  {
    char buffer[32];

    std::string traceparent;
    traceparent.reserve(55);  // "00-" + 32 + "-" + 16 + "-" + 2 = 55 chars

    // Version
    traceparent.append("00-");

    // Trace ID
    span_context.trace_id().ToLowerBase16(opentelemetry::nostd::span<char, 32>(buffer, 32));
    traceparent.append(buffer, 32);
    traceparent.append("-");

    // Span ID
    span_context.span_id().ToLowerBase16(opentelemetry::nostd::span<char, 16>(buffer, 16));
    traceparent.append(buffer, 16);
    traceparent.append("-");

    // Trace flags
    auto flags = span_context.trace_flags().flags();
    snprintf(buffer, 3, "%02x", flags);
    traceparent.append(buffer, 2);

    return traceparent;
  }

public:
  SpanContextReader(const opentelemetry::trace::SpanContext &ctx)
  {
    ctx_.emplace("traceparent", make_traceparent(ctx));
    ctx_.emplace("tracestate", ctx.trace_state()->ToHeader());
  }

  ~SpanContextReader() override = default;

  dd::Optional<dd::StringView> lookup(dd::StringView key) const override
  {
    auto it = ctx_.find(std::string{key});
    if (it != ctx_.end())
    {
      return it->second;
    }

    return dd::nullopt;
  };

  // Invoke the specified `visitor` once for each key/value pair in this object.
  void visit(const std::function<void(dd::StringView, dd::StringView)> &visitor) const override {};
};

class ContextReader final : public dd::DictReader
{
  opentelemetry::context::Context context_;

public:
  ContextReader(opentelemetry::context::Context context) : context_(std::move(context)) {}
  ~ContextReader() override = default;

  // Return the value at the specified `key`, or return `nullopt` if there
  // is no value at `key`.
  dd::Optional<dd::StringView> lookup(dd::StringView) const override
  {
    /*auto v = context_.GetValue({key.data(), key.size()});*/
    return dd::nullopt;
  };

  // Invoke the specified `visitor` once for each key/value pair in this object.
  void visit(const std::function<void(dd::StringView, dd::StringView)> &visitor) const override {};
};

}  // namespace

Tracer::Tracer(const ::datadog::tracing::FinalizedTracerConfig &cfg) : tracer_(cfg) {}

nostd::shared_ptr<opentelemetry::trace::Span> Tracer::StartSpan(
    nostd::string_view name,
    const common::KeyValueIterable &attributes,
    const opentelemetry::trace::SpanContextKeyValueIterable & /*links*/,
    const opentelemetry::trace::StartSpanOptions &opts) noexcept
{
  dd::SpanConfig span_cfg;
  span_cfg.name = std::string(name);

  auto set_tags = [&span_cfg](nostd::string_view key, common::AttributeValue attribute) {
    nostd::visit(details::Overload{
                     [&](const char *cstr) { span_cfg.tags.emplace(key.data(), cstr); },
                     [&](nostd::string_view sv) { span_cfg.tags.emplace(key.data(), sv); },
                     [&](nostd::span<const nostd::string_view> span) {
                       for (const auto &sv : span)
                       {
                         span_cfg.tags.emplace(key.data(), sv);
                       }
                     },
                     [](auto &) {},
                 },
                 attribute);
    return true;
  };

  attributes.ForEachKeyValue(set_tags);

  auto span_kind = [](trace::SpanKind kind) {
    using trace::SpanKind;
    switch (kind)
    {
      case SpanKind::kInternal:
        return "internal";
      case SpanKind::kServer:
        return "server";
      case SpanKind::kClient:
        return "client";
      case SpanKind::kProducer:
        return "producer";
      case SpanKind::kConsumer:
        return "consumer";
    }
  };

  span_cfg.tags.emplace("span.kind", span_kind(opts.kind));

  std::shared_ptr<Span> span = nullptr;

  auto active_span = GetCurrentSpan();
  auto parent_ctx  = active_span->GetContext();

  if (nostd::holds_alternative<opentelemetry::context::Context>(opts.parent))
  {
    auto context = nostd::get<opentelemetry::context::Context>(opts.parent);
    if (opentelemetry::trace::IsRootSpan(context))
    {
      span = std::make_shared<Span>(tracer_.create_span(span_cfg));
    }
    else
    {
      ContextReader reader(context);
      span = std::make_shared<Span>(tracer_.extract_or_create_span(reader, span_cfg));
    }

    goto save_span;
  }

  if (nostd::holds_alternative<opentelemetry::trace::SpanContext>(opts.parent))
  {
    const auto &spanContext = nostd::get<opentelemetry::trace::SpanContext>(opts.parent);
    if (spanContext.IsValid())
    {
      parent_ctx = spanContext;
    }
  }

  if (parent_ctx.IsValid())
  {
    auto it = parents_map_.find(parent_ctx.span_id());
    if (it != parents_map_.cend())
    {
      const auto &parent_span = it->second;
      if (auto p = it->second.lock())
      {
        const auto &dd_span = static_cast<Span &>(*p).span_;
        span                = std::make_shared<Span>(dd_span.create_child(span_cfg));
      }
      else
      {
        parents_map_.erase(it);
      }
    }
    else
    {
      SpanContextReader reader(parent_ctx);
      span = std::make_shared<Span>(tracer_.extract_or_create_span(reader, span_cfg));
    }
  }

  if (span == nullptr)
  {
    span = std::make_shared<Span>(tracer_.create_span(span_cfg));
  }

save_span:
  auto sid = SpanId(span->span_.id());
  parents_map_.emplace(sid, span);
  return nostd::shared_ptr<opentelemetry::trace::Span>(span);
}

#if OPENTELEMETRY_ABI_VERSION_NO == 1
void Tracer::ForceFlushWithMicroseconds(uint64_t timeout) noexcept
{
  // TBD
}

void Tracer::CloseWithMicroseconds(uint64_t timeout) noexcept
{
  // Not supported
}
#endif /* OPENTELEMETRY_ABI_VERSION_NO */

}  // namespace datadog
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
