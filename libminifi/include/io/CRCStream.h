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
#ifndef LIBMINIFI_INCLUDE_IO_CRCSTREAM_H_
#define LIBMINIFI_INCLUDE_IO_CRCSTREAM_H_

#include <zlib.h>
#include <memory>

#include "BaseStream.h"
#include "Serializable.h"


template<typename T>
class CRCStream: public BaseStream {
public:
	/**
	 * Raw pointer because the caller guarantees that
	 * it will exceed our lifetime.
	 */
	explicit CRCStream(T *stream);

	explicit CRCStream( CRCStream<T> &&move );

	virtual ~CRCStream() {

	}

	/**
	 * Reads data and places it into buf
	 * @param buf buffer in which we extract data
	 * @param buflen
	 */
	virtual int readData(std::vector<uint8_t> &buf, int buflen);
	/**
	 * Reads data and places it into buf
	 * @param buf buffer in which we extract data
	 * @param buflen
	 */
	virtual int readData(uint8_t *buf, int buflen);

	/**
	 * Write value to the stream using std::vector
	 * @param buf incoming buffer
	 * @param buflen buffer to write
	 *
	 */
	virtual int writeData(std::vector<uint8_t> &buf, int buflen);

	/**
	 * writes value to stream
	 * @param value value to write
	 * @param size size of value
	 */
	virtual int writeData(uint8_t *value, int size);

	virtual short initialize() {
		child_stream->initialize();
		reset();
		return 0;
	}


	void updateCRC(uint8_t *buffer, uint32_t length);

	uint64_t getCRC() {
		return crc;
	}

	void reset();
protected:
	uint64_t crc;
	T *child_stream;
};


template<typename T>
CRCStream<T>::CRCStream(T *other) :
		child_stream(other) {
	crc = crc32(0L, Z_NULL, 0);
}

template<typename T>
CRCStream<T>::CRCStream(CRCStream<T> &&move) :
		crc(std::move(move.crc)), child_stream(std::move(move.child_stream)) {

}

template<typename T>
int CRCStream<T>::readData(std::vector<uint8_t> &buf, int buflen) {

	if (buf.capacity() < buflen)
		return -1;
	return readData((uint8_t*) &buf[0], buflen);
}

template<typename T>
int CRCStream<T>::readData(uint8_t *buf, int buflen) {
	int ret = child_stream->read(buf, buflen);
	crc = crc32(crc, buf, buflen);
	return ret;
}

template<typename T>
int CRCStream<T>::writeData(std::vector<uint8_t> &buf, int buflen) {

	if (buf.capacity() < buflen)
		return -1;
	return writeData((uint8_t*) &buf[0], buflen);
}

template<typename T>
int CRCStream<T>::writeData(uint8_t *value, int size) {

	int ret = child_stream->write(value, size);
	crc = crc32(crc, value, size);
	return ret;

}
template<typename T>
void CRCStream<T>::reset() {
	crc = crc32(0L, Z_NULL, 0);
}
template<typename T>
void CRCStream<T>::updateCRC(uint8_t *buffer, uint32_t length) {
	crc = crc32(crc, buffer, length);
}

#endif /* LIBMINIFI_INCLUDE_IO_CRCSTREAM_H_ */
