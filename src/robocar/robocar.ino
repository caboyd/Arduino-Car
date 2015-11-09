#define SENSOR_SERVO 9
#define LEFT_MOTOR 6,13,12
#define RIGHT_MOTOR 5,7,8
#define PING_SENSOR 4,2,200
#define TOO_CLOSE 20

//Servo
#include <Servo.h>
#include "ServoMotor.h"


//DC Motor class
#include "Motor.h"

//Ultrasonic sensor class
#include "PingSensor.h"
//ultrasonic sensor library
#include "NewPing.h"



class RobotCar
{
private:
    ServoMotor sensorServo;
    Motor leftMotor;
    Motor rightMotor;
    PingSensor pingSensor;
    unsigned long endTime;
    enum enumstate {stateStopped, stateMoving, stateTurning};
    enumstate state;
    bool looking;
public:
    RobotCar() : sensorServo(SENSOR_SERVO), leftMotor(LEFT_MOTOR), rightMotor(RIGHT_MOTOR), pingSensor(PING_SENSOR)
    {
        state == stateStopped;
        looking = false;
    }
    void init()
    {
        sensorServo.init();
    }

    void run()
    {
        
        unsigned long currentTime = millis();
        Serial.println(currentTime);
        Serial.print("        ");
        Serial.println(endTime);
        int pingDistance = pingSensor.averageDistances(pingSensor.getPingDistance());
      //  Serial.println(pingDistance);
        // Serial.println(pingDistance);
        if(turning())
        {
            
            if(doneTurning(currentTime))
            {
                Serial.println(state);
                move(255);
            }
        } else if(moving())
        {
            if(pingDistance <= TOO_CLOSE)
            {
                if(turning() == false)
                {
                    stop();
                    bool b = decideLeftOrRight();
                    currentTime = millis();
                    if(b)
                    {
                       // Serial.println(state);
                        turnRight();
                       // Serial.println(state);
                        endTime = currentTime + 1000;
                    }else
                    {
                        turnLeft();
                        
                        endTime = currentTime + 1000;
                    }
                }

            } else if(pingDistance <= TOO_CLOSE * 2)
            {
                move(140);
            } else
            {
                move(255);
            }

        } else if(stopped())
        {
            move(255);
        }
    
    }

    bool decideLeftOrRight()
    {
        sensorServo.lookLeft();
        delay(400);
        int left = pingSensor.getPingDistance();
        sensorServo.lookRight();
        delay(400);
        int right = pingSensor.getPingDistance();
        sensorServo.lookCenter();
        delay(400);
        if(left <= right)
            return 1; //represents turn right
        return 0;//turn left
    }

    
    bool turning()
    {
        return (state == stateTurning);
    }
    bool moving()
    {
        return (state == stateMoving);
    }
    bool stopped()
    {
        return (state == stateStopped);
    }
    bool doneTurning(unsigned long currentTime)
    {
        if(currentTime >= endTime)
        {
            Serial.println(currentTime);
            Serial.println(endTime);
            return true;
        }
        return false;
    }

    void move(int speed)
    {
        state = stateMoving;

        leftMotor.setSpeed(speed);
        rightMotor.setSpeed(speed);
    }

    void turnLeft()
    {
        state = stateTurning;
        leftMotor.setSpeed(-255);
        rightMotor.setSpeed(255);
    }

    void turnRight()
    {
       state = stateTurning;
        leftMotor.setSpeed(255);
        rightMotor.setSpeed(-255); 
    }

    void stop()
    {
        //state = stateStopped;
        leftMotor.setSpeed(0);
        rightMotor.setSpeed(0);
    }
};


RobotCar RobotCar;

void setup()
{
   Serial.begin(9600);
    RobotCar.init();
}

void loop()
{
   RobotCar.run();

 
   
   

}

