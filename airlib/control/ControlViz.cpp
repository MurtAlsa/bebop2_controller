//
// Created by redwan on 12/14/22.
//

#include "airlib/control/ControlViz.h"

namespace bebop2 {
    ControlViz::ControlViz( ros::NodeHandle &nh) : nh_(nh) {
        pub_marker_ = nh_.advertise<visualization_msgs::Marker>("/bebop2/goal", 10);
        std::vector<std::string>header{"x", "y", "z", "yaw"};
        logger_ = std::make_unique<LoggerCSV>(header, LOGGER_FQ);
    }



    void ControlViz::set_marker_from_pose(const tf::Transform &pose, visualization_msgs::Marker &msg) {
        msg.header.frame_id = "map";
        msg.ns = "target";
        msg.header.stamp = ros::Time::now();
        msg.action = visualization_msgs::Marker::ADD;


        msg.scale.x = 0.35;
        msg.scale.y = 0.35;
        msg.scale.z = 0.35;

        // update position
        double x, y, z, yaw;
        msg.pose.position.x = x = pose.getOrigin().x();
        msg.pose.position.y = y = pose.getOrigin().y();
        msg.pose.position.z = z = pose.getOrigin().z();

        // update orientation
        msg.pose.orientation.x = pose.getRotation().x();
        msg.pose.orientation.y = pose.getRotation().y();
        msg.pose.orientation.z = pose.getRotation().z();
        msg.pose.orientation.w = pose.getRotation().w();

        yaw = tf::getYaw(pose.getRotation());
        //TODO add logger enable flag
        logger_->addRow(std::vector<double>{x, y, z, yaw});

    }

    void ControlViz::set_marker_color(const MARKER_COLOR &color, visualization_msgs::Marker &msg) {
        switch (color) {
            case RED:
            {
                msg.color.a = 0.5;
                msg.color.r = 1;
                break;
            }
            case GREEN:
            {
                msg.color.a = 0.5;
                msg.color.g = 1;
                break;
            }
            case BLUE:
            {
                msg.color.a = 0.5;
                msg.color.b = 1;
                break;
            }
            case YELLOW:
            {
                msg.color.a = 0.5;
                msg.color.r = 1;
                msg.color.g = 1;
                break;
            }
            case CYAN:
            {
                msg.color.a = 0.5;
                msg.color.b = 1;
                msg.color.g = 1;
                break;
            }
            case GRAY:
            {
                msg.color.a = 0.5;
                msg.color.r = 0.67;
                msg.color.b = 0.67;
                msg.color.g = 0.67;
                break;
            }
        }

    }

    void ControlViz::update(const tf::Transform &pose, const MARKER_COLOR& color) {
        visualization_msgs::Marker msg;
        set_marker_from_pose(pose, msg);
        set_marker_color(color, msg);
        msg.type = visualization_msgs::Marker::SPHERE;

        pub_marker_.publish(msg);

    }

    void ControlViz::setDrone(const tf::Transform &pose) {

        visualization_msgs::Marker msg;
        set_marker_from_pose(pose, msg);
        msg.ns = "drone";
        msg.type = visualization_msgs::Marker::MESH_RESOURCE;
        msg.mesh_resource = "package://bebop2_controller/config/bebop.dae";
//        msg.mesh_use_embedded_materials = true;
        msg.scale.x = msg.scale.y = msg.scale.z = 0.001;

        set_marker_color(GRAY, msg);
        msg.color.a = 1.0;
        msg.id = 101;

        pub_marker_.publish(msg);

    }
} // bebop2