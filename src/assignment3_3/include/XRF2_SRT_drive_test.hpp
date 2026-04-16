#ifndef XRF2_SRT_DRIVE_TEST_HPP
#define XRF2_SRT_DRIVE_TEST_HPP

#include <chrono>
#include <cmath>
#include "rclcpp/rclcpp.hpp"
#include <std_msgs/msg/float64.hpp>


class XRF2_SRT_drive_test : public rclcpp::Node
{
public:
    XRF2_SRT_drive_test();

private:
    // NOTE: MEASRUE THE WHEELBASE, NOT MENTIONED IN THE MANUAL
    static constexpr double WHEELBASE_M      = 0.15;   // L  [m]
    static constexpr double WHEEL_DIAMETER_M = 0.101;   // d  [m]
    static constexpr double PI               = M_PI;

    // ---- Test parameters ---------------------------------------------------
    // Publish rate [s] 1 kHz matches the FRT loop
    static constexpr double PUBLISH_RATE_S = 0.001;

    // Wheel speeds [rad/s]
    static constexpr double DRIVE_SPEED = 3.0;
    static constexpr double TURN_SPEED  = 2.0;

    // Desired right-turn angle magnitude [rad]
    static constexpr double TURN_ANGLE_RAD = PI / 2.0;   // 90 deg

    // Phase durations [s]
    static constexpr double T_SETTLE = 1.0;
    static constexpr double T_DRIVE  = 3.0;

    // Derived turn duration: t = theta * L / (omega * d)
    static constexpr double T_TURN =
        (TURN_ANGLE_RAD * WHEELBASE_M) / (TURN_SPEED * WHEEL_DIAMETER_M);

    // Phase boundaries [s from zero_time_]
    static constexpr double T_SETTLE_END = T_SETTLE;
    static constexpr double T_DRIVE_END  = T_SETTLE_END + T_DRIVE;
    static constexpr double T_TURN_END   = T_DRIVE_END  + T_TURN;

    // ROS members
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr left_message_publisher_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr right_message_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

    double zero_time_ = 0.0;

    // Functions
    void initialize_();
    void timer_callback_();
    void publish_setpoints_(double left, double right);
};

#endif  // XRF2_SRT_DRIVE_TEST_HPP
