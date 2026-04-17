#include "XRF2_FRT_test.hpp"
#include <cmath>

/**
 * @brief Constructor for the XRF2_FRT_test class.
 * 
 *  Sets up the XRF2_FRT_test node and initializes logger and
 *  controller. Sets controller to have no end time.
 *
 */
XRF2_FRT_test::XRF2_FRT_test(uint write_decimator_freq, uint monitor_freq) :
    XenoFrt20Sim(write_decimator_freq, monitor_freq, file, &data_to_be_logged),
    file(1,"./xrf2_logging/assignment3_2","bin"), // change template to your project name
    controller()
{
    printf("%s: Constructing rampio\n", __FUNCTION__);

    // Add variables to logger to be logged
    logger.addVariable("channel1", integer);
    logger.addVariable("channel2", integer);
    logger.addVariable("rotation1", integer);
    logger.addVariable("rotation2", integer);
    
    // To infinite run the controller
    controller.SetFinishTime(0.0);
}

/**
 * @brief Destructor. No functionality.
 * 
 * No functionality implemented.
 *
 */
XRF2_FRT_test::~XRF2_FRT_test()
{
    
}

/**
 * @brief Function to unwrap encoder values
 * 
 *  Calculates true encoder values from received data.
 *  Needed to overcome the jumps between 0 and 2^16 when
 *  doing a full rotation.
 */
int XRF2_FRT_test::unwrap(int data, int previous) 
{
    int delta = data - previous;

    if (delta > HALF_MAX) {
        // Positive flip
        delta = previous + (MAX_COUNT - data);
    } else if (delta < -HALF_MAX) {
        // Negative flip
        delta = -(data + (MAX_COUNT - previous));
    }
    return delta;
}   

/**
 * @brief Initializes the FRT system
 * 
 *  Initializes logger and ico.io. Also enables
 *  PWM ports on the FPGA through actuate_data.
 *  Tracks the current channel data from the encoder
 *  To establish baselin first reading. 
 *
 */
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

    sample_previous[0] = sample_data.channel1;
    sample_previous[1] = sample_data.channel2;

    return 1;
}

/**
 * @brief Initialized state in FSM.
 * 
 *  Currently has no functionality.
 */
int XRF2_FRT_test::initialised()
{
    // Keep the physical syste in a state to be used in the run state
    // Call start() or return 1 to go to run state

    evl_printf("Hello from initialised\n");       // Do something

    return 0;
}

/**
 * @brief Main loop in the FSM. Updates current wheel angle and applies control
 * 
 *  Starts logger, logs encoders and total rotation of the wheels.
 *  Unwraps encoder data, integrates this data into wheel angle and applies control.
 *  Then sends out control to FRT through actuator_data. 
 *
 */
int XRF2_FRT_test::run()
{
    // Do what you need to do
    // Return 1 to go to stopping state

    // Start logger
    logger.start();                             
    monitor.printf("Hello from run\n");

    // Left wheel  = channel2, forward = positive
    // Right wheel = channel1, forward = negative 
    int delta_left = this->unwrap(sample_data.channel1, sample_previous[0]);
    int delta_right = this->unwrap(-sample_data.channel2, -sample_previous[1]);

    this->total_rotation[0] += delta_left;
    this->total_rotation[1] += delta_right;

    sample_previous[0] = sample_data.channel1;
    sample_previous[1] = sample_data.channel2;

    u[0] = total_rotation[0] * this->counts_to_rad;     // PosLeft
    u[1] = total_rotation[1] * this->counts_to_rad;     // PosRight
    u[2] = ros_msg.left_wheel_vel;                      // SetVelLeft  [rad/s]
    u[3] = ros_msg.right_wheel_vel;                     // SetVelRight [rad/s]

    // Running controller
    controller.Calculate(u, y);
 
    // Updating PWM values
    actuate_data.pwm1 = -static_cast<int>(y[1]);   // right motor (channel1)
    actuate_data.pwm2 = static_cast<int>(y[0]);   // left  motor (channel2)
    actuate_data.val1 = true;
    actuate_data.val2 = true;

    // Adding encoder data and wheel angles to logger, and logging
    data_to_be_logged.channel1 = sample_data.channel1;
    data_to_be_logged.channel2 = sample_data.channel2;
    data_to_be_logged.rotation1 = total_rotation[0];
    data_to_be_logged.rotation2 = total_rotation[1];
    logger.log();

    return 0;
}

/**
 * @brief Stops the FSM safely
 * 
 *  Updates all wheel values to 0, so nothing is moving anymore
 *  and stops the logger. 
 * 
 */
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

/**
 * @brief Stopped state in FSM. No functionality
 * 
 * No functionality implemented.
 *
 */
int XRF2_FRT_test::stopped()
{
    // A steady state in which the system can be deactivated whitout harming the physical system

    monitor.printf("Hello from stopping\n");          // Do something

    return 0;
}

/**
 * @brief Pausing state in FSM. No functionality
 * 
 * No functionality implemented.
 *
 */
int XRF2_FRT_test::pausing()
{
    // Bring the physical system to a stop as fast as possible without causing harm to the physical system

    evl_printf("Hello from pausing\n");           // Do something
    return 1 ;
}

/**
 * @brief Paused state in FSM. No functionality
 * 
 * No functionality implemented.
 *
 */
int XRF2_FRT_test::paused()
{
    // Keep the physical system in the current physical state

    monitor.printf("Hello from paused\n");            // Do something
    return 0;
}

/**
 * @brief Error state in FSM. No functionality
 * 
 * No functionality implemented.
 *
 */
int XRF2_FRT_test::error()
{
    // Error detected in the system 
    // Can go to error if the previous state returns 1 from every other state function but initialising 

    monitor.printf("Hello from error\n");             // Do something

    return 0;
}
