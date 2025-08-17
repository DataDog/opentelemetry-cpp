# Datadog OpenTelemetry Exporters

This directory contains components to export [traces] captured with
[OpenTelemetry] to [Datadog].

## Quickstart

TBD
The [quickstart/](quickstart/README.md) directory contains a minimal environment
to get started exporting traces from the C++ client libraries. The following
program is used in this quickstart.

<!-- inject-quickstart-start -->

```cc
#include "google/cloud/opentelemetry/configure_basic_tracing.h"
#include "google/cloud/storage/client.h"
#include "google/cloud/opentelemetry_options.h"
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <bucket-name> <project-id>\n";
    return 1;
  }
  std::string const bucket_name = argv[1];
  std::string const project_id = argv[2];

  // Create aliases to make the code easier to read.
  namespace gc = ::google::cloud;
  namespace gcs = ::google::cloud::storage;

  // Instantiate a basic tracing configuration which exports traces to Cloud
  // Trace. By default, spans are sent in batches and always sampled.
  auto project = gc::Project(project_id);
  auto configuration = gc::otel::ConfigureBasicTracing(project);

  // Create a client with OpenTelemetry tracing enabled.
  auto options = gc::Options{}.set<gc::OpenTelemetryTracingOption>(true);
  auto client = gcs::Client(options);

  auto writer = client.WriteObject(bucket_name, "quickstart.txt");
  writer << "Hello World!";
  writer.Close();
  if (!writer.metadata()) {
    std::cerr << "Error creating object: " << writer.metadata().status()
              << "\n";
    return 1;
  }
  std::cout << "Successfully created object: " << *writer.metadata() << "\n";

  auto reader = client.ReadObject(bucket_name, "quickstart.txt");
  if (!reader) {
    std::cerr << "Error reading object: " << reader.status() << "\n";
    return 1;
  }

  std::string contents{std::istreambuf_iterator<char>{reader}, {}};
  std::cout << contents << "\n";

  // The basic tracing configuration object goes out of scope. The collected
  // spans are flushed to Cloud Trace.

  return 0;
}
```

<!-- inject-quickstart-end -->

## More Information

[opentelemetry]: https://opentelemetry.io/
[traces]: https://opentelemetry.io/docs/concepts/observability-primer/#distributed-traces
[Datadog]: https://www.datadoghq.com/
