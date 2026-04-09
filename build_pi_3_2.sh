#!/bin/bash

# Getting ros version
DETECTED_DISTRO=$(ls /opt/ros/ 2>/dev/null | head -n 1)

if [ -z "$DETECTED_DISTRO" ]; then
    echo "ERROR: No ROS 2 distribution found in /opt/ros/"
    exit 1
fi

if [ "$DETECTED_DISTRO" != "humble" ]; then
    echo "Wrong ROS2 version detected, please use version humble"
    echo "Detected version: $DETECTED_DISTRO"
    exit 1
fi

# --- Logic for building --- #
source /opt/ros/$DETECTED_DISTRO/setup.bash

# Starting by building all dependencies
echo "Building message packages first"
colcon build --executor sequential --parallel-workers 1 --symlink-install --packages-select \
    relbot_msgs \
    xrf2_msgs

if [ -f install/setup.bash ]; then
    echo "Local setup.bash file found, sourcing..."
    source install/setup.bash
else
    echo "Did not find new version, something went wrong in the dependency build"
    exit 1
fi

echo "Building other packages..."
colcon build --executor sequential --parallel-workers 1 --symlink-install --packages-select \
    cam2image_vm2ros \
    relbot_adapter \
    relbot_simulator \
    assignment3_2

# Sourcing new compiled version
if [ -f install/setup.bash ]; then
    echo "------------------------------------------------------------"
    echo "Local setup.bash file found, build complete."
    echo "Don't forget to source!"
    echo "------------------------------------------------------------"
else
    echo "Did not find new version, something went wrong in the build packages build"
    exit 1
fi
