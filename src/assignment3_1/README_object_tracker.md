Node object_tracker
-----------------------------------------------
### Description 
This node tracks an object based on how bright it is. Threshold for 
bright pixels can be set through parameter.

### Inputs
`/input/image`
                Image received from the `cam2image_vm2ros` package. Type: `sensor_msgs::msg::Image`

### Outputs
`output/CoG`
                Center of gravity of the object that is being tracked. Type: `geometry_msgs::msg::Point`
`output/image_grayscale`
                Image but grayscaled. Type: `sensor_msgs::msg::Image`

### Build
Simply build through colcon using
`colcon build --packages-select assignment3_1`

There is also a shell script provided that builds the package and all dependencies.
This is done through setting the shell to an executable, and then running it.
The shell script must be placed in the base workspace, and must NOT be given sudo access.
Run the following from the ros2_ws:
`chmod +X build_pi_3_1.sh`
`./build_pi_3_1.sh`
You may need to set permisson for the file, so use
`chmod 777 build_pi_3_1.sh`, or something similar.

Note that you still have to source `install/setup.bash` after this shell script.

### Run
Run this node through
`ros2 run assignment3_1 object_tracker`

There is also a launch file provided:
`ros2 launch src/assignment3_1/launch/launch_tracker_sim.py`

### Parameters
`brightness_threshold`:     Sets threshold for when a pixel is considered bright. Default: 125.0 (double)

### Core components 
* `calculate_CoG()`:        Calculates CoG from received image