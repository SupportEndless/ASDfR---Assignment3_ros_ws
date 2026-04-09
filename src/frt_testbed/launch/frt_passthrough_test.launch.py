"""Launch file for Assignment 3.2 XRF2 FRT pass-through testbed.

Brings up:
  1. The XRF2 ros_xeno_bridge node (SRT <-> xbuf <-> HRT).
  2. The frt_testbed_node (pattern generator + logger).

NOTE: The Xenomai-side binary 'XRF2_FRT_test' must already be running in an
EVL terminal on the Pi BEFORE this launch file is started. See the package
README for the full boot sequence.
"""
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    pattern_arg = DeclareLaunchArgument(
        "pattern", default_value="zero",
        description="zero|step|ramp|sine|square|differential")
    amp_l = DeclareLaunchArgument("amplitude_left", default_value="0.0")
    amp_r = DeclareLaunchArgument("amplitude_right", default_value="0.0")
    freq = DeclareLaunchArgument("frequency_hz", default_value="0.5")
    dur = DeclareLaunchArgument("duration_s", default_value="0.0")
    rate = DeclareLaunchArgument("publish_rate_hz", default_value="100.0")
    autostart = DeclareLaunchArgument("autostart", default_value="true")
    log_path = DeclareLaunchArgument("log_path", default_value="")

    bridge = Node(
        package="ros_xeno_bridge",
        executable="ros_xeno_bridge",
        name="ros_xeno_bridge",
        output="screen",
    )

    testbed = Node(
        package="frt_testbed",
        executable="frt_testbed_node",
        name="frt_testbed_node",
        output="screen",
        parameters=[{
            "pattern": LaunchConfiguration("pattern"),
            "amplitude_left": LaunchConfiguration("amplitude_left"),
            "amplitude_right": LaunchConfiguration("amplitude_right"),
            "frequency_hz": LaunchConfiguration("frequency_hz"),
            "duration_s": LaunchConfiguration("duration_s"),
            "publish_rate_hz": LaunchConfiguration("publish_rate_hz"),
            "autostart": LaunchConfiguration("autostart"),
            "log_path": LaunchConfiguration("log_path"),
        }],
    )

    return LaunchDescription([
        pattern_arg, amp_l, amp_r, freq, dur, rate, autostart, log_path,
        bridge, testbed,
    ])
