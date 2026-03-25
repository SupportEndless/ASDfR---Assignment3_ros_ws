#include <string>
#include <memory>

#include <rclcpp/rclcpp.hpp>
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;

class BrightnessReceiver : public rclcpp::Node
{

    public:
    BrightnessReceiver() : Node("brightness_receiver")
    {
        // Setting up subscriber for this node using std::bind to bind "this" instance, and set initial using placeholder
        subscription_ = 
            this->create_subscription<std_msgs::msg::String>(
                "image_brightness", 
                10, 
                std::bind(&BrightnessReceiver::topic_callback, this, _1));
    }

    private:        
    // Setting up subscription
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;

    // ---- topic_callback function ---- //
    // Function for generating confirmation of reception
    void topic_callback(std_msgs::msg::String::SharedPtr msg) {
        // Showing message was received
        RCLCPP_INFO(this->get_logger(), "Received a brightness of '%s'", msg->data.c_str());
    }
};

// As taken from tutorial on ros2 website
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<BrightnessReceiver>());
  rclcpp::shutdown();
  return 0;
}