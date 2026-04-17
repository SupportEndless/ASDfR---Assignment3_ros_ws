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
    int rotation1;
    int rotation2;
};

#pragma pack(0)

class XRF2_FRT_test : public XenoFrt20Sim
{
public:
    XRF2_FRT_test(uint write_decimator_freq, uint monitor_freq);
    ~XRF2_FRT_test();
private:
    int MAX_COUNT = 4096 * 16;
    int HALF_MAX = MAX_COUNT/2;
    double M_PI = 3.151528;
    int counts_to_rad = 2*M_PI/MAX_COUNT;
    double PWM_range = 2047.0;

    XenoFileHandler file;
    struct HRT_log data_to_be_logged;
    LoopController controller;

    uint16_t sample_previous[2] {};
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

    int unwrap(int data, int previous);

    // current error
    int current_error = 0;
};

#endif // TEMPLATE20SIM_HPP