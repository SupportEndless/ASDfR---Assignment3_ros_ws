Package object_tracker
-------------------------------------------------
### Description
Ths package contains a node that calculates center of gravity of brightness.
Takes in /image from cam2image_vm2ros package. 

### Inputs
`/input/image`
        Type: sensor_msgs/msg/Image

### Outputs
`/output/CoG`
        Type: geometry_msgs/msg/Point

`/output/image_grayscale`
        Type: sensor_msgs/msg/Image

### Run
In a terminal run the following command
`ros2 run object_tracker object_tracker`

The system can also be launched using launch files, which are located in the launch folder with the launch commands:
`ros2 launch assignment3_1_simulation_test_local.py`
`ros2 launch assignment3_1_simulation_test.py`

Both of these require the cam2image_vm2ros and the relbot_adapter package. The local one uses the VM settings, while the other one runs the settings for the RELbot.

### Parameters
double `brightness_threshold`: Sets the threshold for when a pixel is considered "bright". Default = 125.0

### Core components
* `calculate_CoG()`: Converts input image to grayscale, and calculates CoG. Returns geometry_msgs/msg/Point. Also publishes this grayscale image to /output/image_grayscale directly
* `object_tracking_callback()`: Is called on image receive, uses calculate_CoG() to get CoG, and publishes this to /output/CoG 
