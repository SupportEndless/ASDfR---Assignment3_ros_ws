from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='assignment3_2_unit_test',
            executable='XRF_unit_test',
            remappings=[
                ('/output/right_motor/setpoint_vel', '/input/right_motor/setpoint_vel'),
                ('/output/left_motor/setpoint_vel', '/input/left_motor/setpoint_vel'),
            ],
            parameters=[{
                'amp': 2.0,
                'setting' : 'constant'
            }]
        )
    ])