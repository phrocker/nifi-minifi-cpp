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
#ifndef LIBMINIFI_INCLUDE_CORE_STATE_METRICS_SYSMETRICS_H_
#define LIBMINIFI_INCLUDE_CORE_STATE_METRICS_SYSMETRICS_H_

#include "core/Resource.h"
#include <sstream>
#include <map>
#if defined(__linux__) || defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#endif
#include "MetricsBase.h"
#include "Connection.h"
#include "DeviceInformation.h"
namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace state {
namespace metrics {

/**
 * Justification and Purpose: Provides Connection queue metrics. Provides critical information to the
 * C2 server.
 *
 */
class SystemInformation : public DeviceInformation {
 public:
   
   SystemInformation(const std::string &name, uuid_t uuid) : DeviceInformation(name,uuid)
  {
  }

  
   SystemInformation(const std::string &name) : DeviceInformation(name,0)
  {
  }
   
  SystemInformation() : DeviceInformation("SystemInformation",0){
  }

  std::string getName() {
    return "SystemInformation";
  }

  std::vector<MetricResponse> serialize() {
    std::vector<MetricResponse> serialized;

    MetricResponse vcores;
    vcores.name = "vcores";
    int cpus[2] = { 0 };
#if defined(CTL_HW)
    cpus[0] = CTL_HW;
#if defined(HW_NCPU)
    cpus[1] = HW_NCPU;

    size_t ncpus = getSystemMetric(cpus);
#else
    size_t ncpus = 0;
#endif
#else 
    size_t ncpus = std::thread::hardware_concurrency();
#endif

    vcores.value = std::to_string(ncpus);

    serialized.push_back(vcores);

    MetricResponse mem;
    mem.name = "physicalmem";
#if defined(CTL_HW)
    int bits[2] = { 0 };
    bits[0] = CTL_HW;
#if defined(HW_MEMSIZE)
    bits[1] = HW_MEMSIZE;

    size_t mema = getSystemMetric(bits);
#else
    size_t mema = 0;
#endif
#elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)
	size_t mema = (size_t)sysconf( _SC_PHYS_PAGES ) *
		(size_t)sysconf( _SC_PAGESIZE );

#endif
    mem.value = std::to_string(mema);

    serialized.push_back(mem);

    MetricResponse arch;
    arch.name = "machinearch";
#if defined(CTL_HW)
    int arc[2] = { 0 };
    arc[0] = CTL_HW;
#if defined(HW_MACHINE)
    arc[1] = HW_MACHINE;

    std::string sysarch = getSystemString(arc);
    arch.value = sysarch;

    if (arch.value.empty() || arch.value == "null"){
      arch.value="unknown";
    }
    serialized.push_back(arch);
#endif

#endif

    return serialized;
  }

 protected:

  size_t getSystemMetric(int bit[2]) {

    int64_t value = 0;
    size_t len = sizeof(value);
    if (sysctl(bit, 2, &value, &len, NULL, 0) == 0)
      return (size_t) value;
    return 0L;
    return 0;
  }

  std::string getSystemString(int bit[2]) {

    char str[1024];
    size_t len = 1024;
    if (sysctl(bit, 2, &str, &len, NULL, 0) == 0)
      return std::string(str);
    return "";
  }

};

REGISTER_RESOURCE(SystemInformation);
} /* namespace metrics */
} /* namespace state */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_STATE_METRICS_QUEUEMETRICS_H_ */
