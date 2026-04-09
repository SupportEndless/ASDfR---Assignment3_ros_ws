#include "XRF2_FRT_test.hpp"

#include <algorithm>
#include <cstring>

#include "IcoIo.hpp"   // for MAX_PWM_VALUE

XRF2_FRT_test::XRF2_FRT_test(uint write_decimator_freq, uint monitor_freq) :
    XenoFrt20Sim(write_decimator_freq, monitor_freq, file, &data_to_be_logged),
    file(1, "./xrf2_logging/assignment3_2", "bin"),
    controller()
{
    printf("%s: Constructing XRF2_FRT_test (assignment 3.2 pass-through)\n", __FUNCTION__);

    // Log variables. Order MUST match the FrtTestLog struct field order.
    logger.addVariable("set_vel_left",  double_);
    logger.addVariable("set_vel_right", double_);
    logger.addVariable("enc_left",      integer);
    logger.addVariable("enc_right",     integer);
    logger.addVariable("steer_left",    double_);
    logger.addVariable("steer_right",   double_);
    logger.addVariable("pwm_left",      integer);
    logger.addVariable("pwm_right",     integer);

    // Clear I/O vectors and actuator outputs
    std::memset(u, 0, sizeof(u));
    std::memset(y, 0, sizeof(y));
    std::memset(&data_to_be_logged, 0, sizeof(data_to_be_logged));

    actuate_data.pwm1 = 0;  actuate_data.val1 = false;
    actuate_data.pwm2 = 0;  actuate_data.val2 = false;
    actuate_data.pwm3 = 0;  actuate_data.val3 = false;
    actuate_data.pwm4 = 0;  actuate_data.val4 = false;

    // Infinite run (don't let the 20-sim model self-terminate)
    controller.SetFinishTime(0.0);
}

XRF2_FRT_test::~XRF2_FRT_test()
{
}

int16_t XRF2_FRT_test::clamp_pwm(double v) const
{
    const double lim = static_cast<double>(MAX_PWM_VALUE);
    if (v >  lim) v =  lim;
    if (v < -lim) v = -lim;
    return static_cast<int16_t>(v);
}

void XRF2_FRT_test::halt_motors()
{
    actuate_data.pwm1 = 0;
    actuate_data.pwm2 = 0;
    actuate_data.pwm3 = 0;
    actuate_data.pwm4 = 0;
    // Base class step() pushes actuate_data to the FPGA next cycle.
}

int XRF2_FRT_test::initialising()
{
    // Set physical and cyber system up for use. Return 1 to go to initialised.
    evl_printf("XRF2_FRT_test: initialising\n");

    logger.initialise();
    ico_io.init();

    // Initial controller call so internal 20-sim state is valid.
    controller.Initialize(u, y, 0.0);

    halt_motors();
    return 1;
}

int XRF2_FRT_test::initialised()
{
    // Wait for the SRT side (XenoCmd START) to move us into run.
    return 0;
}

int XRF2_FRT_test::run()
{
    // Base class XenoFrtRosIco::step() has already:
    //   - refreshed sample_data from the FPGA
    //   - refreshed ros_msg  from the SRT Ros2Xeno topic
    // and will push actuate_data + xeno_msg on the next cycle.

    logger.start();

    // --- 1. Sample inputs into the 20-sim u vector ----------------------
    u[0] = static_cast<double>(sample_data.channel1);   // PosLeft  (encoder)
    u[1] = static_cast<double>(sample_data.channel2);   // PosRight (encoder)
    u[2] = ros_msg.left_wheel_vel;                      // SetVelLeft
    u[3] = ros_msg.right_wheel_vel;                     // SetVelRight

    // --- 2. Empty 20-sim "loop controller": y = f(u) --------------------
    //        For Assignment 3.2 this block is intentionally empty / pass-
    //        through. The real controller arrives in Assignment 3.3.
    controller.Calculate(u, y);

    // --- 3. Drive motors (PWM), clamped to MAX_PWM_VALUE ----------------
    actuate_data.pwm1 = clamp_pwm(y[0]);    // SteerLeft
    actuate_data.pwm2 = clamp_pwm(y[1]);    // SteerRight

    // --- 4. Feedback up to SRT on Xeno2Ros ------------------------------
    //        example_c / example_d are the generic float64 slots in
    //        xrf2_msgs/msg/Xeno2Ros. We re-use them for the two encoder
    //        channels so the testbed node can plot cmd vs fb.
    xeno_msg.example_c = static_cast<double>(sample_data.channel1);
    xeno_msg.example_d = static_cast<double>(sample_data.channel2);

    // --- 5. Populate the log record for this cycle ----------------------
    data_to_be_logged.set_vel_left  = u[2];
    data_to_be_logged.set_vel_right = u[3];
    data_to_be_logged.enc_left      = sample_data.channel1;
    data_to_be_logged.enc_right     = sample_data.channel2;
    data_to_be_logged.steer_left    = y[0];
    data_to_be_logged.steer_right   = y[1];
    data_to_be_logged.pwm_left      = actuate_data.pwm1;
    data_to_be_logged.pwm_right     = actuate_data.pwm2;

    if (controller.IsFinished())
        return 1;
    return 0;
}

int XRF2_FRT_test::stopping()
{
    // Bring the physical system to a safe stop, then terminate the controller.
    evl_printf("XRF2_FRT_test: stopping\n");
    halt_motors();
    controller.Terminate(u, y);
    logger.stop();
    return 1;
}

int XRF2_FRT_test::stopped()
{
    // Hold motors at zero while waiting.
    halt_motors();
    return 0;
}

int XRF2_FRT_test::pausing()
{
    // Fast halt: zero the PWM immediately.
    evl_printf("XRF2_FRT_test: pausing\n");
    halt_motors();
    return 1;
}

int XRF2_FRT_test::paused()
{
    halt_motors();
    return 0;
}

int XRF2_FRT_test::error()
{
    // Safety: always keep motors at zero in error state.
    halt_motors();
    monitor.printf("XRF2_FRT_test: error state (code=%d)\n", current_error);
    return 0;
}
