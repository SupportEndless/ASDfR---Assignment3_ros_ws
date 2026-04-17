#ifndef XRF2_SRT_unit_test_hpp
#define XRF2_SRT_unit_test_hpp

// Common includes
#include <cmath>
#include <chrono>

// Custom includes
#include "rclcpp/rclcpp.hpp"
#include <std_msgs/msg/float64.hpp>
#include "example_interfaces/msg/float64.hpp"

class XRF2_SRT_unit_test : public rclcpp::Node
{
    public:
        XRF2_SRT_unit_test();
    private:
        rclcpp::Publisher<example_interfaces::msg::Float64>::SharedPtr right_message_publisher_;
        rclcpp::Publisher<example_interfaces::msg::Float64>::SharedPtr left_message_publisher_;
        rclcpp::TimerBase::SharedPtr timer_;   

        // Timer
        double publish_rate_;
        double sine_freq_ = 0.2;
        double zero_time_;
        std::string setting_;
        double amp_;

        // Functions
        void initialize_();
        void set_params_();
        void timer_callback_();
        double get_motor_inputs_(double);
        
        // Unit test functions
        double constant_input_();
        double sine_wave_(double);
        double step_(double);
};

#endif