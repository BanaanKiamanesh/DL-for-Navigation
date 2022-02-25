#ifndef _BNO055_H
#define _BNO055_h

#define PI 3.1415926535897932384626433832795

// Main I2C Connection Addr
#define BNO055_ADDR 0x29

// Power Mode Addrs
#define POWER_MODE 0x3E
#define POWER_NORMAL 0x00
#define POWER_LOW_POWER 0x01
#define POWER_SUSPEND 0x02

// Operation Mode Addrs
#define OPERATION_MODE 0x3D
#define OPR_NDOF 0x0C
#define OPR_IMU 0x08
#define OPR_NDOF_FMC_OFF 0x0B

// Accelerometer Registers
#define REG_ACC_X 0x08
#define REG_ACC_Y 0x0A
#define REG_ACC_Z 0x0C

// Magnetometer Registers
#define REG_MAG_X 0x0E
#define REG_MAG_Y 0x10
#define REG_MAG_Z 0x12

// Gyroscope Registers
#define REG_GYRO_X 0x14
#define REG_GYRO_Y 0x16
#define REG_GYRO_Z 0x18

// Euler Angles Registers
#define YAW_REG 0x1A
#define ROLL_REG 0x1C
#define PITCH_REG 0x1E

// Quaternions Registers
#define Q0_REG 0x20
#define Q1_REG 0x22
#define Q2_REG 0x24
#define Q3_REG 0x26

// Linear Acceleration Registers
#define LIN_ACC_X 0x28
#define LIN_ACC_Y 0x2A
#define LIN_ACC_Z 0x2C

// Gravitational Acceleration Registers
#define GRV_ACC_X 0x2E
#define GRV_ACC_Y 0x30
#define GRV_ACC_Z 0x32

// Temperature Registers
#define TMP_REG 0x34

// Caliberation Status Register
#define CALIB_STAT 0x35

typedef struct
{
    float x;
    float y;
    float z;
} vector;

typedef struct
{
    float q0;
    float q2;
    float q1;
    float q3;
} four_ple;

class BNO055
{
private:
    vector euler, acc, mag, gyro, euler2, lin_acc, grv_acc;
    four_ple quat;
    int I2C_FD;
    int cal_total, cal_gyro, cal_acc, cal_mag;

public:
    BNO055();
    vector read_acc();
    vector read_mag();
    vector read_gyro();
    four_ple read_quat(bool);
    vector read_lin_acc();
    vector read_euler();
    float read_angle(int);
    vector read_grv_acc();
    vector read_euler2();
    void print_state();
    int caliberation_stat();
    void update();
};
#endif