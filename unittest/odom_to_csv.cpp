//
// Created by redwan on 2/24/23.
//
#include "ros/ros.h"
#include "airlib/localization/Sensors/ApriltagLandmarks.h"
#include "airlib/utility/LoggerCSV.h"
#include "airlib/localization/Sensors/ApriltagLandmarksExtended.h"

#include <thread>

class DecodeModule{
public:
    DecodeModule()
    {
        aprilTag_ = new ApriltagLandmarksExtended(nh_);
        logger_ = new LoggerCSV({"x", "y", "z", "yaw"});

        const char *loggerOut = "/home/redwan/catkin_ws/src/bebop2_controller/unittest/new";
        logger_->setOutputFolder(loggerOut);

        /// start timer
        timer_ = nh_.createTimer(ros::Duration(1/100), &DecodeModule::checkUpdate, this);
        msg_counter_ = 0;

    }
    ~DecodeModule()
    {
        delete aprilTag_;
//        delete logger_;
    }

    void checkUpdate(const ros::TimerEvent& event) {
        if (!aprilTag_->empty()) {
            std::vector<double> rawState, state;
            aprilTag_->operator()(state);
            ROS_INFO("[state %d ] %lf %lf %lf %lf", ++msg_counter_, state[0], state[1], state[2], state[3]);
            logger_->addRow(std::vector<double>{state[0], state[1], state[2], state[3]});
        }

        if(msg_counter_ >= 4100) // default 4148
        {
            ROS_INFO_STREAM("[DecodeModule] is shutting down");
            delete logger_;
            timer_.stop();
        }


    }

private:
    ros::NodeHandle nh_;
    ros::Timer timer_;
    ApriltagLandmarksExtended *aprilTag_;
    LoggerCSV *logger_;
    int msg_counter_;
};




int main(int argc, char* argv[])
{
    ros::init(argc, argv, "odom_to_csv");
    ros::NodeHandle nh;
    DecodeModule aprilTagDecoder;

    ros::MultiThreadedSpinner spinner(4);
    spinner.spin();

    return 0;
}