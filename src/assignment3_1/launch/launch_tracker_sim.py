from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='cam2image_vm2ros',
            executable='cam2image',
            ros_arguments=['--params-file', 'src/cam2image_vm2ros/config/cam2image.yaml']
        ),
        Node(
            package='relbot_simulator',
            executable='relbot_simulator'
        ),
        Node(
            package='relbot_adapter',
            executable='relbot_adapter',
            ros_arguments=['-r', '/output/motor_cmd:=/input/motor_cmd']
        ),
        Node(
            package='assignment3_1',
            executable='object_tracker',
            ros_arguments=['-p', 'brightness_threshold:=100.0']
        ),
        Node(
            package='assignment3_1',
            executable='setpoint_generator',
            ros_arguments=['-p', 'do_brightness_tracking:=true', '-p', 'proportional_gain:=1.0']
        )
    ])