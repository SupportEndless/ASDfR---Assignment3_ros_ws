#include "setpoint_generator.hpp"

// Constructor definition
SetpointGenerator::SetpointGenerator() : Node("setpoint_generator"){
    // Declaring and setting parameters for the ros parameters
    this->initialize_ros_parameters();

    // Setting up publishers for both right and left wheels
    right_wheel_publisher_ = 
        this->create_publisher<example_interfaces::msg::Float64>("input/right_motor/setpoint_vel", 1);
    left_wheel_publisher_ = 
        this->create_publisher<example_interfaces::msg::Float64>("input/left_motor/setpoint_vel", 1);

    // Setting up subscriber way if that is what we want from parameter definition    
    if (this->use_subscriber_) {
        // Setting up subscriber to robot position, so we can use it to time reference signal, and track error
        this->pose_subscriber_ = 
        this->create_subscription<geometry_msgs::msg::PoseStamped>(
            "/output/robot_pose", 
            1,
            std::bind(&SetpointGenerator::topic_callback, this, _1));
    }
    else if (this->do_brightness_tracking_) {
        // See function for information
        this->initialize_brightness_tracking();
    }
    else {
        // Otherwise, use normal timer
        this->timer_ = this->create_wall_timer(
            std::chrono::duration<double>(this->publish_rate_),
            std::bind(&SetpointGenerator::timer_callback, this)
        );
    }
}

void SetpointGenerator::initialize_ros_parameters() {
    this->declare_parameter("omega_right_wheel", 1.0);
    this->declare_parameter("omega_left_wheel", 1.0);
    this->declare_parameter("left_wheel_offset", 0.0);
    this->declare_parameter("right_velocity_amplitude", 0.6);
    this->declare_parameter("left_velocity_amplitude", 0.6);
    this->declare_parameter("publish_rate", 0.01);
    this->declare_parameter("use_subscriber", false);
    this->declare_parameter("do_brightness_tracking", false);
    this->declare_parameter("proportional_gain", 1.0);

    this->omega_right_wheel_ = this->get_parameter("omega_right_wheel").as_double();
    this->omega_left_wheel_ = this->get_parameter("omega_left_wheel").as_double();
    this->left_wheel_offset_ = this->get_parameter("left_wheel_offset").as_double();     // radians
    this->right_velocity_amplitude_ = this->get_parameter("right_velocity_amplitude").as_double();
    this->left_velocity_amplitude_ = this->get_parameter("left_velocity_amplitude").as_double();
    this->publish_rate_ = this->get_parameter("publish_rate").as_double();
    this->use_subscriber_ = this->get_parameter("use_subscriber").as_bool();
    this->do_brightness_tracking_ = this->get_parameter("do_brightness_tracking").as_bool();
    this->proportional_gain_ = this->get_parameter("proportional_gain").as_double();
}

void SetpointGenerator::initialize_brightness_tracking() {
    // Using a subscriber to the camera output, to see CoG
    // Also use subscriber to the pose, so we can generate error signal
    this->camera_position_subscriber_ = 
        this->create_subscription<geometry_msgs::msg::PointStamped>(
            "/output/camera_position",
            1,
            std::bind(&SetpointGenerator::set_looking_at, this, _1));

    // Creating subscriber for CoG value, which will be considered reference
    this->CoG_subscriber_ = 
        this->create_subscription<geometry_msgs::msg::Pose>(
            "CoG",
            1,
            std::bind(&SetpointGenerator::set_CoG_reference, this, _1));

    // Publisher uses own publish function, and a wall timer, since publishing on every reception seems excessive
    this->timer_ = 
        this->create_wall_timer(
            std::chrono::duration<double>(this->publish_rate_),
            std::bind(&SetpointGenerator::brightness_tracking_timer_callback, this));  
}

double SetpointGenerator::get_right_wheel_control(double t) const {
    return this->right_velocity_amplitude_ * std::sin(this->omega_right_wheel_ * t);
}

double SetpointGenerator::get_left_wheel_control(double t) const {
    return this->left_velocity_amplitude_ * std::sin(this->omega_left_wheel_ * t + this->left_wheel_offset_);
}

void SetpointGenerator::topic_callback(geometry_msgs::msg::PoseStamped::SharedPtr msg) {    
    // Initializing messages
    auto right_message = example_interfaces::msg::Float64();
    auto left_message = example_interfaces::msg::Float64();
    
    // Setting message
    double time = msg->header.stamp.sec + (msg->header.stamp.nanosec * 1e-9);
    right_message.data = get_right_wheel_control(time);
    left_message.data = get_left_wheel_control(time);

    // Sending message
    this->right_wheel_publisher_->publish(right_message);
    this->left_wheel_publisher_->publish(left_message);
}

void SetpointGenerator::timer_callback() {
    // Initializing messages
    auto right_message = example_interfaces::msg::Float64();
    auto left_message = example_interfaces::msg::Float64();
    
    // Setting message
    double time = this->get_clock()->now().seconds();
    right_message.data = get_right_wheel_control(time);
    left_message.data = get_left_wheel_control(time);

    // Sending message
    this->right_wheel_publisher_->publish(right_message);
    this->left_wheel_publisher_->publish(left_message);  
}

void SetpointGenerator::set_CoG_reference(geometry_msgs::msg::Pose::SharedPtr msg) {
    this->x_camera_reference_ = msg->position.x;
}

void SetpointGenerator::set_looking_at(geometry_msgs::msg::PointStamped::SharedPtr msg) {
    this->x_camera_current_ = msg->point.x;
}

void SetpointGenerator::brightness_tracking_timer_callback() {
    // Firstly, must calculate error signal
    double e = this->x_camera_reference_ - this->x_camera_current_;
    double control_output = this->proportional_gain_*e;

    // Initialize messages
    auto right_message = example_interfaces::msg::Float64();
    auto left_message = example_interfaces::msg::Float64();

    // Setting message data as controller outputs, which in this case is just proportional to error
    right_message.data = -control_output;           // Rotating to the right from positive error, so this should be negative
    left_message.data = -control_output;             // Other direction, so we only rotate

    // Sending message
    this->right_wheel_publisher_->publish(right_message);
    this->left_wheel_publisher_->publish(left_message);
}

// As taken from tutorial on ros2 website
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SetpointGenerator>());
  rclcpp::shutdown();
  return 0;
}