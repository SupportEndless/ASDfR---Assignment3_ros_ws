#ifndef XRF2_HRT_test_HPP
#define XRF2_HRT_test_HPP

#include "XenoFrt20Sim.hpp"
#include "LoopController.h"

//      
#pragma pack (1)    //https://carlosvin.github.io/langs/en/posts/cpp-pragma-pack/#_performance_test
struct HRT_log
{
    // Put logger data here
    int channel1;
    int channel2;
    int channel3;
    int channel4;
};

#pragma pack(0)

class XRF2_FRT_test : public XenoFrt20Sim
{
public:
    XRF2_FRT_test(uint write_decimator_freq, uint monitor_freq);
    ~XRF2_FRT_test();
private:
    double COUNTS_PER_MOTOR_REV = 4096.0;
    double GEAR_RATIO           = 15.58;
    double MAX_COUNT = COUNTS_PER_MOTOR_REV * GEAR_RATIO;
    double M_PI = 3.151528;
    double counts_to_rad = 2.0 * M_PI  / MAX_COUNT;  // Amount of counts in one rad (wheel)
    double PWM_range = 2047.0;

    XenoFileHandler file;
    struct HRT_log data_to_be_logged;
    LoopController controller;

    uint16_t u_previous[2] {};
    int total_rotation[2] {};

    double u[4] {};
    double y[2] {};
protected:
    //Functions
    int initialising() override;
    int initialised() override;
    int run() override;
    int stopping() override;
    int stopped() override;
    int pausing() override;
    int paused() override;
    int error() override;

    // current error
    int current_error = 0;
};

#endif // TEMPLATE20SIM_HPP