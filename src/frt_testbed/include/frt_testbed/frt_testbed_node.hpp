#ifndef FRT_TESTBED__FRT_TESTBED_NODE_HPP_
#define FRT_TESTBED__FRT_TESTBED_NODE_HPP_

#include <chrono>
#include <fstream>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include "xrf2_msgs/msg/ros2_xeno.hpp"
#include "xrf2_msgs/msg/xeno2_ros.hpp"

namespace frt_testbed
{

/**
 * @brief SRT test-bed node for the XRF2 FRT pass-through.
 *
 * Publishes parameter-selectable test patterns on Ros2Xeno, drives the XRF2
 * FullStateMachine over XenoCmd (Int32), and latches feedback from Xeno2Ros
 * and XenoState. Optionally logs a CSV of (t, cmdL, cmdR, fbL, fbR, state).
 */
class FrtTestbedNode : public rclcpp::Node
{
public:
  /// XRF2 command enum (must match XenoRosCommandBridge::COMMANDS).
  enum XenoCommand : int32_t
  {
    INITIALISE_CMD = 1,
    START_CMD      = 2,
    STOP_CMD       = 3,
    RESET_CMD      = 4,
    PAUSE_CMD      = 5,
    QUIT_CMD       = 6,
  };

  FrtTestbedNode();
  ~FrtTestbedNode() override;

  /// Called from the shutdown hook to stop the FRT and close the log.
  void shutdown_sequence();

private:
  // --- Parameters ---------------------------------------------------------
  std::string pattern_;
  double amplitude_left_{0.0};
  double amplitude_right_{0.0};
  double frequency_hz_{0.5};
  double duration_s_{0.0};         ///< <=0 means run forever
  double publish_rate_hz_{100.0};
  double max_abs_setpoint_{5.0};
  bool   autostart_{false};
  std::string log_path_;

  // --- State --------------------------------------------------------------
  rclcpp::Time start_time_;
  bool stopped_{false};
  int32_t latest_state_{-1};
  double latest_fb_left_{0.0};
  double latest_fb_right_{0.0};
  double last_cmd_left_{0.0};
  double last_cmd_right_{0.0};

  std::ofstream csv_;

  // --- ROS I/O ------------------------------------------------------------
  rclcpp::Publisher<xrf2_msgs::msg::Ros2Xeno>::SharedPtr pub_ros2xeno_;
  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr     pub_xeno_cmd_;
  rclcpp::Subscription<xrf2_msgs::msg::Xeno2Ros>::SharedPtr sub_xeno2ros_;
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr     sub_xeno_state_;
  rclcpp::TimerBase::SharedPtr timer_;

  // --- Helpers ------------------------------------------------------------
  void on_timer();
  void on_xeno2ros(const xrf2_msgs::msg::Xeno2Ros::SharedPtr msg);
  void on_xeno_state(const std_msgs::msg::Int32::SharedPtr msg);
  void compute_pattern(double t, double & out_left, double & out_right) const;
  void send_command(XenoCommand cmd);
  static double clamp(double v, double lim);
};

}  // namespace frt_testbed

#endif  // FRT_TESTBED__FRT_TESTBED_NODE_HPP_
