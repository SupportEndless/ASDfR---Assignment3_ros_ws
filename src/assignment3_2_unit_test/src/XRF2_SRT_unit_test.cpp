#include "XRF2_SRT_unit_test.hpp"

XRF2_SRT_unit_test::XRF2_SRT_unit_test() : Node("XRF2_SRT_unit_test") {
    this->initialize_();
}

void XRF2_SRT_unit_test::initialize_() {
    this-> zero_time_ = this->get_clock()->now().seconds();

    // Set up walltimer
    this->timer_ = this->create_wall_timer(
        std::chrono::duration<double>(this->publish_rate_),
        std::bind(&XRF2_SRT_unit_test::timer_callback_, this));
    

    // Setting up pulishers
    this->right_message_publisher_ = this->create_publisher<example_interfaces::msg::Float64>("output/right_motor/setpoint_vel", 1);
    this->left_message_publisher_ = this->create_publisher<example_interfaces::msg::Float64>("output/left_motor/setpoint_vel", 1);

    // Setting parameters
    this->set_params_();
}

void XRF2_SRT_unit_test::set_params_() {
    this->declare_parameter("setting", static_cast<std::string>("sine"));
    this->declare_parameter("amp", static_cast<double>(2.0));
    this->declare_parameter("publish_rate", static_cast<double>(0.01));

    this->setting_ = this->get_parameter("setting").as_string();
    this->amp_ = this->get_parameter("amp").as_double();
    this->publish_rate_ = this->get_parameter("publish_rate").as_double();
}

void XRF2_SRT_unit_test::timer_callback_() {
    // Getting current time and motor inputs
    double now = this->get_clock()->now().seconds();
    double motor_input = this->get_motor_inputs_(now);
    
    // Publishing
    example_interfaces::msg::Float64 right_message = example_interfaces::msg::Float64();
    example_interfaces::msg::Float64 left_message = example_interfaces::msg::Float64();
    
    right_message.data = motor_input;
    left_message.data = motor_input;
    
    this->right_message_publisher_->publish(right_message);
    this->left_message_publisher_->publish(left_message);
}

// Looks at test setting, and returns corresponding motor input for said test
double XRF2_SRT_unit_test::get_motor_inputs_(double now) {
    if (this->setting_ == "sine"){ 
        return this->sine_wave_(now - this->zero_time_);
    } else if (this->setting_ == "step") {
        return this->step_(now);
    } else if (this->setting_ == "constant") {
        return this->constant_input_();
    }

    // Throwing an error if no of the possible cases is selected
    std::cerr << "Improper unit test setting. Quitting...";
    return 0.0;
}

// Sine function, simply uses amp and frequency to generate setpoints
double XRF2_SRT_unit_test::sine_wave_(double time) {
    return this->amp_ * std::sin(this->sine_freq_ * time);
}

// Simple step function, returning amp_ after 3 seconds
double XRF2_SRT_unit_test::step_(double now) {
    if (now + 3.0 < this->get_clock()->now().seconds()) {
        return 0.0;
    }
    return this->amp_;
}

double XRF2_SRT_unit_test::constant_input_() {
    return this->amp_;
}

// As taken from tutorial on ros2 website
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<XRF2_SRT_unit_test>());
  rclcpp::shutdown();
  return 0;
}