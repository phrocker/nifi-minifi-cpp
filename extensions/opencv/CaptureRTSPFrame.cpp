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
                        auto flow_file = session->get();

                        if (!flow_file) {
                            return;
                        }

                        // For now lets test building and tearing down from scratch on each invocation ....
                        cv::VideoCapture capture("rtsp://admin:Rascal18@192.168.1.200");
                        bool OK = capture.grab();
                        std::shared_ptr<cv::Mat> frame;
                        if (OK == false){
                            logger_->log_error("Unable to Capture RTSP frame!!!");
                        }
                        else{
                            // retrieve a frame of your source
                            if (capture.read(*frame)) {
                                if (!frame->empty()) {
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



//                        try {
//                            // Read graph
//                            std::string tf_type;
//                            flow_file->getAttribute("tf.type", tf_type);
//
//                            std::shared_ptr<tensorflow::GraphDef> graph_def;
//                            uint32_t graph_version;
//
//                            {
//                                std::lock_guard<std::mutex> guard(graph_def_mtx_);
//
//                                if ("graph" == tf_type) {
//                                    logger_->log_info("Reading new graph def");
//                                    graph_def_ = std::make_shared<tensorflow::GraphDef>();
//                                    GraphReadCallback graph_cb(graph_def_);
//                                    session->read(flow_file, &graph_cb);
//                                    graph_version_++;
//                                    logger_->log_info("Read graph version: %i", graph_version_);
//                                    session->remove(flow_file);
//                                    return;
//                                }
//
//                                graph_version = graph_version_;
//                                graph_def = graph_def_;
//                            }
//
//                            if (!graph_def) {
//                                logger_->log_error("Cannot process input because no graph has been defined");
//                                session->transfer(flow_file, Retry);
//                                return;
//                            }
//
//                            // Use an existing context, if one is available
//                            std::shared_ptr<TFContext> ctx;
//
//                            if (tf_context_q_.try_dequeue(ctx)) {
//                                logger_->log_debug("Using available TensorFlow context");
//
//                                if (ctx->graph_version != graph_version) {
//                                    logger_->log_info("Allowing session with stale graph to expire");
//                                    ctx = nullptr;
//                                }
//                            }
//
//                            if (!ctx) {
//                                logger_->log_info("Creating new TensorFlow context");
//                                tensorflow::SessionOptions options;
//                                ctx = std::make_shared<TFContext>();
//                                ctx->tf_session.reset(tensorflow::NewSession(options));
//                                ctx->graph_version = graph_version;
//                                auto status = ctx->tf_session->Create(*graph_def);
//
//                                if (!status.ok()) {
//                                    std::string msg = "Failed to create TensorFlow session: ";
//                                    msg.append(status.ToString());
//                                    throw std::runtime_error(msg);
//                                }
//                            }
//
//                            // Apply graph
//                            // Read input tensor from flow file
//                            auto input_tensor_proto = std::make_shared<tensorflow::TensorProto>();
//                            TensorReadCallback tensor_cb(input_tensor_proto);
//                            session->read(flow_file, &tensor_cb);
//                            tensorflow::Tensor input;
//                            input.FromProto(*input_tensor_proto);
//                            std::vector<tensorflow::Tensor> outputs;
//                            auto status = ctx->tf_session->Run({{input_node_, input}}, {output_node_}, {}, &outputs);
//
//                            if (!status.ok()) {
//                                std::string msg = "Failed to apply TensorFlow graph: ";
//                                msg.append(status.ToString());
//                                throw std::runtime_error(msg);
//                            }
//
//                            // Create output flow file for each output tensor
//                            for (const auto &output : outputs) {
//                                auto tensor_proto = std::make_shared<tensorflow::TensorProto>();
//                                output.AsProtoTensorContent(tensor_proto.get());
//                                logger_->log_info("Writing output tensor flow file");
//                                TensorWriteCallback write_cb(tensor_proto);
//                                session->write(flow_file, &write_cb);
//                                session->transfer(flow_file, Success);
//                            }
//
//                            // Make context available for use again
//                            if (tf_context_q_.size_approx() < getMaxConcurrentTasks()) {
//                                logger_->log_debug("Releasing TensorFlow context");
//                                tf_context_q_.enqueue(ctx);
//                            } else {
//                                logger_->log_info("Destroying TensorFlow context because it is no longer needed");
//                            }
//                        } catch (std::exception &exception) {
//                            logger_->log_error("Caught Exception %s", exception.what());
//                            session->transfer(flow_file, Failure);
//                            this->yield();
//                        } catch (...) {
//                            logger_->log_error("Caught Exception");
//                            session->transfer(flow_file, Failure);
//                            this->yield();
//                        }
                    }


                } /* namespace processors */
            } /* namespace minifi */
        } /* namespace nifi */
    } /* namespace apache */
} /* namespace org */
