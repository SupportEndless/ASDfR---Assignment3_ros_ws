#ifndef XRF2_HRT_test_HPP
#define XRF2_HRT_test_HPP

#include "XenoFrt20Sim.hpp"
#include "LoopController.h"

// Struct for logging data using XenoFRTLogger
#pragma pack (1)    //https://carlosvin.github.io/langs/en/posts/cpp-pragma-pack/#_performance_test
struct HRT_log
{
    // Logger logs both encoders and wheel angles
    int channel1;
    int channel2;
    int rotation1;
    int rotation2;
};

#pragma pack(0)

/**
 * @class XRF2_FRT_test
 * @brief Class for XRF part of assignment 3.3. Signals meant to be sent over RELbot_adapter
 * 
 *  Class uses XenoFrt20Sim inheritance, fills out the FSM in the template provided by XRF2 framework.
 *  Class includes function for wraparound for the encoders, and sends total angle to the automatically
 *  generated 20-sim controller.
 * 
 */
class XRF2_FRT_test : public XenoFrt20Sim
{
public:
    XRF2_FRT_test(uint write_decimator_freq, uint monitor_freq);
    ~XRF2_FRT_test();
private:
    // Encoder related
    int MAX_COUNT = 4096 * 16;
    int HALF_MAX = MAX_COUNT/2;
    double M_PI = 3.151528;
    
    // For converting between encoder values and angle in radians
    int counts_to_rad = 1/(2*M_PI*MAX_COUNT);

    XenoFileHandler file;
    struct HRT_log data_to_be_logged;
    LoopController controller;

    // Keep track of previous sample data and the total rotation in encoder values
    uint16_t sample_previous[2] {};
    int total_rotation[2] {};

    // 20 sim model variables
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

#endif /* XRF2_HRT_test_HPP */