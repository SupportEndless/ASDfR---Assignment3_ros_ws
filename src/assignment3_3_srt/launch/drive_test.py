from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='assignment3_3_srt',
            executable='drive_test',
            remappings=[
                ('/set_vel_right', '/input/right_motor/setpoint_vel'),
                ('/set_vel_left', '/input/left_motor/setpoint_vel'),
            ],
        )
    ])