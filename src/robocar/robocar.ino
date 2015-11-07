#define LEFT_MOTOR 6,13,12
#define RIGHT_MOTOR 5,7,8
#define PING_SENSOR 4,2,200
#define TOO_CLOSE 10

//ultrasonic sensor library
#include "NewPing.h"

#include "Motor.h"
#include "PingSensor.h"
unsigned long pingTimer;


class ArduinoCar
{
private:
  Motor leftMotor;
  Motor rightMotor;
  PingSensor pingSensor;
  unsigned long endTime;
  enum enumstate {stateStopped, stateMoving, stateTurning};
  enumstate state;
public:
  ArduinoCar():leftMotor(LEFT_MOTOR),rightMotor(RIGHT_MOTOR),pingSensor(PING_SENSOR)
  {
    state == stateStopped;
  }
      
int pingDistance;
  void run()
  {
      unsigned long currentTime = millis();
      pingDistance = pingSensor.averageDistances(pingSensor.getPingDistance());
      Serial.println(pingDistance);
     // Serial.println(pingDistance);
      if(turning())
      {
        if(doneTurning(currentTime, pingDistance))
        {
          move(255);
        }
      }
      else if(moving())
      {
        if(pingDistance <= TOO_CLOSE)
        {
          turn();
          endTime = currentTime + 1000;
         // Serial.println(endTime);
        }else if(pingDistance <= TOO_CLOSE*2)
        {
          move(255);
        }
        else
        {
          move(160);
        }

      }
      else if(stopped())
      {
        move(255);
      }
  }

  bool turning(){return (state == stateTurning);}
  bool moving(){return (state == stateMoving);}
  bool stopped(){return (state == stateStopped);}
  bool doneTurning(int currentTime, int distance)
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

  void turn()
  {
    state = stateTurning;
    leftMotor.setSpeed(-255);
    rightMotor.setSpeed(255);
  }
};


ArduinoCar arduinoCar;
 
void setup()
{
    Serial.begin(9600);
    pingTimer = millis(); //start now
}

void loop()
{

arduinoCar.run();
}

