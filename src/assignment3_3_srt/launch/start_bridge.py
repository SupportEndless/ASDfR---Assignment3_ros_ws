from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='relbot_adapter',
            executable='relbot_adapter',
            ros_arguments=['--params-file', 'src/cam2image_vm2ros/config/cam2image.yaml']
        ),
        Node(
            package='ros_xeno_bridge',
            executable='RosXenoBridge'
        )
    ])