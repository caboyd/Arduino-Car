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
        write(155);
	}

	void lookRight()
	{
        write(40);
    }
        
	
	void lookCenter()
	{
	    write(93);
	}
};
