from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription(
        [
            Node(
                package="relbot_simulator",
                executable="relbot_simulator",
                arguments=["--ros-args", "--log-level", "WARN"],
            ),
            Node(
                package="cam2image_vm2ros",
                executable="cam2image",
                arguments=["--ros-args", "--log-level", "WARN", '--params-file', 'src/cam2image_vm2ros/config/cam2image.yaml'],
            ),
            Node(
                package="assignment1_1",
                executable="object_position_indicator",
                remappings=[("input_image", "/output/moving_camera")],
            ),
            Node(
                package="sequence_controller",
                executable="sequence_controller",
                remappings=[
                    ("camera_position", "/output/camera_position"),
                    ("left_motor_setpoint_vel", "/input/right_motor/setpoint_vel"),
                    ("right_motor_setpoint_vel", "/input/right_motor/setpoint_vel"),
                ],
            ),
        ]
    )
