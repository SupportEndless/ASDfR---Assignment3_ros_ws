// TODO: Add top of code info section
#include "object_tracker.hpp"

ObjectTracker::ObjectTracker() :Node("object_tracker") {
    // Initializing ROS parameters first
    this->initialize_ros_parameters();

    // Setting up subscription
    image_subscriber_ = this->create_subscription<sensor_msgs::msg::Image>(
        "/input/image",
        1,
        std::bind(&ObjectTracker::object_tracking_callback, this, _1)
    );

    // Setting up publisher
    brightness_publisher_ = this->create_publisher<geometry_msgs::msg::Point>(
        "output/CoG",
        1
    );

}

void ObjectTracker::initialize_ros_parameters() {
    this->declare_parameter("brightness_threshold", static_cast<double>(125.0));
    this->brightness_threshold_ = this->get_parameter("brightness_threshold").as_double();
}

geometry_msgs::msg::Point ObjectTracker::calculate_CoG(sensor_msgs::msg::Image::SharedPtr img) {
    // Turning the image into a greyscale version
    cv_bridge::CvImagePtr greyscale_ptr = cv_bridge::toCvCopy(img, "mono8");

    // Setting up temp variables to zero
    int x_total = 0;
    int y_total = 0;
    int num_pixels = 0;

    // Cycling over width and height, and checking brightness value per pixel
    for (int y = 0; y < greyscale_ptr->image.rows; y++) {
        for (int x = 0; x < greyscale_ptr->image.cols; x++) {
            if (greyscale_ptr->image.at<uint8_t>(y, x) < this->brightness_threshold_) continue;
            
            // If higher than threshold, then add tot total
            x_total += x;
            y_total += y;
            num_pixels++;
        }   
    }

    // If number of pixels is zero, we must return empty point
    if (num_pixels == 0) {
        geometry_msgs::msg::Point out = geometry_msgs::msg::Point{};
        out.x = -1; out.y = -1; out.z = -1;
        return out;
    }

    // Returning actual center
    geometry_msgs::msg::Point out = geometry_msgs::msg::Point{};
    out.x = x_total/num_pixels;
    out.y = y_total/num_pixels;

    // Put in tracker var
    trackedCoGs.push_back(out.x);

    return out;
}

void ObjectTracker::object_tracking_callback(sensor_msgs::msg::Image::SharedPtr img) {
    // Getting CoG
    geometry_msgs::msg::Point CoG = this->calculate_CoG(img);

    // Posting
    this->brightness_publisher_->publish(CoG);
}

// As taken from tutorial on ros2 website
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ObjectTracker>());
  rclcpp::shutdown();

  std::ofstream file("x_output.txt");
  for (int i = 0; i < trackedCoGs.size(); i++) {
    file << trackedCoGs[i];
    
    // Checking if this is the last entry, then skip the comma
    if(i == trackedCoGs.size()-1) return;
    file << ",";

  }

  return 0;
}