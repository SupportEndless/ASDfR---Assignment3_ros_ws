#ifndef XRF2_HRT_test_HPP
#define XRF2_HRT_test_HPP

#include "XenoFrt20Sim.hpp"
#include "LoopController.h"

//      
#pragma pack (1)    //https://carlosvin.github.io/langs/en/posts/cpp-pragma-pack/#_performance_test
struct HRT_log
{
    // Put logger data here
    int channel1
    int channel2
    int channel3
    int channel4
};

#pragma pack(0)

class XRF2_FRT_test : public XenoFrt20Sim
{
public:
    XRF2_FRT_test(uint write_decimator_freq, uint monitor_freq);
    ~XRF2_FRT_test();
private:
    XenoFileHandler file;
    struct HRT_log data_to_be_logged;
    LoopController controller;

    double u[4];
    double y[2];
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