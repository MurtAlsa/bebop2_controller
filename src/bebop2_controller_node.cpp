//
// Created by redwan on 3/5/23.
//
//
// Created by roboticslab on 12/13/22.
//
#include "ros/ros.h"
#include <iostream>
#include "airlib/control/QuadControllerPID.h"
#include "airlib/localization/Sensors/ApriltagLandmarksExtended.h"
#include "airlib/localization/Filters/RobotLocalization.h"


int main(int argc, char* argv[])
{
    ros::init(argc, argv, "airlib_bebop2");
    ROS_INFO("airlib-bebop2 INITIALIZED!");
    ros::NodeHandle nh;
    double alpha;
    std::string filterType, expType; 
    ros::param::get("~alpha", alpha);
    ros::param::get("~filter", filterType);
    ros::param::get("~exp", expType);


    FilterPtr filter;
    if(filterType == "ekf")
        filter = std::make_shared<bebop2::RobotLocalization>(nh);
    else if (filterType == "lowpass")
        filter = std::make_shared<ComplementaryFilter>(alpha);

    SensorPtr stateSensor;
    if (expType == "sim-dummy")
    {
        const bool WHITE_NOISE = true;
        stateSensor = std::make_shared<bebop2::DummyState>(nh, WHITE_NOISE);
    }
    else if (expType == "bebop-apriltag" || expType == "sim-bag")
        stateSensor = std::make_shared<ApriltagLandmarksExtended>(nh);

    auto stateObserver = std::make_shared<bebop2::StateObserver>(filter, stateSensor);
    bebop2::QuadControllerPID controller(stateObserver, nh);
    ros::AsyncSpinner spinner(4);
    spinner.start();
    ros::waitForShutdown();

    return 0;
}
