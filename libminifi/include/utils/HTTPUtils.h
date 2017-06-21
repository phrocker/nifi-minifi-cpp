/**
 * HTTPUtils class declaration
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
#ifndef __HTTP_UTILS_H__
#define __HTTP_UTILS_H__

#include <curl/curl.h>
#include <curl/curlbuild.h>
#include <curl/easy.h>
#include <uuid/uuid.h>
#include <regex.h>
#include <vector>
#include "controllers/SSLContextService.h"
#include "ByteInputCallBack.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace utils {

struct HTTPUploadCallback {
  ByteInputCallBack *ptr;
  size_t pos;
};

/**
 * HTTP Response object
 */
struct HTTPRequestResponse {
  std::vector<char> data;

  /**
   * Receive HTTP Response.
   */
  static size_t recieve_write(char * data, size_t size, size_t nmemb,
                              void * p) {
    return static_cast<HTTPRequestResponse*>(p)->write_content(data, size,
                                                               nmemb);
  }

  /**
   * Callback for post, put, and patch operations
   * @param buffer
   * @param size size of buffer
   * @param nitems items to add
   * @param insteam input stream object.
   */

  static size_t send_write(char * data, size_t size, size_t nmemb, void * p) {
    if (p != 0) {
      HTTPUploadCallback *callback = (HTTPUploadCallback*) p;
      if (callback->pos <= callback->ptr->getBufferSize()) {
        char *ptr = callback->ptr->getBuffer();
        int len = callback->ptr->getBufferSize() - callback->pos;
        if (len <= 0) {
          delete callback->ptr;
          delete callback;
          return 0;
        }
        if (len > size * nmemb)
          len = size * nmemb;
        memcpy(data, callback->ptr->getBuffer() + callback->pos, len);
        callback->pos += len;
        return len;
      }
    } else {
      return CURL_READFUNC_ABORT;
    }

    return 0;
  }

  size_t write_content(char* ptr, size_t size, size_t nmemb) {
    data.insert(data.end(), ptr, ptr + size * nmemb);
    return size * nmemb;
  }

};

/**
 * Purpose and Justification: Initialzes and cleans up curl once. Cleanup will only occur at the end of our execution since we are relying on a static variable.
 */
class HTTPClientInitializer {
 public:
  static HTTPClientInitializer *getInstance() {
    static HTTPClientInitializer initializer;
    return &initializer;
  }
 private:
  ~HTTPClientInitializer() {
    curl_global_cleanup();
  }
  HTTPClientInitializer() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
  }
};

/**
 * Purpose and Justification: Pull the basics for an HTTPClient into a self contained class. Simply provide
 * the URL and an SSLContextService ( can be null).
 *
 * Since several portions of the code have been relying on curl, we can encapsulate most CURL HTTP
 * operations here without maintaining it everywhere. Further, this will help with testing as we
 * only need to to test our usage of CURL once
 */
class HTTPClient {
 public:
  HTTPClient(
             const std::string &url,
             const std::shared_ptr<minifi::controllers::SSLContextService> ssl_context_service = nullptr)
      : ssl_context_service_(ssl_context_service),
        url_(url),
        logger_(logging::LoggerFactory<HTTPClient>::getLogger()),
        connect_timeout_(0),
        read_timeout_(0),
        content_type(nullptr),
        headers_(nullptr),
        http_code(0),
        res(CURLE_OK) {
    HTTPClientInitializer *initializer = HTTPClientInitializer::getInstance();
    http_session_ = curl_easy_init();
  }

  ~HTTPClient() {
    if (nullptr != headers_) {
      curl_slist_free_all(headers_);
    }
    curl_easy_cleanup(http_session_);
  }

  void initialize(const std::string &method)
                  {
    method_ = method;
    set_request_method(method_);
    if (isSecure(url_) && ssl_context_service_ != nullptr) {
      configure_secure_connection(http_session_);
    }
  }

  void setConnectionTimeout(int64_t timeout) {
    connect_timeout_ = timeout;
  }

  void setReadTimeout(int64_t timeout) {
    read_timeout_ = timeout;
  }

  void setUploadCallback(HTTPUploadCallback *callbackObj)
                         {
    logger_->log_info("Setting callback");
    curl_easy_setopt(http_session_, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(http_session_, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t )callbackObj->ptr->getBufferSize());
    curl_easy_setopt(http_session_, CURLOPT_READFUNCTION,
                     &utils::HTTPRequestResponse::send_write);
    curl_easy_setopt(http_session_, CURLOPT_READDATA,
                     static_cast<void*>(callbackObj));
  }

  struct curl_slist *build_header_list(std::string regex, const std::map<std::string, std::string> &attributes) {
    struct curl_slist *list = NULL;
    if (http_session_) {
      for (auto attribute : attributes) {
        if (matches(attribute.first, regex)) {
          std::string attr = attribute.first + ":" + attribute.second;
          list = curl_slist_append(list, attr.c_str());
        }
      }
    }
    return list;
  }

  void setHeaders(struct curl_slist *list) {
    headers_ = list;
  }

  CURLcode submit()
  {
    curl_easy_setopt(http_session_, CURLOPT_URL, url_.c_str());
    logger_->log_info("Submitting to %s", url_);
    curl_easy_setopt(http_session_, CURLOPT_WRITEFUNCTION,
                     &utils::HTTPRequestResponse::recieve_write);

    curl_easy_setopt(http_session_, CURLOPT_WRITEDATA, static_cast<void*>(&content_));

    res = curl_easy_perform(http_session_);
    curl_easy_getinfo(http_session_, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_getinfo(http_session_, CURLINFO_CONTENT_TYPE, &content_type);
    return res;
  }

  CURLcode getResponseResult() {
    return res;
  }

  int64_t &getResponseCode() {
    return http_code;
  }

  const char *getContentType() {
    return content_type;
  }

  std::string getResponseBody() {
    std::string response_body(content_.data.begin(), content_.data.end());
    return response_body;
  }

  void set_request_method(const std::string method) {
    std::string my_method = method;
    std::transform(my_method.begin(), my_method.end(), my_method.begin(), ::toupper);
    std::cout << "method is now " << my_method << std::endl;
    if (my_method == "POST") {
      curl_easy_setopt(http_session_, CURLOPT_POST, 1);
    } else if (my_method == "PUT") {
      curl_easy_setopt(http_session_, CURLOPT_UPLOAD, 1);
    } else if (my_method == "GET") {
    } else {
      curl_easy_setopt(http_session_, CURLOPT_CUSTOMREQUEST, my_method.c_str());
    }
  }

 protected:

  inline bool matches(const std::string &value, const std::string &sregex) {
    if (sregex == ".*")
      return true;

    regex_t regex;
    int ret = regcomp(&regex, sregex.c_str(), 0);
    if (ret)
      return false;
    ret = regexec(&regex, value.c_str(), (size_t) 0, NULL, 0);
    regfree(&regex);
    if (ret)
      return false;

    return true;
  }

  static CURLcode configure_ssl_context(CURL *curl, void *ctx, void *param) {
    minifi::controllers::SSLContextService *ssl_context_service = static_cast<minifi::controllers::SSLContextService*>(param);
    if (!ssl_context_service->configure_ssl_context(static_cast<SSL_CTX*>(ctx))) {
      return CURLE_FAILED_INIT;
    }
    return CURLE_OK;
  }

  void configure_secure_connection(CURL *http_session) {
    logger_->log_debug("Using certificate file %s", ssl_context_service_->getCertificateFile());
    curl_easy_setopt(http_session, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(http_session, CURLOPT_SSL_CTX_FUNCTION, &configure_ssl_context);
    curl_easy_setopt(http_session, CURLOPT_SSL_CTX_DATA, static_cast<void*>(ssl_context_service_.get()));
  }

  bool isSecure(const std::string &url) {
    if (url.find("https") != std::string::npos) {
      return true;
    }
    return false;
  }
  struct curl_slist *headers_;
  utils::HTTPRequestResponse content_;
  CURLcode res;
  int64_t http_code;
  char *content_type;

  int64_t connect_timeout_;
  // read timeout.
  int64_t read_timeout_;

  std::shared_ptr<logging::Logger> logger_;
  CURL *http_session_;
  std::string url_;
  std::string method_;
  std::shared_ptr<minifi::controllers::SSLContextService> ssl_context_service_;
};

} /* namespace utils */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif
