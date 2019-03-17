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

#include "CaptureRTSPFrame.h"

namespace org {
    namespace apache {
        namespace nifi {
            namespace minifi {
                namespace processors {

                    static core::Property rtspUsername;
                    static core::Property rtspPassword;
                    static core::Property rtspHostname;
                    static core::Property rtspURI;
                    static core::Property captureFrameRate;

                    core::Property CaptureRTSPFrame::RTSPUsername(  // NOLINT
                            "RTSP Username",
                            "The username for connecting to the RTSP stream", "");
                    core::Property CaptureRTSPFrame::RTSPPassword(  // NOLINT
                            "RTSP Password",
                            "Password used to connect to the RTSP stream", "");
                    core::Property CaptureRTSPFrame::RTSPHostname(  // NOLINT
                            "RTSP Hostname",
                            "Hostname of the RTSP stream we are trying to connect to", "");
                    core::Property CaptureRTSPFrame::RTSPURI(  // NOLINT
                            "RTSP URI",
                            "URI that should be appended to the RTSP stream hostname", "");
                    core::Property CaptureRTSPFrame::CaptureFrameRate(  // NOLINT
                            "RTSP Capture Frame Rate",
                            "How many frames should be captured per second from the RTSP stream. This cannot be faster than the upstream source is supplying frame rates", "");

                    core::Relationship CaptureRTSPFrame::Success(  // NOLINT
                            "success",
                            "Successful capture of RTSP frame");
                    core::Relationship CaptureRTSPFrame::Failure(  // NOLINT
                            "failure",
                            "Failures to capture RTSP frame");

                    void CaptureRTSPFrame::initialize() {
                        std::set<core::Property> properties;
                        properties.insert(RTSPUsername);
                        properties.insert(RTSPPassword);
                        properties.insert(RTSPHostname);
                        properties.insert(RTSPURI);
                        properties.insert(CaptureFrameRate);
                        setSupportedProperties(std::move(properties));

                        std::set<core::Relationship> relationships;
                        relationships.insert(Success);
                        relationships.insert(Failure);
                        setSupportedRelationships(std::move(relationships));
                    }

                    void CaptureRTSPFrame::onSchedule(core::ProcessContext *context, core::ProcessSessionFactory *sessionFactory) {

                        std::string value;

                        if (context->getProperty(RTSPUsername.getName(), value)) {
                            rtsp_username_ = value;
                        }
                        if (context->getProperty(RTSPPassword.getName(), value)) {
                            rtsp_password_ = value;
                        }
                        if (context->getProperty(RTSPHostname.getName(), value)) {
                            rtsp_hostname_ = value;
                        }
                        if (context->getProperty(RTSPURI.getName(), value)) {
                            rtsp_uri_ = value;
                        }
                        if (context->getProperty(CaptureFrameRate.getName(), value)) {
                            core::Property::StringToInt(value, capture_framerate_);
                        }
                        logger_->log_trace("CaptureRTSPFrame processor scheduled");
                    }

                    void CaptureRTSPFrame::onTrigger(const std::shared_ptr<core::ProcessContext> &context,
                                                 const std::shared_ptr<core::ProcessSession> &session) {
                        auto flow_file = session->create();

                        // For now lets test building and tearing down from scratch on each invocation ....
                        std::string rtspURI = "rtsp://";
                        rtspURI.append(rtsp_username_);
                        rtspURI.append(":");
                        rtspURI.append(rtsp_password_);
                        rtspURI.append("@");
                        rtspURI.append(rtsp_uri_);

                        //cv::VideoCapture capture("rtsp://admin:Rascal18@192.168.1.200");
                        cv::VideoCapture capture(rtspURI.c_str());
                        bool OK = capture.grab();
                        cv::Mat frame;
                        if (OK == false){
                            logger_->log_error("Unable to Capture RTSP frame!!!");
                        }
                        else{
                            // retrieve a frame of your source
                            if (capture.read(frame)) {
                                if (!frame.empty()) {
                                    logger_->log_info("Writing output capture image flow file");
                                    CaptureRTSPFrameWriteCallback write_cb(frame);
                                    session->write(flow_file, &write_cb);
                                    session->transfer(flow_file, Success);
                                } else {
                                    logger_->log_error("Empty Mat frame received from Capture read command meaning the connection is bad");
                                }
                            } else {
                                logger_->log_error("Unable to read from Capture handle on RTSP stream!!!");
                            }
                        }

                        // Release the Capture reference and free up resources.
                        capture.release();

                    }


                } /* namespace processors */
            } /* namespace minifi */
        } /* namespace nifi */
    } /* namespace apache */
} /* namespace org */
