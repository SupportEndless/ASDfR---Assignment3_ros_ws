Package assignment3_1
-----------------------------------------------
### Description 
This package builds on the relbot adapter and simulator. 
The main purpose of this package is to test the stress impact of these 
nodes when running on a Raspberry Pi 4B with one Xenomai core. 
The nodes work together to follow a center of brightness and calculate a 
setpoint velocity based on the CoG. 

## Content
It contains of an object tracker, and a setpoint_generator.
For both these nodes there is a readme file in this folder.

## Dependencies
`relbot_adapter`:       As provided by the UT
`relbot_simulator`:     As provided by the UT

## Run
To run the whole package, run the following command:
`ros2 launch src/assignment3_1/launch/launch_tracker_sim.py`