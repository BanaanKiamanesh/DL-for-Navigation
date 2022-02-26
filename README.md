# Deep Learning for Navigation

#### Implementation of a Deep RNN Model for Navigation in Comparison to Kalman Filters

### This Repo Contains 2 parts

1. **[Dataset](https://github.com/BanaanKiamanesh/DL_for_Navigation/tree/main/DataSet)
2. **[Model]()

## DataSet Collection

#### Data Contains

    1. Gyroscope Data 3DoF
    2. Accelerometer Data 3DoF
    3. Magnetometer Data 3DoF
    4. Euler Angles Data 3DoF
    5. Quaternions Data 4DoF

To collect the data, I made a small setup using a Raspberry pi and BNO055 IMU. So simple but it works wonderfully.

### 1. Hardware Setup

    1. Raspberry pi 4
    2. BNO055 9DoF IMU
    3. Lithium Polymer Battery
    4. Buck Converter
    5. USB-C to USB-A Adapter
    6. Breadboard
    7. Bunch of Jumper Wires
    > **Note:** It should be portable, So the data collection is not constrained to a specific location and angle.

![Best Data Logger in the Whole World!](images/Hardware_Setup.jpeg)

---

### 2. Software Setup

[Here](https://github.com/BanaanKiamanesh/DL_for_Navigation/tree/main/Data_Collection) is the code for the data collection.
First, the code is written in C++. The code contains a loop with a constant frequency of 100Hz. The loop reads the IMU data for 5 minutes and writes it to a ".csv" file at each iteration.

There is a Executable file for the Code in the same folder.
But in case you want to run the code, you need to install the WiringPi library from [here](http://wiringpi.com/download-and-install/).

There is another prerequisite for the code to run.
High speed I2C (400kHz) in the used Raspberry pi 4 have to be anabled.

For the purpose use the following procedure described in the [here](https://www.raspberrypi-spy.co.uk/2018/02/change-raspberry-pi-i2c-bus-speed/).

After handling all, the code is ready to be compiled.

So, Navigate to the folder where the code is and ***run*** the following ***bash*** command:

    $ g++ -o CollectorApp *.cpp -lwiringPi -lm -std=c++11

***Now the executable file is ready to be run like this***:

    $ ./CollectorApp

---

#### **There are 8 Available Files Containing Data in the *"DataSet"* Folder.**

> **Note:** The files having "(No Mag)" in there names, don't contain magnetometer data.
---
