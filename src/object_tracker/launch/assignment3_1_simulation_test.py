from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # First up, load camera
        Node(
            package='cam2image_vm2ros',
            executable='cam2image',
            ros_arguments=['--params-file', 'src/cam2image_vm2ros/config/cam2image_relbot.yaml']
        # Then load the simulator and adapter
        ),
        # Node(
        #     package='relbot_simulator',
        #     executable='relbot_simulator'
        # ),
        Node(
            package='relbot_adapter',
            executable='relbot_adapter',
            remappings=[("/output/motor_cmd", "/input/motor_cmd")]
        ),
        # Finally, load the object tracker, so we have output
        Node(
            package='object_tracker',
            executable='object_tracker',
            ros_arguments=['-p', 'brightness_threshold:=125.0'],
            remappings=[("input/image", "image")]
        )
    ])