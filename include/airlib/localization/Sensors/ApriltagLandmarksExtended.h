//
// Created by redwan on 3/3/23.
//

#ifndef BEBOP2_CONTROLLER_APRILTAGLANDMARKSEXTENDED_H
#define BEBOP2_CONTROLLER_APRILTAGLANDMARKSEXTENDED_H
#include <memory>
#include <unordered_map>
#include <ros/ros.h>
#include <tf/transform_listener.h>
#include "airlib/robot_defs.h"
#include <apriltag_ros/AprilTagDetectionArray.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <nav_msgs/Odometry.h>
#include <mutex>
#include <queue>
#include "SensorBase.h"
/**
 * @brief This class compute the robot coordinate from stationary tags.
 *
 * There are three stationary apriltags located on the wall.
 * These tags are treated as known landmarks for the map.
 * Bebop2 can view these tags with the front facing camera.
 * It is not necessary to see all the tags even one landmark can help to localize the robot.
 * This information is feedback to the computer to detect apriltags from the camera image.
 * Bebop2 can transmit this images upto 28 Hz speed
 * Apprantly, the tags are located only front direction, therefore robot yaw angle is ignored.
 *
 */

class ApriltagLandmarksExtended : public SensorBase{

public:
    ApriltagLandmarksExtended(ros::NodeHandle& nh);
    void operator()(std::vector<double>& result);
    bool empty();

private:
    /// @brief Known landmarks with respect to map
    std::unordered_map<std::string, tf::Transform> landmarks_;
    /// @brief apriltag node subscriber
    ros::Subscriber apriltagSub_;
    /// @brief ros NodeHandle for dealing with various messages
    ros::NodeHandle nh_;
    /// @brief measurements_ queue is reponsible to efficiently communicate with Control Module
    std::queue<std::vector<double>> measurements_;

    /// @brief tagRoations_
    std::vector<double> tagRoations_;
    ///@brief number of initialization required
    std::unordered_map<std::string, std::once_flag> tagInits_;
    ///@brief filter out some init frames
    int filterCount_;

protected:
    /**
     * @brief given a array of detected tags, here we calculate robot global coordinate with respect to each tag.
     * Each tag propose a noisy state of the robot when transformed it to the global frame.
     * Therefore, an appropriate state filter is used to denoise those readings and compute the robot state accurately.
     *
     * @param msg an array of detected tags
     */
    void apriltag_callback(const apriltag_ros::AprilTagDetectionArray::ConstPtr& msg);
    /// @brief tag is detected with respect to camera frame which needs to be transformed to global frame through
    /// camera baselink -> global_frame
    /// @param tagTransform tag pose with resect to camera frame
    /// @param tagName name of the tag
    /// @return global coordinate [x, y, z]


};



#endif //BEBOP2_CONTROLLER_APRILTAGLANDMARKSEXTENDED_H
