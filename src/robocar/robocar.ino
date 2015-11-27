
#include <SoftwareSerial.h>

//Servo
#include <Servo.h>
#include "ServoMotor.h"


//DC Motor class
#include "Motor.h"

//Ultrasonic sensor class
#include "PingSensor.h"
//ultrasonic sensor library
#include "NewPing.h"

//Bluetooth device class
#include "BTDevice.h"



//Definitions------
//Servo
#define SENSOR_SERVO 9
//Motors
#define LEFT_MOTOR 6,13,12
#define RIGHT_MOTOR 5,7,8
//Sensor
#define PING_SENSOR 4,2,200
#define TOO_CLOSE 16
//Bluetooth
#define BT_DEVICE 10,11




class RobotCar
{
private:
    ServoMotor sensorServo;
    Motor leftMotor;
    Motor rightMotor;
    PingSensor pingSensor;
    BTDevice bTDevice;
    unsigned long endTime;
    enum enumstate {stateStopped, stateMoving, stateTurning};
    enumstate state;
    unsigned long currentTime;
    int pingDistance;
    BTCommand::Command command;
    BTCommand::Command lastCommand;
    bool commandReceived;
    bool autoToggled;
public:
    RobotCar() : sensorServo(SENSOR_SERVO), leftMotor(LEFT_MOTOR), rightMotor(RIGHT_MOTOR), pingSensor(PING_SENSOR), bTDevice(BT_DEVICE)
    {
        state == stateStopped;
        autoToggled = false;
        currentTime = 0;
        endTime = 0;
        commandReceived = false;
    }
    void init()
    {
        sensorServo.init();
    }

    void run()
    {
        
        currentTime = millis();
      //  Serial.println(currentTime);
       // Serial.print("        ");
      //  Serial.println(endTime);
        pingDistance = pingSensor.averageDistances(pingSensor.getPingDistance());

        commandReceived = bTDevice.getCommand(command);

        if(commandReceived && command == BTCommand::AutoToggle)
        {
            state = stateStopped;
            autoToggled = !autoToggled;
            lastCommand = BTCommand::AutoToggle;
        }
        //Serial.println(autoToggled);
        if(autoToggled)
        {
            autoRun();
        }else
        {
            if(pingDistance <= TOO_CLOSE && lastCommand == BTCommand::Forward)
            {
                command = BTCommand::Stop;
                stop();
            }   
            if(commandReceived)
            {
                lastCommand = command;
                Serial.println(command);
                switch(command)
                {
                    case BTCommand::AutoToggle:
                        stop();
                        break;
                    case BTCommand::Forward:
                        move(255);
                        break;
                    case BTCommand::Reverse:
                        move(-255);
                        break;
                    case BTCommand::TurnLeft:
                        turnLeft();
                        break;
                    case BTCommand::TurnRight:
                        turnRight();
                        break;
                    case BTCommand::Stop:
                        stop();
                        break;
                }
            }
        }
    }

    void autoRun()
    {
        if(turning())
        {
            if(doneTurning())
            {
                move(255);
            }
        } else if(moving())
        {
            
            Serial.println(pingDistance);
          /*  if((currentTime / 150) % 4 == 0 )
            {
                sensorServo.peekLeft();              
            }
            else if ((currentTime / 150) % 4 == 2 )
            {
                sensorServo.peekRight();
            }else
            {
                sensorServo.lookCenter();
            }
            */
            if(pingDistance <= TOO_CLOSE)
            {
                if(turning() == false)//REMOVE THIS
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
        delay(20);
        sensorServo.lookLeft();
        delay(350);
        int left = pingSensor.getPingDistance();
        sensorServo.lookRight();
        delay(450);
        int right = pingSensor.getPingDistance();
        sensorServo.lookCenter();
        delay(200);
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
    bool doneTurning()
    {
        if(currentTime >= endTime)
        {

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

