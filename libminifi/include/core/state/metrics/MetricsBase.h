/**
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef LIBMINIFI_INCLUDE_C2_METRICS_METRICSBASE_H_
#define LIBMINIFI_INCLUDE_C2_METRICS_METRICSBASE_H_

#include <vector>
#include <memory>
#include <string>

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace state {
namespace metrics {

struct MetricResponse {
  std::string name;
  std::string value;
  std::vector<MetricResponse> children;
  MetricResponse &operator=(const MetricResponse &other) {
    name = other.name;
    value = other.value;
    children = other.children;
    return *this;
  }
};

/**
 * Purpose Defines a metric.
 */
class Metrics
{
 public:
  virtual ~Metrics() {

  }
  virtual std::string getName() = 0;

  virtual std::vector<MetricResponse> serialize() = 0;

};

/**
 * Purpose: Retrieves Metrics from the defined class. The current Metric, which is a consumable for any reader of Metrics must have the ability to set metrics.
 *
 */
class MetricsSource
{
 public:

  MetricsSource() {

  }

  virtual ~MetricsSource() {
  }

  /**
   * Retrieves all emtrics from this source.
   * @param metric_vector -- metrics will be placed in this vector.
   * @return result of the get operation.
   *  0 Success
   *  1 No error condition, but cannot obtain lock in timely manner.
   *  -1 failure
   */
  virtual int16_t getMetrics(std::vector<std::shared_ptr<Metrics>> &metric_vector) = 0;

};

/**
 * Purpose: Sink interface for all metrics. The current Metric, which is a consumable for any reader of Metrics must have the ability to set metrics.
 *
 */
class MetricsSink
{
 public:

  virtual ~MetricsSink() {
  }
  /**
   * Setter for metrics in this sink.
   * @param metrics metrics to insert into the current sink.
   * @return result of the set operation.
   *  0 Success
   *  1 No error condition, but cannot obtain lock in timely manner.
   *  -1 failure
   */
  virtual int16_t setMetrics(const std::shared_ptr<Metrics> &metrics) = 0;
};

} /* namespace metrics */
} /* namespace state */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_C2_METRICS_METRICSBASE_H_ */
