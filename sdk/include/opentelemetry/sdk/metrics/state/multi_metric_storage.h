// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/state/metric_storage.h"

#  include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class SyncMultiMetricStorage : public SyncWritableMetricStorage
{
public:
  void AddStorage(std::shared_ptr<SyncWritableMetricStorage> storage)
  {
    storages_.push_back(storage);
  }

  virtual void RecordLong(long value,
                          const opentelemetry::context::Context &context) noexcept override
  {
    for (auto &s : storages_)
    {
      s->RecordLong(value, context);
    }
  }

  virtual void RecordLong(long value,
                          const opentelemetry::common::KeyValueIterable &attributes,
                          const opentelemetry::context::Context &context) noexcept override
  {
    for (auto &s : storages_)
    {
      s->RecordLong(value, attributes, context);
    }
  }

  virtual void RecordDouble(double value,
                            const opentelemetry::context::Context &context) noexcept override
  {
    for (auto &s : storages_)
    {
      s->RecordDouble(value, context);
    }
  }

  virtual void RecordDouble(double value,
                            const opentelemetry::common::KeyValueIterable &attributes,
                            const opentelemetry::context::Context &context) noexcept override
  {
    for (auto &s : storages_)
    {
      s->RecordDouble(value, attributes, context);
    }
  }

private:
  std::vector<std::shared_ptr<SyncWritableMetricStorage>> storages_;
};

class AsyncMultiMetricStorage : public AsyncWritableMetricStorage
{
public:
  void AddStorage(std::shared_ptr<AsyncWritableMetricStorage> storage)
  {
    storages_.push_back(storage);
  }

  void RecordLong(
      const std::unordered_map<MetricAttributes, long, AttributeHashGenerator> &measurements,
      opentelemetry::common::SystemTimestamp observation_time) noexcept override
  {
    for (auto &s : storages_)
    {
      s->RecordLong(measurements, observation_time);
    }
  }

  void RecordDouble(
      const std::unordered_map<MetricAttributes, double, AttributeHashGenerator> &measurements,
      opentelemetry::common::SystemTimestamp observation_time) noexcept override
  {
    for (auto &s : storages_)
    {
      s->RecordDouble(measurements, observation_time);
    }
  }

private:
  std::vector<std::shared_ptr<AsyncWritableMetricStorage>> storages_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
