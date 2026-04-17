Package assignment3_2_unit_test
-----------------------------------------------
### Description 
This package implements the SRT unit test node, which should be used 
in tandem with the assignment3_2 package. It generates setpoint values
which the FRT part interpolates into PWM values. Contains three main unit
tests which the user can alter through the command line, or the launch file, 
which are:
1) Sine wave (default)
2) step function
3) Constant

### Inputs
Node does not have any inputs.

### Outputs
`output/right_motor/setpoint_vel`: Sends out right motor setpoint velocity.
`output/left_motor/setpoint_vel`: Sends out left motor setpoint velocity.

### Build
Simply build through colcon using
`colcon build --packages-select assignment3_2_unit_test`

### Run
Before running this node, make sure to always run the assignment3_2 FRT part first.
This can be done (after compiling) through:
`./build/assignment3_2/assignment3_2`
After that, run either of these commands from the base workspace:
`ros2 run assignment3_2_unit_test XRF_unit_test`
`ros2 launch src/assignment3_2_unit_test/launch/unit_test.py`

Note that a launch file is also provided to start both the XenoRosBridge and relbot_adapter
node, which can be ran through:
`ros2 launch src/assignment3_2_unit_test/launch/start_bridge.py`


### Parameters
`setting`
            Selects the type of unit test. Choices are either `sine`, `step` or `constant`. Default: `sine`.

`amp`
            Sets the amplitude of the output for the unit test as double. Default: 2.0.

`publish_rate`
            Sets the publish time for the publishers for the wheel velocities. Default: 0.01.

### Core components 
* `timer_callback()`:       Calculates current time, updates motor setpoints and publishes.
* `get_motor_inputs()`:     Selects type of unit test, and passes appropriate setpoint to   timer_callback