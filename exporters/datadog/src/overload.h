#pragma once
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace datadog
{
namespace details
{

/// NOTE(@dmehala): Generic overload function (P0051R3) like implementation.
template <typename... Ts>
struct Overload : Ts...
{
  using Ts::operator()...;
};

/// NOTE(@dmehala): Guide required for C++17. Remove once we switch to C++20.
template <class... Ts>
Overload(Ts...) -> Overload<Ts...>;

}  // namespace details
}  // namespace datadog
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
