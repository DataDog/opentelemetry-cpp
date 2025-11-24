// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/exporters/datadog/tracer_provider.h"

using namespace OPENTELEMETRY_NAMESPACE;

// TODO: Update this when the Datadog tracer supports instrumentation scopes.
// For now, regardless of the GetTracer inputs, the same tracer instance is returned.
TEST(DatadogProvider, GetTracer)
{
  exporter::datadog::TracerProvider provider;
  auto t1 = provider.GetTracer("t1");
  auto t1_prime = provider.GetTracer("t1");
  auto t2 = provider.GetTracer("t2");

  ASSERT_NE(nullptr, t1);
  ASSERT_NE(nullptr, t1_prime);
  ASSERT_NE(nullptr, t2);

  EXPECT_EQ(t1, t1_prime);
  EXPECT_EQ(t1, t2); // TODO: When fixed, assert that t1 != t2
}
