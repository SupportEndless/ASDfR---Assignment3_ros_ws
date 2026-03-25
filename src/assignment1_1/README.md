Package cam2image_vm2ros
-----------------------------------------------
### Description
Assignment specific package for assignment 1 of Advanced Software Development for Robotics for Rogier Meijerman
The folder contains three extra files compared to the base:
1. image_brightness.cpp
2. brightness_receiver.cpp
3. object_position_indicator.cpp

Number 1. and 2. are for assignemnts 1.1, 1.2 and 1.3. The object_position_indicator is for assignment 1.4. 
The image_brightness.cpp determines average brightness of the image through a subscriber to the broadcasted images. It also creates a publisher that sends out the average brightness. brightness_receiver connects to the average brightness publisher, and displays the average brightness it receives (if brightniss > threshold that is).
Lastly, object_position_indicator subscribes to the image as well, grayscales it and calculates the CoG for the brightness, which it publishes on a topic as well. 

### Building environment
From the ROS2 base workspace use
colcon build --packages-select assignment1_1

source:
source install/setup.bash

### Run
In a terminal run the following commands (all in seperate terminals from the base folder)

Setting up cam:
ros2 run cam2image_vm2ros cam2image --ros-args --params-file src/cam2image_vm2ros/config/cam2image.yaml

#### Assignment 1.1 / 1.2 / 1.3

Setting up image_brightness - receives image and publishes results
ros2 run assignment1_1 image_brightness

Setting up subscriber
ros2 run assignment1_1 brightness_receiver

#### Assignment 1.4

Setting up object_postion_indicator
ros2 run assignment1_1 object_position_idicator

For output
ros2 topic echo /CoG

### Parameters
double `brightness_threshold`: brightness threshold before a pixel is counted as bright. Between 0.0 - 255.0 
        Only defined for image_brightness and object_position_indicator

