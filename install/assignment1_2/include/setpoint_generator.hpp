#ifndef ASSIGNMENT1_2_SETPOINT_GENERATOR
#define ASSIGNMENT1_2_SETPOINT_GENERATOR

// Normal packages
#include <chrono>
#include <cmath>

// ROS2 specific packages
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "geometry_msgs/msg/point_stamped.hpp"
#include "example_interfaces/msg/float64.hpp"
#include "rclcpp/rclcpp.hpp"

// For chrono so we can use time stamps and placeholders for bind
using namespace std::chrono_literals;
using std::placeholders::_1;

class SetpointGenerator : public rclcpp::Node
{
    public:
        SetpointGenerator();
    private:
        // NOT TO BE CONFUSED WITH ANGULAR VELOCITY! THIS IS FOR SETPOINT GENERATION FREQUENCY!
        double omega_right_wheel_;          // Angular frequency of right wheel
        double omega_left_wheel_;           // Angular frequency of left wheel
        double left_wheel_offset_;          // Phase shift of left wheel compared to right, for graphing purpose
        double right_velocity_amplitude_;   // Sets amplitude of velocity on right wheel
        double left_velocity_amplitude_;    // ""  "" but on left wheel
        double publish_rate_;
        bool use_subscriber_;
        
        // Specific for brightness tracking
        bool do_brightness_tracking_;
        double error_;                      // The error value for the brightness tracking controller
        const int IMAGE_WIDTH_ = 300;        // In pixels, but is same as coords
        const int IMEGA_HEIGHT_ = 200;       // Also in pixels, same logic
        double proportional_gain_;
        double x_camera_reference_;
        double x_camera_current_;

        // Setting up publisher and subscriber
        rclcpp::Publisher<example_interfaces::msg::Float64>::SharedPtr right_wheel_publisher_;
        rclcpp::Publisher<example_interfaces::msg::Float64>::SharedPtr left_wheel_publisher_; 
        rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr pose_subscriber_;                  // Subscriber setup for subscription based publisher
        rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr CoG_subscriber_;                          // Subscriber for CoG
        rclcpp::Subscription<geometry_msgs::msg::PointStamped>::SharedPtr camera_position_subscriber_;      // Subscriber for current camera position
        rclcpp::TimerBase::SharedPtr timer_;                                                                // Timer used in create_wall_timer() for multiple functions

        // === FUNCTIONS === // 
        void initialize_ros_parameters();
        void initialize_brightness_tracking();
        double get_right_wheel_control(double) const;
        double get_left_wheel_control(double) const;
        void topic_callback(geometry_msgs::msg::PoseStamped::SharedPtr msg);     // Sends out control input to both left and right wheel, depending on parameters
        void timer_callback();          // For using time

        // Specific for brightness tracking
        void set_looking_at(geometry_msgs::msg::PointStamped::SharedPtr msg);
        void set_CoG_reference(geometry_msgs::msg::Pose::SharedPtr msg);
        void brightness_tracking_timer_callback();
        
};

#endif