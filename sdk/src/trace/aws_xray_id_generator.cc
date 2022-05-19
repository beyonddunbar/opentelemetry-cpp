#include "opentelemetry/sdk/trace/aws_xray_id_generator.h"

#include <ctime>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/version.h"
#include "src/common/random.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

trace_api::SpanId AwsXrayIdGenerator::GenerateSpanId() noexcept
{
  uint8_t span_id_buf[trace_api::SpanId::kSize];
  sdk::common::Random::GenerateRandomBuffer(span_id_buf);
  return trace_api::SpanId(span_id_buf);
}

trace_api::TraceId AwsXrayIdGenerator::GenerateTraceId() noexcept
{
  uint8_t trace_id_buf[trace_api::TraceId::kSize];
  // The first 8-hex-digits (4 bytes) represent the timestamp at which the trace is generated
  uint32_t now = std::time(0);
  trace_id_buf[0] = (now >> 24) & 0xFF;
  trace_id_buf[1] = (now >> 16) & 0xFF;
  trace_id_buf[2] = (now >> 8) & 0xFF;
  trace_id_buf[3] = now & 0xFF;
  // The remaining 24-hex-digits (12 bytes) are randomly generated
  opentelemetry::nostd::span<uint8_t> buf_span(std::begin(trace_id_buf) + 4, 12);
  sdk::common::Random::GenerateRandomBuffer(buf_span);
  return trace_api::TraceId(trace_id_buf);
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
