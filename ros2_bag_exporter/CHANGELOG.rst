^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ros2_bag_exporter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Forthcoming
-----------
* Jazzy migration (`#34 <https://github.com/assistive-autonomy/tartan_rosbag_exporter/issues/34>`_)
  * Migrate ROS2 from Humble to Jazzy
  - Also migrate from CycloneDDS to Zenoh
  - Bring ford msg dependencies from source not available in Jazzy
  - Fix minor cv_bridge issue pointing to new include header
  * Remove deprecated mergify configuration
  * Fix pre-commit issues
* Contributors: Alejandro Bordallo Micó

2.0.0 (2025-08-07)
------------------
* Fix camera-lidar timestamp synchronisation (`#25 <https://github.com/ipab-rad/tartan_rosbag_exporter/issues/25>`_)
  - Add `timestamp_offset_ms` parameter for every camera to adjust
  lidar timestamps to correlate correctly with camera data.
  - Update `find_closest_timestamp` function to receive this parameter
  and apply the time offset required for each camera.
* Add support for Nebula HESAI LIDAR points (`#23 <https://github.com/ipab-rad/tartan_rosbag_exporter/issues/23>`_)
  - Define `PointXYZIRCAEDT` point data structure and register it
  with the PCL point cloud library in a new header file.
  - Use `NebulaPoint` as an alias for `PointXYZIRCAEDT`
  - Cast `NebulaPoint` to pcl::PointXYZI before saving
  - Iterate through the points in the original NebulaPoint cloud
  and assign the values to a new point cloud with `pcl::PointXYZI`
  structure.
  - Update `pointcloud_handler.hpp` to use `NebulaPoint` when
  handling point clouds.
  - Allow the user to define a directory for exported data
  - Modify `dev.sh` and `runtime.sh` to include `-o | --output` option to
  specify the export directory.
  - Mount config files when running the runtime Docker
  - Modify `runtime.sh` to mount `ros2_bag_exporter/config` in
  `/opt/ros_ws/config`
  - Update README
  - Provide instructions for running the Docker container and the
  `ros2_bag_exporter bag_exporter` ROS node
* Contributors: Hector Cruz

1.1.0 (2025-07-01)
------------------
* Export the tf message with the most transforms. (`#19 <https://github.com/ipab-rad/tartan_rosbag_exporter/issues/19>`_)
  - Fix extracting the wrong tf_static message if multiple and different
  are present in the bag file. This assumes that the most relevant
  tf_static message is the one with the most transforms. `#17 <https://github.com/ipab-rad/tartan_rosbag_exporter/issues/17>`_
  - Modify `TFHandler` to:
  - Save each `TFMessage` with its associated
  transformation count in a sorted map when `process_message()`
  is called
  - Get the `TFMessage` with the most transforms from the sorted map
  when `save_msg_to_file()` is called
  - Modify `BagExporter` to:
  - Call the TF's handler `save_msg_to_file()` method in `export_bag()`
  if a `TFHandler` was registered
  - Remove old tf extraction logic in `export_bag()`
  - Ensure `sample_interval` is not zero for `TF` in `export_config.yaml`
  to avoid zero division errors.
  - Update README to clarify this behaviour.
* Contributors: Hector Cruz

1.0.0 (2025-06-03)
------------------
* Add multiple rosbag extraction support (`#12 <https://github.com/ipab-rad/tartan_rosbag_exporter/issues/12>`_)
  - Change `rosbag` param to `rosbags_directory` to explicitly
  mention that the input should be a directory
  - Throw an exception if it is not a directory
  - Add `output_directory` ros param to define the exportation
  destination path
  - Add `process_rosbag_directory()` member function to scope the
  multi rosbag exporation logic
  - Name export directories with the recording name first
  - Find all the rosbag files inside the directory and export each file
  individually
  - Keep `global_id` for each data message recording level rather than
  split level.
  - Modify `setup_handlers()`, `export_bag()` and
  `create_metadata_file()` functions so they adapt to a specific
  rosbag file.
  - Add `find_rosbags()`, `get_new_name()` and `get_rosbag_index()`
  utilities functions and add a `utils` namespace.
  - Modify logs to reduce terminal cluttering and provide a better format.
  - Ensure the exporter returns consistent exit codes
  - Removed scattered `return` statements and unnecessary
  `try-catch` blocks to centralise error handling in the
  top-level function.
  - Add `run()` function and move the running logic from
  class constructor to it, so returns can be implemented
  - Make `config_file` a class member and avoid receiving an
  argument when calling `load_configuration`
  - Ensured alignment with CLI conventions:
  return `0` on success, `1` on failure.
  - Rename functions to align better with their purpose
  - Rename `export_bag()` to `extract_data()`
  - Rename `create_metadata_file()` to `export_data()`
  - Update README
* Fix incorrect logic in bag exporter (`#10 <https://github.com/ipab-rad/tartan_rosbag_exporter/issues/10>`_)
  - Avoid throwing an exception by adding a missing negation `!` in the
  `handlers\_["/tf_static"].handler->save_msg_to_file(0)` condition when
  processing `tf_static` message
  - Ensure that `CameraInfo` topics are evaluated only once by removing
  them from
  the handlers after processing
  - Remove `exported` flag from `Handler` as no longer necessary
* Remove handler format fallbacks (`#9 <https://github.com/ipab-rad/tartan_rosbag_exporter/issues/9>`_)
  - Enforce image extensions and remove  encoding fallbacks
  - Force `.jpg` or `.png` extensions for `CompressedImage`
  - Throw an exception otherwise
  - Remove image encoding fallbacks in `ImageHandler` and throw an
  exception
  if `cv_bridge` fails
  - Ensure only point cloud messages with `intensity` field are supported
  - Throw an exception otherwise
  - Throw exception if messages exportation failed in `BagExporter`
* Add TF and CameraInfo exportation support (`#8 <https://github.com/ipab-rad/tartan_rosbag_exporter/issues/8>`_)
  - Add `tf2_msgs::msg::TFMessage` exportation support (`#1 <https://github.com/ipab-rad/tartan_rosbag_exporter/issues/1>`_)
  - Introduce the `TFHandler` class to handle `tf2_msgs/msg/Transforms`
  type messages
  - Create a YAML structure when `process_message()` is called
  - Save YAML (`transforms.yaml`) into the defined file path with the
  `save_msg_to_file()` function
  - Modify `BagExporter` to:
  - Support `TFHandler` handler initialisation if specified in the config
  YAML
  - Ensure that `/tf_static` data is exported only once
  - Add `sensor_msgs::msg::CameraInfo` exportation support (`#1 <https://github.com/ipab-rad/tartan_rosbag_exporter/issues/1>`_)
  - Introduce the `CameraInfoHandler` class to handle
  `sensor_msgs/msg/CameraInfo` type messages
  - Create a YAML structure when `process_message()` is called
  - Save YAML (`camera_calibration.yaml`) into the defined file path with
  `save_msg_to_file()` function
  - Modify `BagExporter` to:
  - Support `CameraInfo` handler initialisation if specified in the config
  YAML
  - Ensure that camera info data is exported only once
  - Increment the total message count by one only for camera info-type
  topics
  - Avoid using random codes to colourise logs (`#5 <https://github.com/ipab-rad/tartan_rosbag_exporter/issues/5>`_)
  - Define macros with the colour ANSI codes and use them instead
  - Avoid using random codes to log an emoji (`#4 <https://github.com/ipab-rad/tartan_rosbag_exporter/issues/4>`_ )
  -  Paste the emoji directly  in the log instead
* Add github workflows and linter cfgs (`#7 <https://github.com/ipab-rad/tartan_rosbag_exporter/issues/7>`_)
  - Add GitHub workflows and linter configuration files
  - Applied pre-commit linting
  - Applied `clang-format`
  - Applied `ament-cpp-lint`
* Fix wrong config installation path
* Move ros2_bag_exporter pkg to a lower level directory
  - Remove ros2bag_splitter tool
  - Create repository README and update package README
* Contributors: Hector Cruz
