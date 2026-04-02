#!/bin/bash

# Getting ros version
DETECTED_DISTRO=$(ls /opt/ros/ 2>/dev/null | head -n 1)

source /opt/ros/$DETECTED_DISTRO/setup.bash

# Starting by building all dependencies
echo "Building message packages first"
colcon build --packages-select \
    relbot_msgs \
    xrf2_msgs

if [ -f install/setup.bash ]; then
    source install/setup.bash
fi

colcon build --packages-select \
    cam2image_vm2ros \
    relbot_adapter \
    relbot_simulator \
    assignment3_1 \
    assignment3_2 \
    object_tracker \
    sequence_controller
