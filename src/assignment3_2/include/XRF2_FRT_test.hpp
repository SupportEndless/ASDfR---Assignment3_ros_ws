#ifndef XRF2_FRT_TEST_HPP
#define XRF2_FRT_TEST_HPP

#include "XenoFrt20Sim.hpp"
#include "LoopController.h"

#pragma pack(1)
/**
 * @brief Binary log record written by XenoFrtLogger each cycle.
 *
 * Ordering and types are exactly what is registered with
 * logger.addVariable() in the constructor.
 */
struct FrtTestLog
{
    double   set_vel_left;   ///< u[2]: setpoint velocity left  (from Ros2Xeno)
    double   set_vel_right;  ///< u[3]: setpoint velocity right (from Ros2Xeno)
    int      enc_left;       ///< u[0]: encoder position left  (sample_data.channel1)
    int      enc_right;      ///< u[1]: encoder position right (sample_data.channel2)
    double   steer_left;     ///< y[0]: LoopController output left
    double   steer_right;    ///< y[1]: LoopController output right
    int      pwm_left;       ///< actuate_data.pwm1
    int      pwm_right;      ///< actuate_data.pwm2
};
#pragma pack(0)

/**
 * @class XRF2_FRT_test
 * @brief FRT pass-through controller for Assignment 3.2.
 *
 * Subclass of XenoFrt20Sim that wires the RELbot hardware (encoders + PWM) to
 * the SRT ROS 2 side through an empty 20-sim-generated LoopController. No
 * control math lives here; the 20-sim block is intentionally empty and merely
 * forwards values. This class exists to exercise the full XRF2 signal path.
 */
class XRF2_FRT_test : public XenoFrt20Sim
{
public:
    XRF2_FRT_test(uint write_decimator_freq, uint monitor_freq);
    ~XRF2_FRT_test();

private:
    XenoFileHandler  file;
    FrtTestLog       data_to_be_logged;
    LoopController   controller;

    // 20-sim I/O vectors (see controller/LoopController.info):
    //   u[0]=PosLeft, u[1]=PosRight, u[2]=SetVelLeft, u[3]=SetVelRight
    //   y[0]=SteerLeft, y[1]=SteerRight
    double u[4];
    double y[2];

    /// Clamp a floating-point steer output to the IcoIo PWM range.
    int16_t clamp_pwm(double v) const;

    /// Force both motors to zero PWM and push to the FPGA (safety).
    void halt_motors();

protected:
    // XenoFrt20Sim state callbacks
    int initialising() override;
    int initialised() override;
    int run() override;
    int stopping() override;
    int stopped() override;
    int pausing() override;
    int paused() override;
    int error() override;

    int current_error = 0;
};

#endif // XRF2_FRT_TEST_HPP
