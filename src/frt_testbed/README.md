# frt_testbed — Assignment 3.2 test harness

SRT ROS 2 node that exercises the XRF2 FRT pass-through (`assignment3_2`).
No controller logic; just signal injection + logging.

## Topics

| Direction | Topic       | Message type              | Notes                          |
|-----------|-------------|---------------------------|--------------------------------|
| OUT       | `Ros2Xeno`  | `xrf2_msgs/msg/Ros2Xeno`  | Test setpoints to the FRT side |
| OUT       | `XenoCmd`   | `std_msgs/msg/Int32`      | State-machine commands         |
| IN        | `Xeno2Ros`  | `xrf2_msgs/msg/Xeno2Ros`  | Encoder feedback (in `example_c/_d`) |
| IN        | `XenoState` | `std_msgs/msg/Int32`      | Current FRT state              |

XRF2 state-machine commands (from `XenoRosCommandBridge.hpp`):
`INITIALISE=1, START=2, STOP=3, RESET=4, PAUSE=5, QUIT=6`.

## Parameters

- `pattern` — one of `zero`, `step`, `ramp`, `sine`, `square`, `differential`
- `amplitude_left`, `amplitude_right`
- `frequency_hz`, `duration_s`, `publish_rate_hz`
- `max_abs_setpoint` — safety clamp (rad/s) before publishing
- `autostart` — sends `INITIALISE` then `START` on boot
- `log_path` — CSV output `t, cmd_left, cmd_right, fb_left, fb_right, state`

## Boot sequence on the Pi (RELbot on its stand!)

```bash
# 1. In a Xenomai EVL terminal: start the FRT binary
./build/assignment3_2/XRF2_FRT_test

# 2. In a regular terminal (sourced ROS 2): start bridge + testbed
ros2 launch frt_testbed frt_passthrough_test.launch.py \
    pattern:=sine amplitude_left:=1.0 amplitude_right:=1.0 \
    frequency_hz:=0.5 duration_s:=10.0 \
    log_path:=/tmp/frt_sine.csv
```

To record a rosbag of the full loop:

```bash
ros2 bag record Ros2Xeno Xeno2Ros XenoCmd XenoState
```

## Verification tests (attach results to the report)

1. **Zero test** — `pattern:=zero`. `Ros2Xeno` all zero, motors silent,
   `Xeno2Ros` encoder values stable at rest.
2. **Encoder path** — `pattern:=zero`, manually rotate each wheel forward,
   confirm `Xeno2Ros.example_c/_d` increase monotonically.
3. **Step** — `pattern:=step amplitude_left:=1.0 amplitude_right:=1.0
   duration_s:=2.0`. Both wheels must turn forward (camera-facing direction).
4. **Differential** — `pattern:=differential amplitude_left:=1.0
   amplitude_right:=1.0`. Wheels turn in opposite directions → sign check.
5. **Sine sweep** — `pattern:=sine frequency_hz:=0.5 duration_s:=10`. Record
   a rosbag + CSV; plot cmd vs fb to show end-to-end loop integrity.
6. **Safety** — Ctrl-C mid-sine. Verify the STOP + QUIT commands are sent,
   FRT transitions to STOPPED, motors stop.

## Answers to the manual's questions

**Q1. Advantages / disadvantages of separately testing the FRT skeleton first?**

*Advantages:* every non-control failure mode (SPI / FPGA wiring, xbuf IPC,
ros_xeno_bridge plumbing, state machine, build system) is isolated and
debugged before any control math is added — so when 3.3 breaks, we know the
cause is in the controller, not the infrastructure. Regression-testable with
patterns that stress the signal path (sine, step, differential).

*Disadvantages:* the "empty" 20-sim model produces zero PWM regardless of the
setpoints, so the motor / actuator path can only be verified by bypassing the
controller, or by temporarily wiring `y[0..1] = u[2..3]` in the empty model.
Timing / latency figures measured here are optimistic because the run loop
body is trivial.

**Q2. Advantage of starting from an empty 20-sim controller for 3.3?**

The 20-sim code generator produces the exact same file layout (`LoopController.{h,cpp}`,
`controller/common/*`, inputs `u[0..3]`, outputs `y[0..1]`) regardless of
model complexity. Starting empty means the CMake integration, signal mapping
in `run()`, and the XRF2 framework contract are already proven correct, so in
3.3 only the 20-sim model itself has to change — drop in the new generated
files and rebuild.
