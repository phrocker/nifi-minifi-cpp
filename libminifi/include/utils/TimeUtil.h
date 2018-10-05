/**
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
#ifndef __TIME_UTIL_H__
#define __TIME_UTIL_H__

#define __STDC_WANT_LIB_EXT1__ 1
#include <time.h>
#include <string.h>
#include <iomanip>
#include <sstream>
#include <chrono>
#include "capi/expect.h"


#define TIME_FORMAT "%Y-%m-%d %H:%M:%S" // should be the same as %T

 /**
  * Gets the current time in milliseconds
  * @returns milliseconds since epoch
  */
inline uint64_t getTimeMillis() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

/**
 * Gets the current time in nanoseconds
 * @returns nanoseconds since epoch
 */
inline uint64_t getTimeNano() {

	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

}


/**
 * Returns a string based on TIME_FORMAT, converting
 * the parameter to a string
 * @param msec milliseconds since epoch
 * @returns string representing the time
 */
inline std::string getTimeStr(uint64_t msec, bool enforce_locale = false) {
	char date[120];
	time_t second = (time_t)(msec / 1000);
	msec = msec % 1000;

	if (UNLIKELY(enforce_locale)) {
		strftime(date, sizeof(date) / sizeof(*date), TIME_FORMAT, localtime(&second));
	}
	else {
#if defined(__STDC_LIB_EXT1__) || defined(WIN32)
		struct tm timeinfo;
#ifdef WIN32
		_gmtime64_s(&timeinfo, &second);
#else
		gmtime_s(&timeinfo, &second);
#endif
		strftime(date, sizeof(date) / sizeof(*date), TIME_FORMAT, &timeinfo);
#else
		strftime(date, sizeof(date) / sizeof(*date), TIME_FORMAT, gmtime(&second));
#endif
	}
	std::string ret = date;
	date[0] = '\0';
	sprintf(date, ".%03llu", (unsigned long long) msec);

	ret += date;
	return ret;
}

#endif
