#include <chrono>
#include <string>
#include <memory>

#include <cv_bridge/cv_bridge.hpp>
#include <rclcpp/rclcpp.hpp>
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/image.hpp"

using std::placeholders::_1;

class ImageBrightness : public rclcpp::Node
{

    public:
    ImageBrightness() : Node("image_brightness")
    {
        // Setting up publisher for this node, so we can publish brightness
        publisher_ = 
            this->create_publisher<std_msgs::msg::String>("image_brightness", 10);

        // Setting up subscriber for this node using std::bind to bind "this" instance, and set initial using placeholder
        subscription_ = 
            this->create_subscription<sensor_msgs::msg::Image>(
                "image", 
                10, 
                std::bind(&ImageBrightness::topic_callback, this, _1));

        // Setting up the brightness parameter for command line access (standard = 125)
        this->declare_parameter("brightness_threshold", 125);
    }

    private:        
    // Setting up publishing publisher and subscriber
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;

    // Function to get image brightness and return it in string form
    float get_brightness(sensor_msgs::msg::Image::SharedPtr img) {
        
        // Using cv_bridge and cv to do the image processing for improved efficiency
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(img, "bgr8");
        cv::Scalar mean_val = cv::mean(cv_ptr->image);

        // Calculating perceived average brightness here from equation in report
        return (0.114*mean_val[0] + 0.587*mean_val[1] + 0.299*mean_val[2]);
    }

    // ---- topic_callback function ---- //
    // Function for generating confirmation of reception, and sending out processed result immediately when received.
    // Takes in the message from publisher and sends out average brightness to console log
    void topic_callback(sensor_msgs::msg::Image::SharedPtr img) {
        
        // Guarding if for if it is too dark, we send no message
        if (this->get_brightness(img) < this->get_parameter("brightness_threshold").as_double()) {
            return;
        }

        // Setting up sending message every time we receive a message and it is not too dark
        auto message = std_msgs::msg::String();
        message.data = std::to_string(this->get_brightness(img));
        this->publisher_->publish(message);
    }
};

// As taken from tutorial on ros2 website
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ImageBrightness>());
  rclcpp::shutdown();
  return 0;
}