#ifndef XRF2_SRT_unit_test
#define XRF2_SRT_unit_test

// Common includes
#include <cmath>

// Custom includes
#include "rclcpp/rclcpp.hpp"
#include <std_msgs/msg/float64.hpp>

class XRF2_SRT_unit_test : rclcpp.Node()
{
    public:
    
    private:
        rclcpp::Publisher<std_msgs::msg::float64>::SharedPtr right_wheel_velocity_publisher_;
        rclcpp::Publisher<std_msgs::msg::float64>::SharedPtr left_wheel_velocity_publisher_;

        
};

#endif