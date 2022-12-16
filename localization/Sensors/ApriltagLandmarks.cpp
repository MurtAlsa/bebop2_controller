//
// Created by redwan on 12/15/22.
//

#include "ApriltagLandmarks.h"

ApriltagLandmarks::ApriltagLandmarks() {

    std::vector<int>tagIds;
    ros::param::get("~apriltags", tagIds);
    for(auto tagId : tagIds)
    {
        std::string tag_name = "tag" + std::to_string(tagId);
        std::vector<double> value;
        ros::param::get("~" + tag_name, value);
        tf::Transform tagTransform;
        tagTransform.setOrigin(tf::Vector3(value[0], value[1], value[2]));
        landmarks_[tag_name] = tagTransform;
    }

    apriltagSub_ = nh_.subscribe("tag_detections", 1, &ApriltagLandmarks::apriltag_callback, this);

}



void
ApriltagLandmarks::apriltag_callback(const apriltag_ros::AprilTagDetectionArray_<std::allocator<void>>::ConstPtr &msg) {

    for(auto detection: msg->detections)
    {
        std::string tagName = "tag" + std::to_string(detection.id[0]);
        auto pose = detection.pose.pose.pose;
        tf::Transform tagTransform;
        tagTransform.setOrigin(tf::Vector3(pose.position.x, pose.position.y, pose.position.z));
        tagTransform.setRotation(tf::Quaternion(pose.orientation.x, pose.orientation.y, pose.orientation.z, pose.orientation.w));
        auto position = transformToGlobalFrame(tagTransform, tagName);
        measurements_.push({position.x, position.y, position.z, 0});
    }

}

FieldLocation ApriltagLandmarks::transformToGlobalFrame(const tf::Transform &tagTransform, const std::string &tagName) {
    // https://visp-doc.inria.fr/doxygen/visp-daily/tutorial-bebop2-vs.html
    // https://www.andre-gaschler.com/rotationconverter/

    tf::Transform endEffector;
    endEffector.setOrigin(tf::Vector3(-0.09, 0, 0));
    endEffector.setRotation(tf::Quaternion(0.5, -0.5, 0.5, -0.5));
    tf::Transform baseLink = endEffector * tagTransform;
    auto loc = landmarks_[tagName].getOrigin() - baseLink.getOrigin();
    FieldLocation tagStateObs = FieldLocation{tagName, loc.x(), loc.y(), loc.z()};

    return tagStateObs;
}

void ApriltagLandmarks::publish_tf(const std::vector<double> &state) {
    tf::Transform globalCoord;
    tf::Quaternion q;
    q.setRPY(0, 0, state[3]);
    globalCoord.setOrigin(tf::Vector3(state[0], state[1], state[2]));
    globalCoord.setRotation(q);
    br_.sendTransform(tf::StampedTransform(globalCoord, ros::Time::now(), "map", "robot"));
//        ROS_INFO_STREAM(robot_position_);
}

std::vector<double> ApriltagLandmarks::get_observations() {
    auto obs = measurements_.front();
    measurements_.pop();
    return obs;
}

bool ApriltagLandmarks::empty() {
    return measurements_.empty();
}
