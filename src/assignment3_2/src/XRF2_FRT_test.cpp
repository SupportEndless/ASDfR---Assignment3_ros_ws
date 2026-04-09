#include "XRF2_FRT_test.hpp"

XRF2_FRT_test::XRF2_FRT_test(uint write_decimator_freq, uint monitor_freq) :
    XenoFrt20Sim(write_decimator_freq, monitor_freq, file, &data_to_be_logged),
    file(1,"./xrf2_logging/assignment3_2","bin"), // change template to your project name
    controller()
{
    printf("%s: Constructing rampio\n", __FUNCTION__);

    // Add variables to logger to be logged, has to be done before you can log data
    logger.addVariable("channel1", id_int);
    logger.addVariable("channel2", id_int);
    logger.addVariable("channel3", id_int);
    logger.addVariable("channel4", id_int);
    
    // To infinite run the controller, uncomment line below
    controller.SetFinishTime(0.0);
}

XRF2_FRT_test::~XRF2_FRT_test()
{
    
}

int XRF2_FRT_test::initialising()
{
    // Set physical and cyber system up for use in a 
    // Return 1 to go to initialised state

    evl_printf("Hello from initialising\n");      // Do something

    // The logger has to be initialised at only once
    logger.initialise();
    // The FPGA has to be initialised at least once
    ico_io.init();

    // Setting all output booleans to true
    actuate_data.val1 = true;
    actuate_data.val2 = true;
    actuate_data.val3 = true;
    actuate_data.val4 = true;

    return 1;
}

int XRF2_FRT_test::initialised()
{
    // Keep the physical syste in a state to be used in the run state
    // Call start() or return 1 to go to run state

    evl_printf("Hello from initialised\n");       // Do something

    return 0;
}

int XRF2_FRT_test::run()
{
    // Do what you need to do
    // Return 1 to go to stopping state

    // Start logger
    logger.start();                             
    monitor.printf("Hello from run\n");  
    
    // Setting encoder data
    data_to_be_logged.channel1 = sample_data.channel1;
    data_to_be_logged.channel2 = sample_data.channel2;
    data_to_be_logged.channel3 = sample_data.channel3;
    data_to_be_logged.channel4 = sample_data.channel4;

    

    return 0;
}

int XRF2_FRT_test::stopping()
{
    // Bring the physical system to a stop and set it in a state that the system can be deactivated
    // Return 1 to go to stopped state
    logger.stop();                                // Stop logger
    evl_printf("Hello from stopping\n");          // Do something

    // Halting all motors
    actuate_data.val1 = true;
    actuate_data.val2 = true;
    actuate_data.val3 = true;
    actuate_data.val4 = true;

    actuate_data.pwm1 = 0;
    actuate_data.pwm2 = 0;
    actuate_data.pwm3 = 0;
    actuate_data.pwm4 = 0;

    return 1;
}

int XRF2_FRT_test::stopped()
{
    // A steady state in which the system can be deactivated whitout harming the physical system

    monitor.printf("Hello from stopping\n");          // Do something

    return 0;
}

int XRF2_FRT_test::pausing()
{
    // Bring the physical system to a stop as fast as possible without causing harm to the physical system

    evl_printf("Hello from pausing\n");           // Do something
    return 1 ;
}

int XRF2_FRT_test::paused()
{
    // Keep the physical system in the current physical state

    monitor.printf("Hello from paused\n");            // Do something
    return 0;
}

int XRF2_FRT_test::error()
{
    // Error detected in the system 
    // Can go to error if the previous state returns 1 from every other state function but initialising 

    monitor.printf("Hello from error\n");             // Do something

    return 0;
}
