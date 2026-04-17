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
            package='assignment1_2',
            executable='setpoint_generator',
            ros_arguments=['-p', 'omega_right_wheel:=0.2', '-p', 'omega_left_wheel:=0.2', '-p', 'right_velocity_amplitude:=1.0', '-p', 'left_velocity_amplitude:=1.0', '-p', 'publish_rate:=0.01']
        )
    ])