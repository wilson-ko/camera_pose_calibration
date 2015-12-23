#include "camera_pose_calibration.hpp"

#include <opencv2/opencv.hpp>
#include <dr_util/node.hpp>

#include <camera_pose_calibration/Calibrate.h>
#include <camera_pose_calibration/CalibrateTopic.h>
#include <visualization_msgs/Marker.h>

#include <pcl_ros/point_cloud.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <image_transport/image_transport.h>
#include <ros/ros.h>


namespace camera_pose_calibration {

class CameraPoseCalibrationNode : public dr::Node {
public:
	CameraPoseCalibrationNode();

protected:
	/// Image transport object used for publishing images.
	image_transport::ImageTransport image_transport;

	/// Image transport publisher for publishing the detected pattern.
	image_transport::Publisher detected_pattern_publisher;

	/// Listens to transforms from tf to transform calibration from the correct target to the camera.
	tf::TransformListener transform_listener;

	/// Publishes tf transforms.
	tf::TransformBroadcaster transform_broadcaster;

	/// Publishes the original pointcloud.
	ros::Publisher cloud_publisher;

	/// Publishes the expected (generated) pattern.
	ros::Publisher target_cloud_publisher;

	/// Publishes the expected pattern transformed using the found calibration transform.
	ros::Publisher transformed_target_cloud_publisher;

	/// Publishes the detected pattern from the source cloud.
	ros::Publisher source_cloud_publisher;

	/// Publishes the detected pattern but projected on a fitted plane.
	ros::Publisher projected_source_cloud_publisher;

	/// Publishes a marker for showing the plane of the calibration points.
	ros::Publisher calibration_plane_marker_publisher;

	/// Service server for calibration.
	ros::ServiceServer calibrate_server;

	/// Service server for calibration with image and cloud from topics.
	ros::ServiceServer calibrate_server_topic;

	/// Timer to periodically republish TF transforms.
	ros::Timer tf_timer;

	/// The found calibration transformation.
	tf::StampedTransform calibration_transform;

	/// Determines whether to publish the transform over tf.
	bool publish_transform;

	/// Rate with which to publish transforms.
	double publish_rate;

	/// Topic to read image from
	std::string image_topic;

	/// Topic to read point cloud from
	std::string cloud_topic;

	/// Determines if we have calibrated (and thus should publish the transform).
	bool calibrated;

	/// Parses ROS parameters and calls the calibration service.
	void parseInput();

	/// Create a marker for the detected calibration plane.
	visualization_msgs::Marker createCalibrationPlaneMarker(pcl::PointCloud<pcl::PointXYZ>::ConstPtr points, uint8_t pattern_width, uint8_t pattern_height);

	/// Called when the calibrated TF transforms should be republished.
	void onTfTimeout(ros::TimerEvent const &);

	/// Calibrates the camera given the image and point cloud by a ROS topic, and all other information in the request.
	bool onCalibrateTopic(camera_pose_calibration::CalibrateTopic::Request & req, camera_pose_calibration::CalibrateTopic::Response & res);

	/// Calibrates the camera given the information in the request.
	bool onCalibrate(camera_pose_calibration::Calibrate::Request & req, camera_pose_calibration::Calibrate::Response & res);
};

}