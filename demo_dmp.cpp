#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <fstream>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Motor.h"
#include "map.h"
#include <pigpio.h>
#include <thread>
#include <iostream>

using namespace std;

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;

float motorPower = 0;

Motor fMotor (FRONTMOTOR, MOTORP, false);
Motor rMotor (RIGHTMOTOR, MOTORP, false);
Motor lMotor (LEFTMOTOR, MOTORP, true);
Motor bMotor (BACKMOTOR, MOTORP, true);


// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    // initialize device
    printf("Initializing I2C devices...\n");
    mpu.initialize();

    // verify connection
    printf("Testing device connections...\n");
    printf(mpu.testConnection() ? "MPU6050 connection successful\n" : "MPU6050 connection failed\n");

    // load and configure the DMP
    printf("Initializing DMP...\n");
    devStatus = mpu.dmpInitialize();

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        printf("Enabling DMP...\n");
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        //Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        //attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        printf("DMP ready!\n");
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        printf("DMP Initialization failed (code %d)\n", devStatus);
        exit(1);
    }
}


// ================================================================
// ===                     MAIN Gyro Loop                       ===
// ================================================================

void refreshGyro() {
    // if programming failed, don't try to do anything
    if (!dmpReady) return;
    // get current FIFO count
    fifoCount = mpu.getFIFOCount();
    cout << "fifoCount: " << fifoCount << endl;
    if (fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        printf("FIFO overflow!\n");
// otherwise, check for DMP data ready interrupt (this should happen frequently)
	} else if (fifoCount >= 42) {
	    // read a packet from FIFO
	    mpu.getFIFOBytes(fifoBuffer, packetSize);

	    mpu.dmpGetQuaternion(&q, fifoBuffer);
	    mpu.dmpGetGravity(&gravity, &q);
	    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
	    printf("ypr  %7.2f %7.2f %7.2f    ", ypr[0] * 180/M_PI, ypr[1] * 180/M_PI, ypr[2] * 180/M_PI);
	    printf("\n");
            mpu.resetFIFO();
	}
    usleep(10000);
}

void setMotorPower()
{
    fMotor.setData(motorPower, ypr[1], 0, ypr[0], 0);
    rMotor.setData(motorPower, ypr[2], 0, ypr[0], 0);
    lMotor.setData(motorPower, ypr[2], 0, ypr[0], 0);
    bMotor.setData(motorPower, ypr[1], 0, ypr[0], 0);
    //cout << "\nFront:" << fMotor.getSpeed() << "\nRight:" << rMotor.getSpeed() << "\nLeft:" << lMotor.getSpeed() << "\nBack:" << bMotor.getSpeed() << endl;;
}

void getInput()
{
    while(true)
    {
        cout << "Please input the speed:";
        cin >> motorPower;
    }
}

int main()
{
    gpioInitialise();

    setup();
    usleep(100000);

    ifstream pdvalues;

    pdvalues.open("pdvals.txt");
    if(pdvalues.fail())
    {
        exit(2);
    }
    //yaw/pitch p, yaw/pitch d, roll p, roll d
    float ypp,ypd,rp,rd;
	string str;
    pdvalues >> rpp >> rpd, yp, yd;

    cout << rpp << endl << rpd << endl;

    fMotor.pdvals(rpp,rpd,yp,yd);
    rMotor.pdvals(rpp,rpd,yp,yd);
    lMotor.pdvals(rpp,rpd,yp,yd);
    bMotor.pdvals(rpp,rpd,yp,yd);

    thread input (getInput);

    while(true){
      refreshGyro();
	    setMotorPower();
	    cout << fMotor.getSpeed() << " " << rMotor.getSpeed() << " " << lMotor.getSpeed() << " " << bMotor.getSpeed() << endl;
	    usleep(15000);
	}

}
