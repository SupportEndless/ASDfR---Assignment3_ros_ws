#include "frt_testbed/frt_testbed_node.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>

namespace frt_testbed
{

FrtTestbedNode::FrtTestbedNode()
: rclcpp::Node("frt_testbed_node")
{
  // --- Declare & read parameters ---
  pattern_          = declare_parameter<std::string>("pattern", "zero");
  amplitude_left_   = declare_parameter<double>("amplitude_left", 0.0);
  amplitude_right_  = declare_parameter<double>("amplitude_right", 0.0);
  frequency_hz_     = declare_parameter<double>("frequency_hz", 0.5);
  duration_s_       = declare_parameter<double>("duration_s", 0.0);
  publish_rate_hz_  = declare_parameter<double>("publish_rate_hz", 100.0);
  max_abs_setpoint_ = declare_parameter<double>("max_abs_setpoint", 5.0);
  autostart_        = declare_parameter<bool>("autostart", false);
  log_path_         = declare_parameter<std::string>("log_path", "");

  RCLCPP_INFO(get_logger(),
    "frt_testbed: pattern=%s ampL=%.3f ampR=%.3f freq=%.2fHz dur=%.2fs rate=%.0fHz",
    pattern_.c_str(), amplitude_left_, amplitude_right_,
    frequency_hz_, duration_s_, publish_rate_hz_);

  // --- Publishers / subscribers (XRF2 topic names) ---
  pub_ros2xeno_ = create_publisher<xrf2_msgs::msg::Ros2Xeno>("Ros2Xeno", 10);
  pub_xeno_cmd_ = create_publisher<std_msgs::msg::Int32>("XenoCmd", 10);

  sub_xeno2ros_ = create_subscription<xrf2_msgs::msg::Xeno2Ros>(
    "Xeno2Ros", 10,
    std::bind(&FrtTestbedNode::on_xeno2ros, this, std::placeholders::_1));

  sub_xeno_state_ = create_subscription<std_msgs::msg::Int32>(
    "XenoState", 10,
    std::bind(&FrtTestbedNode::on_xeno_state, this, std::placeholders::_1));

  // --- Optional CSV log ---
  if (!log_path_.empty()) {
    csv_.open(log_path_, std::ios::out | std::ios::trunc);
    if (csv_.is_open()) {
      csv_ << "t,cmd_left,cmd_right,fb_left,fb_right,state\n";
      RCLCPP_INFO(get_logger(), "logging to %s", log_path_.c_str());
    } else {
      RCLCPP_WARN(get_logger(), "could not open log file %s", log_path_.c_str());
    }
  }

  // --- Autostart ---
  if (autostart_) {
    // Give the bridge a moment to connect before firing commands.
    using namespace std::chrono_literals;
    rclcpp::sleep_for(200ms);
    send_command(INITIALISE_CMD);
    rclcpp::sleep_for(200ms);
    send_command(START_CMD);
  }

  // --- Main timer ---
  start_time_ = now();
  const auto period = std::chrono::duration<double>(1.0 / publish_rate_hz_);
  timer_ = create_wall_timer(
    std::chrono::duration_cast<std::chrono::nanoseconds>(period),
    std::bind(&FrtTestbedNode::on_timer, this));
}

FrtTestbedNode::~FrtTestbedNode()
{
  if (csv_.is_open()) csv_.close();
}

double FrtTestbedNode::clamp(double v, double lim)
{
  if (v >  lim) return  lim;
  if (v < -lim) return -lim;
  return v;
}

void FrtTestbedNode::send_command(XenoCommand cmd)
{
  std_msgs::msg::Int32 m;
  m.data = static_cast<int32_t>(cmd);
  pub_xeno_cmd_->publish(m);
  RCLCPP_INFO(get_logger(), "XenoCmd -> %d", m.data);
}

void FrtTestbedNode::compute_pattern(double t, double & out_left, double & out_right) const
{
  const double w = 2.0 * M_PI * frequency_hz_;

  if (pattern_ == "zero") {
    out_left  = 0.0;
    out_right = 0.0;
  } else if (pattern_ == "step") {
    out_left  = amplitude_left_;
    out_right = amplitude_right_;
  } else if (pattern_ == "ramp") {
    // Linear ramp from 0 to amplitude over frequency_hz_^-1 seconds, then hold.
    const double ramp_dur = (frequency_hz_ > 0.0) ? (1.0 / frequency_hz_) : 1.0;
    const double k = std::min(1.0, t / ramp_dur);
    out_left  = k * amplitude_left_;
    out_right = k * amplitude_right_;
  } else if (pattern_ == "sine") {
    out_left  = amplitude_left_  * std::sin(w * t);
    out_right = amplitude_right_ * std::sin(w * t);
  } else if (pattern_ == "square") {
    const double s = (std::sin(w * t) >= 0.0) ? 1.0 : -1.0;
    out_left  = amplitude_left_  * s;
    out_right = amplitude_right_ * s;
  } else if (pattern_ == "differential") {
    out_left  =  amplitude_left_;
    out_right = -amplitude_right_;
  } else {
    out_left  = 0.0;
    out_right = 0.0;
  }
}

void FrtTestbedNode::on_timer()
{
  if (stopped_) return;

  const double t = (now() - start_time_).seconds();

  if (duration_s_ > 0.0 && t >= duration_s_) {
    RCLCPP_INFO(get_logger(), "duration reached (%.2fs), stopping", t);
    shutdown_sequence();
    return;
  }

  double cmd_l, cmd_r;
  compute_pattern(t, cmd_l, cmd_r);

  cmd_l = clamp(cmd_l, max_abs_setpoint_);
  cmd_r = clamp(cmd_r, max_abs_setpoint_);

  xrf2_msgs::msg::Ros2Xeno m;
  m.left_wheel_vel  = cmd_l;
  m.right_wheel_vel = cmd_r;
  pub_ros2xeno_->publish(m);

  last_cmd_left_  = cmd_l;
  last_cmd_right_ = cmd_r;

  if (csv_.is_open()) {
    csv_ << std::fixed << std::setprecision(6)
         << t << ','
         << cmd_l << ',' << cmd_r << ','
         << latest_fb_left_ << ',' << latest_fb_right_ << ','
         << latest_state_ << '\n';
  }
}

void FrtTestbedNode::on_xeno2ros(const xrf2_msgs::msg::Xeno2Ros::SharedPtr msg)
{
  // Xeno2Ros has generic fields example_c / example_d; we use them as
  // (encoder_left, encoder_right) by convention with XRF2_FRT_test.
  latest_fb_left_  = msg->example_c;
  latest_fb_right_ = msg->example_d;
}

void FrtTestbedNode::on_xeno_state(const std_msgs::msg::Int32::SharedPtr msg)
{
  if (msg->data != latest_state_) {
    RCLCPP_INFO(get_logger(), "XenoState: %d -> %d", latest_state_, msg->data);
  }
  latest_state_ = msg->data;
}

void FrtTestbedNode::shutdown_sequence()
{
  if (stopped_) return;
  stopped_ = true;

  // Publish a few zero setpoints so the FRT leaves motors at rest.
  for (int i = 0; i < 5; ++i) {
    xrf2_msgs::msg::Ros2Xeno m;
    m.left_wheel_vel  = 0.0;
    m.right_wheel_vel = 0.0;
    pub_ros2xeno_->publish(m);
    rclcpp::sleep_for(std::chrono::milliseconds(10));
  }

  send_command(STOP_CMD);
  rclcpp::sleep_for(std::chrono::milliseconds(100));
  send_command(QUIT_CMD);

  if (csv_.is_open()) csv_.close();
  if (timer_) timer_->cancel();
}

}  // namespace frt_testbed

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<frt_testbed::FrtTestbedNode>();
  rclcpp::on_shutdown([node]() { node->shutdown_sequence(); });
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
