// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <datadog/trace_id.h>
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"

inline opentelemetry::trace::TraceId TraceId(::datadog::tracing::TraceID id)
{
  union
  {
    std::uint64_t u64[2];
    std::uint8_t u8[16];
  };

  u64[0] = id.high;
  u64[1] = id.low;

  return opentelemetry::trace::TraceId(opentelemetry::nostd::span<const uint8_t, 16>(u8, 16));
}

inline opentelemetry::trace::SpanId SpanId(std::uint64_t id)
{
  union
  {
    std::uint64_t u64;
    std::uint8_t u8[8];
  };

  u64 = id;

  return opentelemetry::trace::SpanId(opentelemetry::nostd::span<const uint8_t, 8>(u8, 8));
}
