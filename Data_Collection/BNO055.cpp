#include <iostream>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <cmath>
#include <inttypes.h>
#include <stdlib.h>
#include "BNO055.h"

BNO055::BNO055() // Funtion to Primarily Init the operation and power registers
{
    I2C_FD = wiringPiI2CSetup(BNO055_ADDR);
    if (I2C_FD == -1)
    {
        std::cout << "Device Not Found!";
        return;
    }
    wiringPiI2CWriteReg8(I2C_FD, POWER_MODE, POWER_NORMAL); // Power Mode init
    wiringPiI2CWriteReg8(I2C_FD, OPERATION_MODE, OPR_NDOF); // Operation Mode Register
    delay(100);
    update();
}

vector BNO055::read_acc() // Read the Accelerometer and Scale
{
    acc.x = (int16_t)wiringPiI2CReadReg16(I2C_FD, REG_ACC_X) / 100.00; // m/s^2
    acc.y = (int16_t)wiringPiI2CReadReg16(I2C_FD, REG_ACC_Y) / 100.00; // m/s^2
    acc.z = (int16_t)wiringPiI2CReadReg16(I2C_FD, REG_ACC_Z) / 100.00; // m/s^2
    return acc;
}

vector BNO055::read_mag() // Read the Magenetometers and Scale
{
    mag.x = (int16_t)wiringPiI2CReadReg16(I2C_FD, REG_MAG_X) / 16.00; // mT
    mag.y = (int16_t)wiringPiI2CReadReg16(I2C_FD, REG_MAG_Y) / 16.00; // mT
    mag.z = (int16_t)wiringPiI2CReadReg16(I2C_FD, REG_MAG_Z) / 16.00; // mT
    return mag;
}

vector BNO055::read_gyro() // Read the Gyroscopes and Scale
{
    gyro.x = (int16_t)wiringPiI2CReadReg16(I2C_FD, REG_GYRO_X) / 16.00; // Degrees/Sec
    gyro.y = (int16_t)wiringPiI2CReadReg16(I2C_FD, REG_GYRO_Y) / 16.00; // Degrees/Sec
    gyro.z = (int16_t)wiringPiI2CReadReg16(I2C_FD, REG_GYRO_Z) / 16.00; // Degrees/Sec
    return gyro;
}

vector BNO055::read_euler() // Read the Euler Angles and Scale
{
    euler.z = (int16_t)wiringPiI2CReadReg16(I2C_FD, YAW_REG)   / 16.00;   // Degrees
    euler.y = (int16_t)wiringPiI2CReadReg16(I2C_FD, ROLL_REG)  / 16.00;  // Degrees
    euler.x = (int16_t)wiringPiI2CReadReg16(I2C_FD, PITCH_REG) / 16.00; // Degrees
    return euler;
}

vector BNO055::read_euler2() // Return the Euler Angles Calculated From the Quaternions
{
    read_quat(true);
    return euler2;
}

four_ple BNO055::read_quat(bool euler_update) // Read Quaternions
{
    quat.q0 = (int16_t)wiringPiI2CReadReg16(I2C_FD, Q0_REG) / (pow(2, 14));
    quat.q1 = (int16_t)wiringPiI2CReadReg16(I2C_FD, Q1_REG) / (pow(2, 14));
    quat.q2 = (int16_t)wiringPiI2CReadReg16(I2C_FD, Q2_REG) / (pow(2, 14));
    quat.q3 = (int16_t)wiringPiI2CReadReg16(I2C_FD, Q3_REG) / (pow(2, 14));

    if (euler_update) // Calculate Euler Angles due to Quaternions
    {
        euler2.z = (atan2(2 * (quat.q0 * quat.q3 + quat.q1 * quat.q2), 1 - 2 * (pow(quat.q2, 2) + pow(quat.q3, 2)))) * 180 / PI;
        euler2.y = (asin(2 * (quat.q0 * quat.q2 - quat.q3 * quat.q1))) * 180 / PI;
        euler2.x = (atan2(2 * (quat.q0 * quat.q1 + quat.q2 * quat.q3), 1 - 2 * (pow(quat.q1, 2) + pow(quat.q2, 2)))) * 180 / PI;
    }
    return quat;
}

vector BNO055::read_lin_acc() // Read Linear Acceleration
{
    lin_acc.x = (int16_t)wiringPiI2CReadReg16(I2C_FD, Q0_REG) / 100.00; // m/s^2
    lin_acc.y = (int16_t)wiringPiI2CReadReg16(I2C_FD, Q0_REG) / 100.00; // m/s^2
    lin_acc.z = (int16_t)wiringPiI2CReadReg16(I2C_FD, Q0_REG) / 100.00; // m/s^2
    return lin_acc;
}

vector BNO055::read_grv_acc() // Read Linear Acceleration
{
    grv_acc.x = (int16_t)wiringPiI2CReadReg16(I2C_FD, Q0_REG) / 100.00; // m/s^2
    grv_acc.y = (int16_t)wiringPiI2CReadReg16(I2C_FD, Q0_REG) / 100.00; // m/s^2
    grv_acc.z = (int16_t)wiringPiI2CReadReg16(I2C_FD, Q0_REG) / 100.00; // m/s^2
    return grv_acc;
}

void BNO055::update() // Update All Measurements
{
    read_acc();
    read_mag();
    read_gyro();
    read_quat(true);
    read_lin_acc();
    read_euler();
    read_grv_acc();
}

float BNO055::read_angle(int type)
{
    /* type =
    1: Roll
    2: Pitch
    3: Yaw
    */

    vector angles = read_euler2();
    float angle;
    switch (type)
    {
    case 1:
        angle = angles.x;
        break;

    case 2:
        angle = angles.y;
        break;

    case 3:
        angle = angles.z;
        break;
    }
    return angle;
}

void BNO055::print_state() // Print All Read States
{
    update();
    caliberation_stat();
    system("clear");
    std::cout << "\n=============================== Properties =================================\n";

    std::cout << "\n*************************** Caliberation Status ****************************\n\n";
    std::cout << "\tNotice that: 3 Indicates Fully Calibrated; 0 Indicates not Calibrated\n\n";
    std::cout << " Gyro : " << cal_gyro << "\t| Acc : " << cal_acc << "\t| Magnetometer : " << cal_mag << "\n";
    std::cout << "****************************************************************************\n\n";

    std::cout << " Pitch = " << euler.x << " °\n";
    std::cout << " Roll = " << euler.y << " °\n";
    std::cout << " Yaw = " << euler.z << " °\n";
    std::cout << " Pitch 2 = " << euler2.x << " °\n";
    std::cout << " Roll 2 = " << euler2.y << " °\n";
    std::cout << " Yaw 2 = " << euler2.z << " °\n";
    std::cout << " Acc Y = " << acc.x << " m/s^2\n";
    std::cout << " Acc X = " << acc.y << " m/s^2\n";
    std::cout << " Acc Z = " << acc.z << " m/s^2\n";
    std::cout << " Mag X = " << mag.x << " mT\n";
    std::cout << " Mag Y = " << mag.y << " mT\n";
    std::cout << " Mag Z = " << mag.z << " mT\n";
    std::cout << " Gyro X = " << gyro.x << " °/s\n";
    std::cout << " Gyro Y = " << gyro.y << " °/s\n";
    std::cout << " Gyro Z = " << gyro.z << " °/s\n";
    std::cout << " Linear Acceleration X = " << lin_acc.x << " m/s^2\n";
    std::cout << " Linear Acceleration Y = " << lin_acc.y << " m/s^2\n";
    std::cout << " Linear Acceleration Z = " << lin_acc.z << " m/s^2\n";
    std::cout << " Gravitational Acceleration X = " << grv_acc.x << " m/s^2\n";
    std::cout << " Gravitational Acceleration Y = " << grv_acc.y << " m/s^2\n";
    std::cout << " Gravitational Acceleration Z = " << grv_acc.z << " m/s^2\n";
    std::cout << "\n=======================================================================\n";
}

int BNO055::caliberation_stat() // Get the Caliberation Status and Divide each Status of Each Sensor Pack
{
    int calib_stat = wiringPiI2CReadReg8(I2C_FD, CALIB_STAT);
    cal_total = ((calib_stat & 0b11000000) >> 6);
    cal_gyro = ((calib_stat & 0b00110000) >> 4);
    cal_acc = ((calib_stat & 0b00001100) >> 2);
    cal_mag = (calib_stat & 0b0000011);
    return calib_stat;
}