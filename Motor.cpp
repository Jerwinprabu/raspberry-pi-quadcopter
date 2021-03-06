#include "Motor.h"
#include <pigpio.h>

Motor::Motor(int pini)
{
    gpioInitialise();
    pin = pini;
    gpioServo(pin, 1000);
    rpp = 0;
    rpd = 0;
    positive = true;
<<<<<<< HEAD
    rppreviousError = 0;
    ypreviousError = 0;
=======
    rpPreviousError = 0;
    yPreviousError = 0;
>>>>>>> 8fa7781224c20d2980c047b4961f28f6a6313eb2
}

Motor::Motor(int pini, float pi, bool positivein)
{
    gpioInitialise();
    pin = pini;
    gpioServo(pin, 1000);
    rpp = pi;
    rpd = 0;
    positive = positivein;
<<<<<<< HEAD
    rppreviousError = 0;
    ypreviousError = 0;
=======
    rpPreviousError = 0;
    yPreviousError = 0;
>>>>>>> 8fa7781224c20d2980c047b4961f28f6a6313eb2
}

Motor::Motor(int pini, float pi, float di, bool positivein)
{
    gpioInitialise();
    pin = pini;
    gpioServo(pin, 1000);
    rpp = pi;
    rpd = di;
    positive = positivein;
<<<<<<< HEAD
    rppreviousError = 0;
    ypreviousError = 0;
=======
    rpPreviousError = 0;
    yPreviousError = 0;
>>>>>>> 8fa7781224c20d2980c047b4961f28f6a6313eb2
}

//input speed 0 to 100 and convert to 1000usec to 2000usec
void Motor::set(int s)
{
    if(s >= 100)
    {
        speed = 99;
    }
    else if(s <= 0)
    {
        speed = 1;
    }
    else
    {
        speed = s;
    }
    gpioSpeed = (static_cast<int>(speed) + 100) * 10;
    gpioServo(pin, gpioSpeed);
}

void Motor::pSet(float s, float current, float target)
{
    float currentError = error(current, target);
    float pOut = currentError * rpp;
	if(positive)
	{
		speed = s + pOut;
	}
	else
	{
		speed = s - pOut;
	}
    set(speed);
}

void Motor::pdSet(float s)
{
    //
    //roll/Pitch
    //
    rpCurrentError = error(rpCurrent, rpTarget);
    rppOut = rpCurrentError * rpp;
    rpdOut = (rpPreviousError - rpCurrentError) * rpd;



    //
    //yaw
    //

    yCurrentError = error(yCurrent, yTarget);
    ypOut = yCurrentError * yp;
    ydOut = (yPreviousError - yCurrentError) * yd;

    if(positive)
    {
        speed = s + rppOut + rpdOut + ypOut + ydOut;
    }
    else
    {
        speed = s - (rppOut + rpdOut + ypOut + ydOut);
    }

    set(speed);
    rpPreviousError = rpCurrentError;
<<<<<<< HEAD
    yPreviousError = yCurrentError
}

void Motor::pdvals(float rppi, float rpdi, float ypi, float rdi)
{
    rpp = rppi;
    rpd = rppd;
=======
    yPreviousError = yCurrentError;
}

void Motor::pdvals(float rppi, float rpdi, float ypi, float ydi)
{
    rpp = rppi;
    rpd = rppi;
>>>>>>> 8fa7781224c20d2980c047b4961f28f6a6313eb2
    yp = ypi;
    yd = ydi;
}

int Motor::getSpeed()
{
    return speed;
}

float Motor::error(float current, float target)
{
	return current - target;
}

<<<<<<< HEAD
float Motor::setData(float s, float rpCurrenti, float rpTargeti, float yCurrenti, yTargeti)
=======
void Motor::setData(float s, float rpCurrenti, float rpTargeti, float yCurrenti, float yTargeti)
>>>>>>> 8fa7781224c20d2980c047b4961f28f6a6313eb2
{
    rpCurrent = rpCurrenti;
    rpTarget = rpTargeti;
    yCurrent = yCurrenti;
    yTarget = yTargeti;
    pdSet(s);
}
