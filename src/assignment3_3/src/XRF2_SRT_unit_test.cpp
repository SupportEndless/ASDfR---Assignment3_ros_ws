#include "XRF2_SRT_unit_test.hpp"

XRF2_SRT_unit_test::XRF2_SRT_unit_test() {
    this->initialize_;
}

void XRF2_SRT_unit_test::initialize_() {
    this-> zero_time_ = this->get_clock().now().seconds();
    
    // Set up walltimer
    this->timer_ = this->create_wall_timer(
        std::chrono::duration<double>(this->publish_rate_),
        std::bind(&XRF2_SRT_unit_test::timer_callback_, this))
    
    return nullptr;
}

void XRF2_SRT_unit_test::timer_callback_() {
    double now = this->get_clock().now().seconds();
    double motor_input = this->sine_wave_(now - this->zero_time);

    example_interfaces::msg::Float64 right_message = example_interfaces::msg::Float64();
    example_interfaces::msg::Float64 left_message = example_interfaces::msg::Float64();

    right_message.data = motor_input;
    left_message.data = motor_input;

    this->right_message_publisher_.publish(right_message);
    this->left_message_publisher_.publish(left_message);
}

const double XRF2_SRT_unit_test::sine_wave_(double time) {
    return std::sin(this->sine_freq_ * time);
}

// As taken from tutorial on ros2 website
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<XRF2_SRT_unit_test>());
  rclcpp::shutdown();
  return 0;
}