#include <memory>
#include <stack>
#include <array>
#include <cv_bridge/cv_bridge.h>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/string.hpp"

using std::placeholders::_1;

class ObjectPositionIndicator : public rclcpp::Node
{
    public:
    ObjectPositionIndicator() : Node("object_position_indicator") 
    {
        // Setting up publisher with Pose built-in
        publisher_ = this->create_publisher<geometry_msgs::msg::Pose>("CoG", 10);

        // Setting up subscriber
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            "image",
            10,
            std::bind(&ObjectPositionIndicator::topic_callback, this, _1)
        );

        // Setting up the brightness parameter for command line access (standard = 125.0)
        this->declare_parameter("brightness_threshold", 125.0);
        this->threshold_ = this->get_parameter("brightness_threshold").as_double();  // Confirming into object variable

        // ---- THIS BLOCK BASICALLY FROM ROS2 DOCUMENTATION COPY ---- //
    
        // Setting up parameter subscriber
        param_subscriber_ = std::make_shared<rclcpp::ParameterEventHandler>(this);
        
        auto cb = [this](const rclcpp::Parameter & p) {
            this->threshold_ = p.as_double();
        };
        cb_handle_ = param_subscriber_->add_parameter_callback("brightness_threshold", cb);

        // ---- END OF ROS2 DOCUMENTATION COPY ---- //

        // GRAYSCALE DEBUG -- PUBLISHER FOR RQT
        debug_pub_ = this->create_publisher<sensor_msgs::msg::Image>("debug_grayscale", 10);

    }

    private:
    // Setting up variables for publisher, subscriber and parameter requirements
    rclcpp::Publisher<geometry_msgs::msg::Pose>::SharedPtr publisher_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    std::shared_ptr<rclcpp::ParameterEventHandler> param_subscriber_;
    std::shared_ptr<rclcpp::ParameterCallbackHandle> cb_handle_;
    
    // Setting up brightness threshold variable
    double threshold_;

    // GRAYSCALE DEBUG -- PUBLISHER FOR RQT
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr debug_pub_;

    // ---- FUNCTIONS ---- //

    // Generates CoQ and publishes grayscale image for debug
    void topic_callback(sensor_msgs::msg::Image::SharedPtr img) {
        // First must grayscale the image using cv library
        cv_bridge::CvImagePtr grayscale_ptr = cv_bridge::toCvCopy(img, "mono8");

        // Then, apply threshold on brightness and compute CoG using result
        std::array<int, 2> to_publish_ = this->find_CoG(grayscale_ptr);

        // Publishing result to topic
        auto message = geometry_msgs::msg::Pose();
        // Setting coordinates (SE(3) and rotation)
        message.position.x = to_publish_[0]; message.position.y = to_publish_[1]; message.position.z = 0.0;
        message.orientation.x = 0.0; message.orientation.y = 0.0; message.orientation.z = 0.0; message.orientation.w = 1.0;
        this->publisher_->publish(message);

        // GRAYSCALE DEBUG -- PUBLISHER FOR RQT
        debug_pub_->publish(*grayscale_ptr->toImageMsg());

    }

    // Helper function to get CoG of all pixels of grayscale image
    std::array<int, 2> find_CoG(cv_bridge::CvImagePtr gscale_ptr) {
        // Logic: Add all x/y coordinates of bright pixels, and divide by total no. of bright pixels.
        int x_total = 0;
        int y_total = 0;
        int no_pixels = 0;

        // We must cycle over the width and the height, and see the values per pixel
        for (int y = 0; y < gscale_ptr->image.rows; y++) {
            for (int x = 0; x < gscale_ptr->image.cols; x++) {
                // If pixel value not bright enough, continue
                if (gscale_ptr->image.at<uint8_t>(y, x) < threshold_) {
                    continue;
                } 

                // Otherwise, must add coordintes to total x and y
                x_total = x_total + x;
                y_total = y_total + y;
                no_pixels++;
            }
        }
        
        // If no pixels found, return just top left at 0,0
        if (no_pixels == 0) {
            return std::array<int, 2>({0, 0});
        } 

        // Otherwise return result
        return std::array<int, 2>({x_total/no_pixels, y_total/no_pixels});
    }
};

// As taken from tutorial on ros2 website
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ObjectPositionIndicator>());
  rclcpp::shutdown();
  return 0;
}