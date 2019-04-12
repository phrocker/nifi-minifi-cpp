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

#include "ObjectDetection.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

static core::Property HaarCascadeXMLPath;
static core::Property ImageEncoding;

core::Property ObjectDetection::HaarCascadeXMLPath(  // NOLINT
    "HaarCascading XML definition Path",
    "Local filesystem path where the HaarCascading XML definition can be loaded from for use in object detection",
    "/usr/local/Cellar/opencv/4.0.1/share/opencv4/haarcascades/haarcascade_frontalface_alt.xml");

core::Property ObjectDetection::ImageEncoding( // NOLINT
    "Image Encoding",
    "The encoding that should be applied the the frame images captured from the RTSP stream",
    ".jpg"
);

core::Relationship ObjectDetection::Success(  // NOLINT
    "success",
    "Successful capture of RTSP frame");
core::Relationship ObjectDetection::Failure(  // NOLINT
    "failure",
    "Failures to capture RTSP frame");

void ObjectDetection::initialize() {
  std::set<core::Property> properties;
  properties.insert(HaarCascadeXMLPath);
  properties.insert(ImageEncoding);
  setSupportedProperties(std::move(properties));

  std::set<core::Relationship> relationships;
  relationships.insert(Success);
  relationships.insert(Failure);
  setSupportedRelationships(std::move(relationships));
}

void ObjectDetection::onSchedule(core::ProcessContext *context, core::ProcessSessionFactory *sessionFactory) {

  std::string value;

  if (context->getProperty(HaarCascadeXMLPath.getName(), value)) {
    haar_cascade_xml_path_ = value;
  }
  if (context->getProperty(ImageEncoding.getName(), value)) {
    image_encoding_ = value;
  }

  logger_->log_trace("ObjectDetection processor scheduled");
}

void ObjectDetection::onTrigger(const std::shared_ptr<core::ProcessContext> &context,
                                const std::shared_ptr<core::ProcessSession> &session) {

  std::shared_ptr<FlowFileRecord> flowFile = std::static_pointer_cast<FlowFileRecord>(session->get());

  // Do nothing if there are no incoming files
  if (!flowFile) {
    logger_->log_debug("No incoming FlowFile was found. Returning");
    return;
  }

  // Read the incoming image
  cv::Mat image_mat_;
  ObjectDetection::ObjectDetectionReadCallback object_rb(flowFile, &image_mat_);
  session->read(flowFile, &object_rb);

  // Detect objects in the incoming image
  double scale = 0.5;

  cv::CascadeClassifier face_cascade(haar_cascade_xml_path_);
  std::vector<cv::Rect> faces;
  cv::Mat frame_gray;

  //Convert to gray
  cv::cvtColor(image_mat_, frame_gray, cv::COLOR_BGR2GRAY);
  equalizeHist(frame_gray, frame_gray);

  //Scale to improve performance
  resize(frame_gray, frame_gray, cv::Size(), scale, scale);

  // Detect faces
  face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

  //Scale face params to original image size
  float bSale = 1 / scale;

  for (auto const& face : faces){
    cv::Point pt1(face.x * bSale, face.y * bSale); // Display detected faces on main window - live stream from camera
    cv::Point pt2((face.x * bSale + face.height * bSale), (face.y * bSale + face.width * bSale));

    //Draw
    rectangle(image_mat_, pt1, pt2, cv::Scalar(0, 255, 0), 2, 8, 0);

    ObjectDetection::ObjectDetectionWriteCallback write_cb(image_mat_, image_encoding_);
    session->write(flowFile, &write_cb);
    session->transfer(flowFile, Success);
  }

  frame_gray.release();
  image_mat_.release();
}

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
