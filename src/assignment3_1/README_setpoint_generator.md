Node setpoint_generator
-----------------------------------------------
### Description 
Generates setpoints for the relbot(simulator) based on the received CoG from 
the object_tracker node. Sends this out to the relbot_adapter.
Contains unit test with sine wave, which has many options, shown in parameters.

### Inputs
`/output/robot_pose`
            Gets current pose. Type: `geometry_msgs::msg::PoseStamped`
`/output/camera_position`
            Gets current camera position from relbot. Type: `geometry_msgs::msg::PointStamped`
`CoG`
            The CoG from the object_tracker node. Type: `geometry_msgs::msg::Pose`

### Outputs
`input/right_motor/setpoint_vel`:   Setpoint for the right wheel
`input/left_motor/setpoint_vel`:    Setpoint for the left wheel

### Build
To build this node:
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
To run, use either of the following commands:
`ros2 run assignment3_1 setpoint_generator`
`ros2 launch src/assignment3_1/launch/launch_tracker_sim.py`

### Parameters
`omega_right_wheel`:                
                Right wheel omega for unit test. Default 1.0 (double)
`omega_left_wheel`:                 
                Left wheel omega for unit test. Default 1.0 (double)
`left_wheel_velocity_offset`:       
                Velocity offset for left wheel when doing unit test. Default 0.0 (double)
`right_velocity_amplitude`:         
                Amplitude of unit test sine wave on right wheel. Default 0.6 (double)
`left_velocity_amplitude`:
                Amplitude of unit test since wave on left wheel. Default 0.6 (double)
`publish_rate`
                Wait time between wall timer publishes. Default 0.01 (double)
`use_subscriber`
                Sets node so that it sends out new setpoint when receiving image. Default: false (bool)
`do_brightness_tracking`
                Allows node to do brightness tracking. Default false (bool)
`proportional_gain`
                Gain used in the tracking cotroller. Default 1.0 (double)

### Core components 
* `timer_callback()`:                       Runs main logic, gets wheel velocities and publishes. Used for unit test.
* `brightness_tracking_timer_callback()`:   Runs main logic for brightness tracking. Calculates error signal and wheel setpoints, and publishes.