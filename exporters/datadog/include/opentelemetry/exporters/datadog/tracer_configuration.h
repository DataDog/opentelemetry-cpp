// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

#include <datadog/tracer_config.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace datadog
{

struct TracerConfiguration final : public ::datadog::tracing::TracerConfig
{};

}  // namespace datadog
}  // namespace exporter

OPENTELEMETRY_END_NAMESPACE
