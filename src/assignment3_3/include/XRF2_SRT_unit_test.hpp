#ifndef XRF2_SRT_unit_test
#define XRF2_SRT_unit_test

// Common includes
#include <cmath>
#include <chrono>

// Custom includes
#include "rclcpp/rclcpp.hpp"
#include <std_msgs/msg/float64.hpp>

class XRF2_SRT_unit_test : public rclcpp::Node
{
    public:
    
    private:
        rclcpp::Publisher<std_msgs::msg::float64>::SharedPtr right_message_publisher_;
        rclcpp::Publisher<std_msgs::msg::float64>::SharedPtr left_message_publisher_;

        // Timer
        double pusblish_rate_;
        double sine_freq_ = 0.2;
        double zero_time_;
        std::string setting_;
        double amp_;

        // Functions
        void initialize_();
        void set_params_();
        void timer_callback_();
        const void get_motor_inputs_(double);
        
        // Unit test functions
        const void constant_input_();
        const double sine_wave_(double);
        const double step_();
};

#endif