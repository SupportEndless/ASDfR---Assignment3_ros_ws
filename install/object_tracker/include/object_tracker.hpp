// TODO: Add top of code info section
#ifndef OBJECT_TRACKER_HPP
#define OBJECT_TRACKER_HPP

// Normal packages
#include <cv_bridge/cv_bridge.hpp>

// ROS2 specific packages
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/point_stamped.hpp>
#include "sensor_msgs/msg/image.hpp"
#include "rclcpp/rclcpp.hpp"

using std::placeholders::_1;

class ObjectTracker : public rclcpp::Node
{
    public:
        ObjectTracker();

    private:
        // Private veriables
        double brightness_threshold_;
        
        // Setting up publisher and subscriber
        rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_subscriber_;
        rclcpp::Publisher<geometry_msgs::msg::Point>::SharedPtr brightness_publisher_;

        // Functions
        void initialize_ros_parameters();                                                   // Sets ROS parrameters
        geometry_msgs::msg::Point calculate_CoG(sensor_msgs::msg::Image::SharedPtr img);    // Calculates CoG based on received image, unpacks Image pointer as well
        void object_tracking_callback(sensor_msgs::msg::Image::SharedPtr img);                                                    // Callback on imagae stream
};

#endif