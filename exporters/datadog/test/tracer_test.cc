// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/exporters/datadog/tracer.h"
#include "opentelemetry/exporters/datadog/tracer_provider.h"

#include <datadog/null_collector.h>

using namespace OPENTELEMETRY_NAMESPACE;

namespace
{

exporter::datadog::TracerConfiguration make_cfg()
{
  exporter::datadog::TracerConfiguration cfg;
  cfg.collector = std::make_shared<::datadog::tracing::NullCollector>();
  return cfg;
}

}  // namespace

TEST(DatadogTracer, CheckSpanContext)
{
  exporter::datadog::TracerProvider tp(make_cfg());
  auto tracer = tp.GetTracer("test");

  auto span = tracer->StartSpan("FirstSpan");

  auto context = span->GetContext();
  ASSERT_TRUE(context.IsValid());
}

TEST(DatadogTracer, CheckParenting_ActiveContext)
{
  exporter::datadog::TracerProvider tp(make_cfg());
  auto tracer = tp.GetTracer("test-parenting");

  auto root_span  = tracer->StartSpan("root-span");
  auto root_scope = tracer->WithActiveSpan(root_span);

  auto child_span = tracer->StartSpan("child-span");

  auto root_context = root_span->GetContext();
  ASSERT_TRUE(root_context.IsValid());

  auto child_context = child_span->GetContext();
  ASSERT_TRUE(child_context.IsValid());

  EXPECT_EQ(root_context.trace_id(), child_context.trace_id());
  EXPECT_EQ(root_context.IsSampled(), child_context.IsSampled());
}

TEST(DatadogTracer, CheckParenting_SpanOptions)
{
  exporter::datadog::TracerProvider tp(make_cfg());
  auto tracer = tp.GetTracer("test-parenting-with-span-opts");

  auto root_span    = tracer->StartSpan("root-span");
  auto root_context = root_span->GetContext();
  ASSERT_TRUE(root_context.IsValid());

  opentelemetry::trace::StartSpanOptions opts;
  opts.parent = root_context;

  auto child_span    = tracer->StartSpan("child-span-opts", opts);
  auto child_context = child_span->GetContext();
  ASSERT_TRUE(child_context.IsValid());

  EXPECT_EQ(root_context.trace_id(), child_context.trace_id());
  EXPECT_EQ(root_context.IsSampled(), child_context.IsSampled());
}

TEST(DatadogTracer, CheckParenting_SpanOptions_FromAnotherTracer)
{
  exporter::datadog::TracerProvider tp(make_cfg());
  auto tracer_serviceA = tp.GetTracer("test-parenting-service-A");

  auto serviceA_span    = tracer_serviceA->StartSpan("serviceA-span");
  auto serviceA_context = serviceA_span->GetContext();
  ASSERT_TRUE(serviceA_context.IsValid());

  auto tracer_serviceB = tp.GetTracer("test-parenting-service-B");

  opentelemetry::trace::StartSpanOptions opts;
  opts.parent = serviceA_context;

  auto serviceB_span    = tracer_serviceB->StartSpan("serviceB-span", opts);
  auto serviceB_context = serviceB_span->GetContext();
  ASSERT_TRUE(serviceB_context.IsValid());

  EXPECT_EQ(serviceA_context.trace_id(), serviceB_context.trace_id());
  EXPECT_EQ(serviceA_context.IsSampled(), serviceB_context.IsSampled());
}
