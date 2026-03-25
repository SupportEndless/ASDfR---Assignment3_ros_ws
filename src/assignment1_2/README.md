Assignment 1.2
===============

# About
This package works in tandem with the "relbot_adapter", "relbot_msgs" and "relbot_simulator" packages. 
It generates a sinusoidal setpoint pattern for both the right wheel and left wheel velocities for the relbot.

# Install and build
This package can be installed normally, so from the base workspace:

"
colcon build --packages-select assignment1_2
"

# Running
There are two ways of running the package, the first is to manually run the package. The second is to use the launch file.

## Manually running the package
To run the package, make sure the "relbot_adapter" and "relbot_simulator" are running. Note that this also requires the "cam2image_vm2ros" pacakage to be running.
To run with basic behavior:
"
ros2 run assignment1_2 setpoint_generator
"

## Using the launch file
Make sure the launch file "launch_RELbot_sim.py" is located in a launch folder in the base workspace, so:
ros2_ws
    -build
        ...
    -install
        ...
    -launch
        launch_RELbot_sim.py
        ...
    -log
        ...
    -src
        ...
This file launches all the relevant nodes, and passes arguments to the setpoint generator. These can be adjusted in the Python file. More details below.

# Paramters
The follow parameters can be tuned
- "omega_right_wheel": <double> Sets the angular frequency of the sine wave velocity output of the right wheel. Default 1.0
- `omega_left_wheel`: <double> Same as right, but for left wheel. Default 1.0
- `left_wheel_offset`: <double> Sets an offset to the angular frequency (phase shift) to the left wheel, usually for plotting purpose. Default 0.0
- `right_velocity_amplitude`: <double> Sets the amplitude of the velocity setpoints. Default 0.6
- `left_velocity_amplitude`: <double> Same as right amplitude, but for left wheel. Default 0.6
- `publish_rate`: <double> Sets the delay between publishes of new setpoints in seconds, if the subscriber type is not used. Default 0.1
- `use_subscriber`: <bool> If set to true, use legacy code that sends out a new setpoint every time a new publish is made on the /output/robot_pose topic. This is very CPU intensive, and should not be used in general. Default false
- `do_brightness_tracking`: <bool> Enables or disables the brightness tracking mode. Default false
- `proportional_gain` <double> The gain used in the proportional controller for the brightness tracking system. Default 1.0 (recommend lower values)