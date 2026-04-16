#include "XRF2_SRT_drive_test.hpp"

XRF2_SRT_drive_test::XRF2_SRT_drive_test()
    : rclcpp::Node("xrf2_srt_drive_test")
{
    left_message_publisher_ =
        this->create_publisher<std_msgs::msg::Float64>("set_vel_left", 10);
    right_message_publisher_ =
        this->create_publisher<std_msgs::msg::Float64>("set_vel_right", 10);

    initialize_();
}

void XRF2_SRT_drive_test::initialize_()
{
    zero_time_ = this->get_clock()->now().seconds();

    timer_ = this->create_wall_timer(
        std::chrono::duration<double>(PUBLISH_RATE_S),
        std::bind(&XRF2_SRT_drive_test::timer_callback_, this));

    RCLCPP_INFO(this->get_logger(),
                "drive_test up. L=%.3fm d=%.3fm | "
                "settle=%.2fs drive=%.2fs turn=%.3fs (theta=%.1f deg @ %.2f rad/s)",
                WHEELBASE_M, WHEEL_DIAMETER_M,
                T_SETTLE, T_DRIVE, T_TURN,
                TURN_ANGLE_RAD * 180.0 / PI, TURN_SPEED);
}

void XRF2_SRT_drive_test::timer_callback_()
{
    const double t = this->get_clock()->now().seconds() - zero_time_;

    double left  = 0.0;
    double right = 0.0;

    if (t < T_SETTLE_END) {
        // Hold zero while everything settles
        left  = 0.0;
        right = 0.0;
    } else if (t < T_DRIVE_END) {
        // Drive forward
        left  = DRIVE_SPEED;
        right = DRIVE_SPEED;
    } else if (t < T_TURN_END) {
        // Right turn in place
        // Left wheel forward, right wheel reverse
        left  =  TURN_SPEED;
        right = -TURN_SPEED;
    } else {
        // Stop and hold
        left  = 0.0;
        right = 0.0;
    }

    publish_setpoints_(left, right);
}

void XRF2_SRT_drive_test::publish_setpoints_(double left, double right)
{
    std_msgs::msg::Float64 left_msg;
    std_msgs::msg::Float64 right_msg;

    left_msg.data  = left;
    right_msg.data = right;

    left_message_publisher_->publish(left_msg);
    right_message_publisher_->publish(right_msg);
}

// Standard ROS2 node entry point
int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<XRF2_SRT_drive_test>());
    rclcpp::shutdown();
    return 0;
}
