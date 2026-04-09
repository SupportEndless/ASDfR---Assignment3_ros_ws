#ifndef XRF2_SRT_unit_test
#define XRF2_SRT_unit_test

// Common includes
#include <cmath>
#include <chrono>

// Custom includes
#include "rclcpp/rclcpp.hpp"
#include <std_msgs/msg/float64.hpp>

class XRF2_SRT_unit_test : rclcpp.Node()
{
    public:
    
    private:
        rclcpp::Publisher<std_msgs::msg::float64>::SharedPtr right_message_publisher_;
        rclcpp::Publisher<std_msgs::msg::float64>::SharedPtr left_message_publisher_;

        // Timer
        double pusblish_rate_;
        double sine_freq_ = 0.2;
        double zero_time_;
        double sine_amp_ = 2;

        // Functions
        void initialize_();
        void timer_callback_();
        const void constant_input_();
        const double sine_wave_(double);
        const void step_();
};

#endif