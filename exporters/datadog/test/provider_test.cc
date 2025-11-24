// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/exporters/datadog/tracer_provider.h"

using namespace OPENTELEMETRY_NAMESPACE;

TEST(DatadogProvider, DifferentNameYieldDifferentTracerInstances)
{
  exporter::datadog::TracerProvider provider;
  auto t1 = provider.GetTracer("t1");
  ASSERT_TRUE(t1 != nullptr);
  auto t2 = provider.GetTracer("t2");
  ASSERT_TRUE(t2 != nullptr);

  EXPECT_NE(t1, t2);
}

TEST(DatadogProvider, SameNameYieldTheSameTracerInstance)
{
  exporter::datadog::TracerProvider provider;
  auto t1 = provider.GetTracer("t1");

  auto t1_prime = provider.GetTracer("t1");
  ASSERT_TRUE(t1 != nullptr);
  EXPECT_EQ(t1, t1_prime);
}
