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
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include "Serializable.h"

#define htonll_r(x) ((((uint64_t)htonl(x)) << 32) + htonl((x) >> 32))

bool EndiannessCheck::IS_LITTLE = EndiannessCheck::is_little_endian();

#define IS_PRINTABLE(c) __builtin_expect(!!((c >= 1) && (c <= 127)),1)

template<typename T>
int Serializable::writeData(const T &t,DataStream *stream) {
    uint8_t bytes[sizeof t];
    std::copy(static_cast<const char*>(static_cast<const void*>(&t)),
              static_cast<const char*>(static_cast<const void*>(&t)) + sizeof t,
              bytes);
    return stream->writeData(bytes, sizeof t);
}

template<typename T>
int Serializable::writeData(const T &t, uint8_t *to_vec) {
    std::copy(static_cast<const char*>(static_cast<const void*>(&t)),
              static_cast<const char*>(static_cast<const void*>(&t)) + sizeof t,
              to_vec);
    return sizeof t;
}

template<typename T>
int Serializable::writeData(const T &t, std::vector<uint8_t> &to_vec) {
    uint8_t bytes[sizeof t];
    std::copy(static_cast<const char*>(static_cast<const void*>(&t)),
              static_cast<const char*>(static_cast<const void*>(&t)) + sizeof t,
              bytes);
    to_vec.insert(to_vec.end(), &bytes[0], &bytes[sizeof t]);
    return sizeof t;
}





int Serializable::write(uint8_t value,DataStream *stream) {
    return stream->writeData(&value, 1);
}
int Serializable::write(char value,DataStream *stream) {
    return stream->writeData((uint8_t *) &value, 1);
}

int Serializable::write(uint8_t *value, int len,DataStream *stream) {
    return stream->writeData(value, len);
}

int Serializable::write(bool value) {
    uint8_t temp = value;
    return write(temp);
}

int Serializable::read(uint8_t &value,DataStream *stream) {
    uint8_t buf;

    int ret = stream->readData(&buf, 1);
    if (ret == 1)
        value = buf;
    return ret;
}

int Serializable::read(char &value,DataStream *stream) {
    uint8_t buf;

    int ret = stream->readData(&buf, 1);
    if (ret == 1)
        value = (char) buf;
    return ret;
}

int Serializable::read(uint8_t *value, int len,DataStream *stream) {
    return stream->readData(value, len);
}

int Serializable::read(uint16_t &value,DataStream *stream, bool is_little_endian) {

    return stream->readShort(value, is_little_endian);
}

int Serializable::read(uint32_t &value,DataStream *stream, bool is_little_endian) {

    return stream->readLong(value, is_little_endian);

}
int Serializable::read(uint64_t &value,DataStream *stream, bool is_little_endian) {

    return stream->readLongLong(value, is_little_endian);

}

int Serializable::write(uint32_t base_value,DataStream *stream, bool is_little_endian) {

    const uint32_t value = is_little_endian ? htonl(base_value) : base_value;

    return writeData(value,stream);
}

int Serializable::write(uint64_t base_value,DataStream *stream, bool is_little_endian) {

    const uint64_t value =
        is_little_endian == 1 ? htonll_r(base_value) : base_value;
    return writeData(value,stream);
}

int Serializable::write(uint16_t base_value,DataStream *stream, bool is_little_endian) {

    const uint16_t value =
        is_little_endian == 1 ? htons(base_value) : base_value;

    return writeData(value,stream);
}

int Serializable::readUTF(std::string &str,DataStream *stream, bool widen) {
    uint32_t utflen;
    int ret = 1;

    if (!widen) {
        uint16_t shortLength = 0;
        ret = read(shortLength,stream);
        utflen = shortLength;

        if (ret <= 0)
            return ret;
    } else {
        uint32_t len;
        ret = read(len,stream);
        if (ret <= 0)
            return ret;
        utflen = len;
    }

    if (utflen == 0)
        return 1;

    uint8_t *utf_to_write = NULL;
    char *chararr = NULL;
    utf_to_write = new uint8_t[utflen];
    chararr = new char[utflen];
    memset(chararr, 0, utflen);

    int c, char2, char3;
    int count = 0;
    int chararr_count = 0;

    ret = stream->readData(utf_to_write, utflen);
    if (ret <= 0) {
        delete[] utf_to_write;
        delete[] chararr;
        if (ret == 0) {
            if (!widen)
                return (2 + utflen);
            else
                return (4 + utflen);
        } else
            return ret;
    }

    while (count < utflen) {
        c = (int) utf_to_write[count] & 0xff;
        if (c > 127)
            break;
        count++;
        chararr[chararr_count++] = (char) c;
    }

    while (count < utflen) {
        c = (int) utf_to_write[count] & 0xff;
        switch (c >> 4) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            /* 0xxxxxxx*/
            count++;
            chararr[chararr_count++] = (char) c;
            break;
        case 12:
        case 13:
            /* 110x xxxx   10xx xxxx*/
            count += 2;
            if (count > utflen) {
                delete[] utf_to_write;
                delete[] chararr;
                return -1;
            }
            char2 = (int) utf_to_write[count - 1];
            if ((char2 & 0xC0) != 0x80) {
                delete[] utf_to_write;
                delete[] chararr;
                return -1;
            }
            chararr[chararr_count++] = (char) (((c & 0x1F) << 6)
                                               | (char2 & 0x3F));
            break;
        case 14:
            /* 1110 xxxx  10xx xxxx  10xx xxxx */
            count += 3;
            if (count > utflen) {
                delete[] utf_to_write;
                delete[] chararr;
                return -1;
            }
            char2 = (int) utf_to_write[count - 2];
            char3 = (int) utf_to_write[count - 1];
            if (((char2 & 0xC0) != 0x80) || ((char3 & 0xC0) != 0x80)) {
                delete[] utf_to_write;
                delete[] chararr;
                return -1;
            }
            chararr[chararr_count++] = (char) (((c & 0x0F) << 12)
                                               | ((char2 & 0x3F) << 6) | ((char3 & 0x3F) << 0));
            break;
        default:
            delete[] utf_to_write;
            delete[] chararr;
            return -1;
        }
    }
    // The number of chars produced may be less than utflen
    std::string value(chararr, chararr_count);
    str = value;
    delete[] utf_to_write;
    delete[] chararr;
    if (!widen)
        return (2 + utflen);
    else
        return (4 + utflen);
}

int Serializable::writeUTF(std::string str,DataStream *stream, bool widen) {
    int inLength = str.length();
    uint32_t utflen = 0;
    int currentPtr = 0;

    /* use charAt instead of copying String to char array */
    for (auto c : str) {
        if (IS_PRINTABLE(c)) {
            utflen++;
        } else {
            utflen += 2;
        }
    }

    if (utflen > 65535)
        return -1;
    if (utflen == 0) {

        if (!widen) {
            uint16_t shortLen = utflen;
            write(shortLen,stream);
        } else {

        }
        return 1;
    }

    std::vector<uint8_t> utf_to_write;
    if (!widen) {
        utf_to_write.resize(utflen);

        uint16_t shortLen = utflen;
        //	writeData(shortLen,utf_to_write);
        //currentPtr+=2;
        //utf_to_write.data() + currentPtr++ = (uint8_t) ((utflen >> 8) & 255);
        //utf_to_write.data() + currentPtr++ = (uint8_t) ((utflen >> 0) & 255);
    } else {

        utf_to_write.resize(utflen);

        //writeData(utflen,utf_to_write);
//		currentPtr+=4;
        /*		utf_to_write.data() + currentPtr++ = (uint8_t) ((utflen >> 24) & 255);
         utf_to_write.data() + currentPtr++ = (uint8_t) ((utflen >> 16) & 255);
         utf_to_write.data() + currentPtr++ = (uint8_t) ((utflen >> 8) & 255);
         utf_to_write.data() + currentPtr++= (uint8_t) ((utflen >> 0) & 255);*/
    }

    int i = 0;

    /*	for(auto c : str){
     if (IS_PRINTABLE(c)){
     writeData(c,utf_to_write);
     currentPtr++;
     //			utf_to_write.data() + currentPtr++= c;
     }else
     break;
     }*/



    //for (;i < inLength; i++){
    //auto c = str.at(i);
    uint8_t *underlyingPtr = &utf_to_write[0];
    for (auto c : str) {
        if (IS_PRINTABLE(c)) {
            writeData(c, underlyingPtr++);
            //currentPtr++;
            //utf_to_write.data() + currentPtr++= c;
            /*} else if (c > 0x7FF) {
             writeData(utflen,utf_to_write.data()+currentPtr);
             currentPtr+=3;
             utf_to_write.data() + currentPtr++= (uint8_t) (((c >> 0x0C) & 15) | 224);
             utf_to_write.data() + currentPtr++= (uint8_t) (((c >> 0x06) & 63) | 128) ;
             utf_to_write.data() + currentPtr++= (uint8_t) (((c >> 0x00) & 63) | 128);*/
        } else {
            auto t = (uint8_t) (((c >> 0x06) & 31) | 192);
            writeData(t, underlyingPtr++);
            currentPtr++;
            //utf_to_write.data() + currentPtr++= (uint8_t) (((c >> 0x06) & 31) | 192 );
            t = (uint8_t) (((c >> 0x00) & 63) | 128);
            writeData(t, underlyingPtr++);
            currentPtr++;
            //utf_to_write.data() + currentPtr++= (uint8_t) (((c >> 0x00) & 63) | 128);
        }
    }
    int ret;

    if (!widen) {

        uint16_t short_length = utflen;
        write(short_length,stream);

        for (int i = 0; i < utflen; i++) {
        }
        for (auto c : utf_to_write) {
        }
        ret = stream->writeData(utf_to_write.data(), utflen);
    } else {
        utflen += 4;
        write(utflen,stream);
        ret = stream->writeData(utf_to_write.data(), utflen);
    }
    return ret;
}

int DataStream::writeData(uint8_t *value, int size) {

    /*if (buffer.size() + size < buffer.capacity())
    	{
    		buffer.resize( buffer.size() + size );
    	}
    	*/
    std::copy(value,value+size,std::back_inserter(buffer));

    return size;
}

int DataStream::readLongLong(uint64_t &value, bool is_little_endian) {
    if ((8 + readBuffer) > buffer.size()) {
        // if read exceed
        return -1;
    }
    uint8_t *buf = &buffer[readBuffer];

    if (is_little_endian) {
        value = ((uint64_t) buf[0] << 56) | ((uint64_t) (buf[1] & 255) << 48)
                | ((uint64_t) (buf[2] & 255) << 40)
                | ((uint64_t) (buf[3] & 255) << 32)
                | ((uint64_t) (buf[4] & 255) << 24)
                | ((uint64_t) (buf[5] & 255) << 16)
                | ((uint64_t) (buf[6] & 255) << 8)
                | ((uint64_t) (buf[7] & 255) << 0);
    } else {
        value = ((uint64_t) buf[0] << 0) | ((uint64_t) (buf[1] & 255) << 8)
                | ((uint64_t) (buf[2] & 255) << 16)
                | ((uint64_t) (buf[3] & 255) << 24)
                | ((uint64_t) (buf[4] & 255) << 32)
                | ((uint64_t) (buf[5] & 255) << 40)
                | ((uint64_t) (buf[6] & 255) << 48)
                | ((uint64_t) (buf[7] & 255) << 56);
    }
    readBuffer += 8;
    return 8;
}

int DataStream::readLong(uint32_t &value, bool is_little_endian) {
    if ((4 + readBuffer) > buffer.size()) {
        // if read exceed
        return -1;
    }
    uint8_t *buf = &buffer[readBuffer];

    if (is_little_endian) {
        value = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    } else {
        value = buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;

    }
    readBuffer += 4;
    return 4;
}

int DataStream::readShort(uint16_t &value, bool is_little_endian) {

    if ((2 + readBuffer) > buffer.size()) {
        // if read exceed
        return -1;
    }
    uint8_t *buf = &buffer[readBuffer];

    if (is_little_endian) {
        value = (buf[0] << 8) | buf[1];
    } else {
        value = buf[0] | buf[1] << 8;

    }
    readBuffer += 2;
    return 2;
}


int DataStream::readData(uint8_t *buf,int buflen) {
    if ((buflen + readBuffer) > buffer.size()) {
        // if read exceed
        return -1;
    }

    std::copy(&buffer[readBuffer],&buffer[readBuffer+buflen],buf);

    readBuffer += buflen;
    return buflen;
}
