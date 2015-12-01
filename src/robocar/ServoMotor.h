#include <Servo.h>
#include <Arduino.h>

class ServoMotor:private Servo
{
private:
    int pin;
public:
	ServoMotor(int Pin):Servo()
	{
        pin = Pin;
	}

    void init()
    {
        attach(pin);
    }
	void lookLeft()
	{
        write(150);
	}

	void lookRight()
	{
        write(30);
    }
        
	void peekLeft()
    {
        write(105);
    }

    void peekRight()
    {
        write(75);
    }
	void lookCenter()
	{
	    write(90);
	}
};

