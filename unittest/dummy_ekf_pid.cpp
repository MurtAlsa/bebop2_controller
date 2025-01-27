//
// Created by roboticslab on 12/13/22.
//
#include "ros/ros.h"
#include <iostream>
#include "airlib/control/QuadControllerPID.h"



int main(int argc, char* argv[])
{
    ros::init(argc, argv, "airlib_bebop2");
    ROS_INFO("BEBOP2 CONTROLLER INITIALIZED!");
    ros::NodeHandle nh;
    double alpha;
    ros::param::get("~alpha", alpha);

    const int STATE_DIM = 4;
    const double DT = 0.03;

    /*
     * Sigmas - just an estimate, usually comes from uncertainty of sensor, but
     * if you used fused data from multiple sensors, it's difficult to find
     * these uncertainties directly.
     */
    std::vector<double> sigma_pos{0.015, 0.015, 0.015, 0.01}; // GPS measurement uncertainty [x [m], y [m], z [m], theta [rad]]
    auto stateFilter = std::make_shared<bebop2::ExtendedKalmanFilter>(sigma_pos, DT, STATE_DIM);
    // use cmd_vel to update state
    auto cmd_sub = nh.subscribe("cmd_vel", 1, &bebop2::ExtendedKalmanFilter::update_cmd, stateFilter.get());

    const bool WHITE_NOISE = true;
    auto stateSensor = std::make_shared<bebop2::DummyState>(nh, WHITE_NOISE);
    auto stateObserver = std::make_shared<bebop2::StateObserver>(stateFilter, stateSensor);

    bebop2::QuadControllerPID controller(stateObserver, nh);
    const int THREAD_COUNT = 4;

    ros::AsyncSpinner spinner(THREAD_COUNT);
    spinner.start();
    ros::waitForShutdown();

//    ros::MultiThreadedSpinner spinner(THREAD_COUNT);
//    spinner.spin();
    return 0;
}