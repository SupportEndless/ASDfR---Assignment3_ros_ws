# IMPORTANT FOR ENCODER DATA
The encoder data is not proper. If looking from above, with the front of the robot facing north direction, then the following is true:
Right encoder:      Channel1,       rotation flipped (clockwise measurement positive)
Left encoder:       Channel2,       rotation normal (counter clockwise measurement positive)

# For setting FSM states:
ros2 topic pub --once /XenoCmd std_msgs/msg/Int32 "{data: x}"
