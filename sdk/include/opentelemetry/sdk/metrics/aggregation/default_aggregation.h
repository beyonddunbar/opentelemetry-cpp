// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <memory>
#  include "opentelemetry/common/spin_lock_mutex.h"
#  include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#  include "opentelemetry/sdk/metrics/aggregation/drop_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/lastvalue_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/sum_aggregation.h"
#  include "opentelemetry/sdk/metrics/data/point_data.h"
#  include "opentelemetry/sdk/metrics/instruments.h"

#  include <mutex>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class DefaultAggregation
{
public:
  static std::unique_ptr<Aggregation> CreateAggregation(
      const opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
      const opentelemetry::sdk::metrics::AggregationConfig *aggregation_config)
  {
    switch (instrument_descriptor.type_)
    {
      case InstrumentType::kCounter:
      case InstrumentType::kUpDownCounter:
      case InstrumentType::kObservableCounter:
      case InstrumentType::kObservableUpDownCounter:
        return (instrument_descriptor.value_type_ == InstrumentValueType::kLong)
                   ? std::move(std::unique_ptr<Aggregation>(new LongSumAggregation()))
                   : std::move(std::unique_ptr<Aggregation>(new DoubleSumAggregation()));
        break;
      case InstrumentType::kHistogram: {
        return (instrument_descriptor.value_type_ == InstrumentValueType::kLong)
                   ? std::move(std::unique_ptr<Aggregation>(new LongHistogramAggregation(
                         static_cast<
                             const opentelemetry::sdk::metrics::HistogramAggregationConfig<long> *>(
                             aggregation_config))))
                   : std::move(std::unique_ptr<Aggregation>(new DoubleHistogramAggregation(
                         static_cast<const opentelemetry::sdk::metrics::HistogramAggregationConfig<
                             double> *>(aggregation_config))));
        break;
      }
      case InstrumentType::kObservableGauge:
        return (instrument_descriptor.value_type_ == InstrumentValueType::kLong)
                   ? std::move(std::unique_ptr<Aggregation>(new LongLastValueAggregation()))
                   : std::move(std::unique_ptr<Aggregation>(new DoubleLastValueAggregation()));
        break;
      default:
        return std::move(std::unique_ptr<Aggregation>(new DropAggregation()));
    };
  }

  static std::unique_ptr<Aggregation> CreateAggregation(AggregationType aggregation_type,
                                                        InstrumentDescriptor instrument_descriptor)
  {
    switch (aggregation_type)
    {
      case AggregationType::kDrop:
        return std::unique_ptr<Aggregation>(new DropAggregation());
        break;
      case AggregationType::kHistogram:
        if (instrument_descriptor.value_type_ == InstrumentValueType::kLong)
        {
          return std::unique_ptr<Aggregation>(new LongHistogramAggregation());
        }
        else
        {
          return std::unique_ptr<Aggregation>(new DoubleHistogramAggregation());
        }
        break;
      case AggregationType::kLastValue:
        if (instrument_descriptor.value_type_ == InstrumentValueType::kLong)
        {
          return std::unique_ptr<Aggregation>(new LongLastValueAggregation());
        }
        else
        {
          return std::unique_ptr<Aggregation>(new DoubleLastValueAggregation());
        }
        break;
      case AggregationType::kSum:
        if (instrument_descriptor.value_type_ == InstrumentValueType::kLong)
        {
          return std::unique_ptr<Aggregation>(new LongSumAggregation());
        }
        else
        {
          return std::unique_ptr<Aggregation>(new DoubleSumAggregation());
        }
        break;
      default:
        return DefaultAggregation::CreateAggregation(instrument_descriptor, nullptr);
    }
  }

  static std::unique_ptr<Aggregation> CloneAggregation(AggregationType aggregation_type,
                                                       InstrumentDescriptor instrument_descriptor,
                                                       const Aggregation &to_copy)
  {
    const PointType point_data = to_copy.ToPoint();
    switch (aggregation_type)
    {
      case AggregationType::kDrop:
        return std::unique_ptr<Aggregation>(new DropAggregation());
      case AggregationType::kHistogram:
        if (instrument_descriptor.value_type_ == InstrumentValueType::kLong)
        {
          return std::unique_ptr<Aggregation>(
              new LongHistogramAggregation(nostd::get<HistogramPointData>(point_data)));
        }
        else
        {
          return std::unique_ptr<Aggregation>(
              new DoubleHistogramAggregation(nostd::get<HistogramPointData>(point_data)));
        }
      case AggregationType::kLastValue:
        if (instrument_descriptor.value_type_ == InstrumentValueType::kLong)
        {
          return std::unique_ptr<Aggregation>(
              new LongLastValueAggregation(nostd::get<LastValuePointData>(point_data)));
        }
        else
        {
          return std::unique_ptr<Aggregation>(
              new DoubleLastValueAggregation(nostd::get<LastValuePointData>(point_data)));
        }
      case AggregationType::kSum:
        if (instrument_descriptor.value_type_ == InstrumentValueType::kLong)
        {
          return std::unique_ptr<Aggregation>(
              new LongSumAggregation(nostd::get<SumPointData>(point_data)));
        }
        else
        {
          return std::unique_ptr<Aggregation>(
              new DoubleSumAggregation(nostd::get<SumPointData>(point_data)));
        }
      default:
        return DefaultAggregation::CreateAggregation(instrument_descriptor, nullptr);
    }
  }
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif