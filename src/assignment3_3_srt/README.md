Package assignment3_3_srt
-----------------------------------------------
### Description 
This package implements the SRT part of assignment3.2. 
Main functionality is to drive in a straight line, then
turn 90 degrees after a set timer. 

### Inputs
Does not receive inputs, only sends.

### Outputs
`set_val_left`:     Left wheel setpoint
`set_vel_right`:    Right wheel setpoint

### Build
Simply build through colcon using
`colcon build --packages-select assignment3_3_srt`
Note that the XRF2 message packages must be correctly configured and installed.

There is also a shell script provided that builds the package and all dependencies.
This is done through setting the shell to an executable, and then running it.
The shell script must be placed in the base workspace, and must NOT be given sudo access.
Run the following from the ros2_ws:
`chmod +X build_pi_3_3.sh`
`./build_pi_3_3.sh`
You may need to set permisson for the file, so use
`chmod 777 build_pi_3_3.sh`, or something similar.

### Run
Before running this node, make sure the bridge and adapter nodes are active, as well
as the FRT part given in the package assignment3_3.
Run either of the following commands to start the node:
`ros2 run assignment3_3_srt drive_test`
`ros2 launch src/assignment3_3_srt/launch/drive_test.py`
There is also a launch file for launching both the bridge and the adapter nodes, which runs through the following command:
`ros2 launch src/assignment3_3_srt/launch/start_bridge.py`

### Parameters
Does not take parameters.

### Core components 
* `timer_callback_()`:      Runs main logic for determining the next setpoints.
* `publish_setpoints_()`:   Publishes left and right wheel setpoints.