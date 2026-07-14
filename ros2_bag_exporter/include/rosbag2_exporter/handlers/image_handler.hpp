/*
 * This file is based on the original work by:
 *
 * Original Author: Abdalrahman M. Amer, www.linkedin.com/in/abdalrahman-m-amer
 * Original Date: 13.10.2024
 *
 * Modified By: Hector Cruz, hcruzgo@ed.ac.uk
 * Modification Date: 27.01.2025
 *
 * Changes:
 * - Remove unused process_compressed_message member function
 */

#ifndef ROSBAG2_EXPORTER__HANDLERS__IMAGE_HANDLER_HPP_
#define ROSBAG2_EXPORTER__HANDLERS__IMAGE_HANDLER_HPP_

#include "rclcpp/logging.hpp"
#include "rosbag2_exporter/handlers/base_handler.hpp"

#include <opencv2/opencv.hpp>

#include "sensor_msgs/msg/compressed_image.hpp"
#include "sensor_msgs/msg/image.hpp"

#include <cv_bridge/cv_bridge.hpp>

#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace rosbag2_exporter
{

class ImageHandler : public BaseHandler
{
public:
  // Constructor to accept logger and encoding, with a default value for encoding
  ImageHandler(const std::string & topic_dir, const std::string & encoding, rclcpp::Logger logger)
  : BaseHandler(logger), topic_dir_(topic_dir)
  {
    // Validate or set default encoding if not provided
    if (encoding.empty()) {
      RCLCPP_WARN(logger, "No encoding provided. Defaulting to 'bgr8'.");
      // Default to bgr8 if encoding is not provided
      encoding_ = "bgr8";
    } else {
      encoding_ = encoding;
    }
  }

  // Handle uncompressed image messages
  void process_message(const rclcpp::SerializedMessage & serialized_msg, size_t global_id) override
  {
    // Deserialize the incoming uncompressed image message
    sensor_msgs::msg::Image img_msg;
    rclcpp::Serialization<sensor_msgs::msg::Image> serializer;
    serializer.deserialize_message(&serialized_msg, &img_msg);

    // Create a timestamped filename
    std::stringstream ss_timestamp;
    ss_timestamp << img_msg.header.stamp.sec << "-" << std::setw(9) << std::setfill('0')
                 << img_msg.header.stamp.nanosec;
    std::string timestamp_str = ss_timestamp.str();

    // Determine file extension based on encoding
    std::string extension = ".png";  // Default to PNG

    // Create the full file path
    std::string filepath = topic_dir_ + "/" + timestamp_str + extension;

    // Save data for later
    data_meta_vec_.push_back(DataMeta{filepath, img_msg.header.stamp, global_id});
    data_vec_.push_back(img_msg);
  }

  bool save_msg_to_file(size_t index) override
  {
    // Check index bounds
    if (index >= data_meta_vec_.size() || index >= data_vec_.size()) {
      RCLCPP_ERROR(logger_, "[ImageHandler] Provided index is out of range");
      return false;
    }

    DataMeta & data_meta = data_meta_vec_[index];
    sensor_msgs::msg::Image & img_msg = data_vec_[index];

    // Convert the sensor message to a cv::Mat image using cv_bridge
    cv_bridge::CvImagePtr cv_ptr;
    try {
      cv_ptr = cv_bridge::toCvCopy(img_msg, encoding_);
    } catch (const cv_bridge::Exception & e) {
      throw std::runtime_error(
        "CV Bridge failed to convert to the requested encoding: " + encoding_);
    }
    // cv_bridge can correctly convert 'bayer_rggb8' or 'bayer_bggr8' to BGR8 or Mono8.
    // If "rgb8" is needed, an extra conversion is required since cv_bridge doesn't handle it
    // directly
    if (encoding_ == "rgb8") {
      cv::cvtColor(cv_ptr->image, cv_ptr->image, cv::COLOR_RGB2BGR);
    }

    // Write the image to disk
    if (!cv::imwrite(data_meta.data_path, cv_ptr->image)) {
      RCLCPP_WARN(logger_, "Failed to write image to %s", data_meta.data_path.c_str());
      return false;
    } else {
      RCLCPP_DEBUG(logger_, "Successfully wrote image to %s", data_meta.data_path.c_str());
      return true;
    }
  }

private:
  std::string topic_dir_;
  std::string encoding_;
  std::vector<sensor_msgs::msg::Image> data_vec_;
};

}  // namespace rosbag2_exporter

#endif  // ROSBAG2_EXPORTER__HANDLERS__IMAGE_HANDLER_HPP_
