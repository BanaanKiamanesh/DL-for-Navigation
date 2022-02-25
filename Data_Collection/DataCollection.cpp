#include <iostream>
#include <fstream>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <cmath>
#include "BNO055.h"

int main() 
{
    std::cout << "Init..." << std::endl;

    /*
    ````` BNO055 IMU Initialization `````
    */
    BNO055 imu;
    imu.update();

    /* 
    ````` Loop Properties `````
    */
    const double loop_freq = 100;                        // Control Loop Frequency
    const double loop_time_s = (1 / loop_freq);          // Control Loop Execution Time in Sec
    const double loop_time_us = loop_time_s * 1'000'000; // Control Loop Exec Time in Micro Sec
    unsigned long loop_timer;                            // Loop Timer
    double current_time = 0;                             // Current Time in the Loop According to the Start Time
    loop_timer = micros();                               // Loop Timer Initialization
    const int log_time = 5 * 60;                         // Amount of Time in Seconds to Log Data
    const int loop_count = log_time * loop_freq;         // Total Number of Loops 
    long loop_counter = 0;

    // CSV file prep
    std::cout << "\nCreating the File..." << std::endl;
    std::ofstream out("IMU_Data.csv");                   // Create the File

    // Write the Titles(Optional)
    out << "Gyro_x"     << "," << "Gyro_y"  << "," << "Gyro_z"  << ",";
    out << "Acc_x"      << "," << "Acc_y"   << "," << "Acc_z"   << ",";
    out << "Mag_x"      << "," << "Mag_y"   << "," << "Mag_z"   << ",";
    out << "Euler_x"    << "," << "Euler_y" << "," << "Euler_z" << ",";
    out << "Quat_0"     << "," << "Quat_1"  << "," << "Quat_2"  << ","  << "Quat_3" << "\n";    


    std::cout << "==============================================================\n";
    std::cout << "Starting the Recording Process! Time(us) : " << micros() << " ..." << std::endl;
    unsigned long t0 = micros();
    
    /*
    ````` Main Loop `````
    */
    while (true)
    {   
        if(loop_counter >= loop_count)          // Ending Condition
            break;

        // read data from the sensor
        vector gyro = imu.read_gyro();
        vector acc  = imu.read_acc();
        vector mag  = imu.read_mag();
        vector eu   = imu.read_euler();
        four_ple qt = imu.read_quat(false);

        out << gyro.x   << "," << gyro.y    << "," << gyro.z    << ",";
        out << acc.x    << "," << acc.y     << "," << acc.z     << ",";
        out << mag.x    << "," << mag.y     << "," << mag.z     << ",";
        out << eu.x     << "," << eu.y      << "," << eu.z      << ",";
        out << qt.q0    << "," << qt.q1     << "," << qt.q2     << ","  << qt.q3 << "\n";

        // Loop Time set stuff...
        while (micros() - loop_timer < loop_time_us);   // Add Enough Delay to make the Contol Loop Exec in the Exact Frequency

        loop_timer = micros();                          // Record Starting Time of the Next Iteration
        current_time += (double)loop_time_us / 1000000; // What is the Current Time from the Beginning of the Execution
        loop_counter++;                                 // Count the Iterations
    }

    unsigned long t1 = micros();
    std::cout << "Ending the Recording Process! Time(us) : " << micros() << " ..." << std::endl;
    std::cout << "\tAverage Loop Time: " << (t1-t0)/loop_count << " us" << std::endl;
    std::cout << "==============================================================\n";

    std::cout << "End!...." << std::endl;

    return 0;
}