#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor
{
private:
	int in1;
	int in2;
	int en;

	void forward()
	{
		digitalWrite(in1, HIGH);
		digitalWrite(in2, LOW);
	}
	void backward()
	{
		digitalWrite(in1, LOW);
		digitalWrite(in2, HIGH);
	}
public:
	Motor(int EN, int IN1, int IN2)
	{
		en = EN;
		in1 = IN1;
		in2 = IN2;
		pinMode(en, OUTPUT);
		pinMode(in1, OUTPUT);
		pinMode(in2, OUTPUT);
	}

	void setSpeed(int speed)
	{
		if(speed >= 0)
		{
			if(speed > 255)
				speed = 255;

			forward();
			analogWrite(en, speed);
		}
		if(speed < 0)
		{
			if(speed < -255)
				speed = -255;
			backward();
			analogWrite(en, -speed);
		}
	}
};

#endif

