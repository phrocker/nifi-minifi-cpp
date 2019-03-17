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

#ifndef NIFI_MINIFI_CPP_OBJECTDETECTION_H
#define NIFI_MINIFI_CPP_OBJECTDETECTION_H

#include <atomic>

#include <core/Resource.h>
#include <core/Processor.h>
#include <opencv2/opencv.hpp>

namespace org {
    namespace apache {
        namespace nifi {
            namespace minifi {
                namespace processors {


                    class ObjectDetection : public core::Processor {

                    public:

                        explicit ObjectDetection(const std::string &name, utils::Identifier uuid = utils::Identifier())
                                : Processor(name, uuid),
                                  logger_(logging::LoggerFactory<ObjectDetection>::getLogger()) {
                        }

                        static core::Property RTSPUsername;

                        static core::Relationship Success;
                        static core::Relationship Failure;

                        void initialize() override;
                        void onSchedule(core::ProcessContext *context, core::ProcessSessionFactory *sessionFactory) override;
                        void onTrigger(core::ProcessContext *context, core::ProcessSession *session) override {
                            logger_->log_error("onTrigger invocation with raw pointers is not implemented");
                        }
                        void onTrigger(const std::shared_ptr<core::ProcessContext> &context,
                                       const std::shared_ptr<core::ProcessSession> &session) override;

                        class ObjectDetectionWriteCallback : public OutputStreamCallback {
                        public:
                            explicit ObjectDetectionWriteCallback(cv::Mat image_mat)
                                    : image_mat_(std::move(image_mat)) {
                            }
                            ~ObjectDetectionWriteCallback() override = default;

                            int64_t process(std::shared_ptr<io::BaseStream> stream) override {
                                int64_t ret = 0;
                                if (image_buf_.size() > 0) {
                                    imencode(".jpg", image_mat_, image_buf_);
                                    ret = stream->write(image_buf_.data(), image_buf_.size());
                                }
                                return ret;
                            }

                        private:
                            std::vector<uchar> image_buf_;
                            cv::Mat image_mat_;
                        };

                    private:
                        std::shared_ptr<logging::Logger> logger_;
                        std::string rtsp_username_;

                    };

                    REGISTER_RESOURCE(ObjectDetection, "Captures a frame from the RTSP stream at specified intervals."); // NOLINT

                } /* namespace processors */
            } /* namespace minifi */
        } /* namespace nifi */
    } /* namespace apache */
} /* namespace org */


#endif //NIFI_MINIFI_CPP_OBJECTDETECTION_H
