#!/bin/bash

# Getting ros version
DETECTED_DISTRO=$(ls /opt/ros/ 2>/dev/null | head -n 1)

source /opt/ros/$DETECTED_DISTRO/setup.bash

if [ -z "$DETECTED_DISTRO" ]; then
    echo "ERROR: No ROS 2 distribution found in /opt/ros/"
    exit 1
fi

# Starting by building all dependencies
echo "Building message packages first"
colcon build --packages-select \
    relbot_msgs \
    xrf2_msgs

if [ -f install/setup.bash ]; then
    echo "Local setup.bash file found, sourcing..."
    source install/setup.bash
fi

echo "Building other packages..."
colcon build --packages-select \
    cam2image_vm2ros \
    relbot_adapter \
    relbot_simulator \
    assignment3_1 \
    object_tracker \
    sequence_controller
