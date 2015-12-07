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

//Camera class
#include "Adafruit_VC0706.h"

//Definitions------
//Servo
#define SENSOR_SERVO 11
//Motors
#define LEFT_MOTOR 6,13,12
#define RIGHT_MOTOR 5,7,8
//Sensor
#define PING_SENSOR 4,2,200
#define TOO_CLOSE 16
//Bluetooth
#define BT_DEVICE 0,1,115200
//Camera
#define CAMERA A5,A4
SoftwareSerial cameraconnection =  SoftwareSerial(CAMERA);

class RobotCar
{
  private:
    Adafruit_VC0706 cam;
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
    uint16_t jpglen;
    bool sending;
  public:
    RobotCar() : sensorServo(SENSOR_SERVO), leftMotor(LEFT_MOTOR), rightMotor(RIGHT_MOTOR), pingSensor(PING_SENSOR), bTDevice(BT_DEVICE), cam(&cameraconnection)
    {
      state == stateStopped;
      autoToggled = false;
      currentTime = 0;
      endTime = 0;
      commandReceived = false;
      sending = false;
    }
    void init()
    {
      sensorServo.init();
      // Try to locate the camera
      if (cam.begin())
      {
       // Serial.println("Camera Found:");
      } else
      {
       // Serial.println("No camera found?");
       //return;
      }
      // Print out the camera version information (optional)
      char *reply = cam.getVersion();

      // Set the picture size - you can choose one of 640x480, 320x240 or 160x120
      // Remember that bigger pictures take longer to transmit!
      delay(200);
      //cam.setImageSize(VC0706_640x480);        // biggest
      //cam.setImageSize(VC0706_320x240);        // medium
      cam.setImageSize(VC0706_160x120);          // small
      
      delay(200);
      // You can read the size back from the camera (optional, but maybe useful?)
      uint8_t imgsize = cam.getImageSize();
      //if (imgsize == VC0706_640x480) Serial.println("\n640x480");
      //if (imgsize == VC0706_320x240) Serial.println("\n320x240");
      //if (imgsize == VC0706_160x120) Serial.println("\n160x120");

      delay(200);
      cam.setCompression(90);
      //Serial.println("\nFailed to set compression");

     // Serial.println("\nCompression set.");
      //Serial.println("\nCompression factor: ");
      //Serial.println(cam.getCompression());
      delay(200);
      char *c;
      c = cam.setBaud57600();
     // Serial.println("\nSnap in 2 secs...");
      delay(200);
    }
    
    void run()
    {
      currentTime = millis();
      pingDistance = pingSensor.averageDistances(pingSensor.getPingDistance());
      commandReceived = bTDevice.getCommand(command);
      
      if (commandReceived )
      {
        if (command == BTCommand::AutoToggle)
        {
          state = stateStopped;
          autoToggled = !autoToggled;
          lastCommand = BTCommand::AutoToggle;
        } else if (command == BTCommand::StopAll)
        {
          state = stateStopped;
          autoToggled = false;
          lastCommand = BTCommand::Stop;
          stop();
        }
      }

      if (!sendingPicture())
      {
        cam.takePicture();
        delay(10);

        // Get the size of the image (frame) taken
        jpglen = cam.frameLength();
        //Convert 16bit jpeg length to 2 bytes
        unsigned char jpeglenMSB = jpglen >> 8;
        unsigned char jpeglenLSB = jpglen;

        //Commands to sync with android
        Serial.write(217);  //217 = D9 in ASCII
        Serial.write(217);
        Serial.write(217);
        //Send 16 bit length as 2 bytes
        Serial.write(jpeglenMSB);
        Serial.write(jpeglenLSB);
        //Command to sync with android
        Serial.write(217);
        sendPicture();
      }

      if (sendingPicture())
      {
        uint8_t *buffer;
        uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
        buffer = cam.readPicture(bytesToRead); // Read from camera
        Serial.write(buffer, bytesToRead);  //Write to bluetooth
        jpglen -= bytesToRead; //Decrement by length read from camera

        if (jpglen == 0)
        {
          doneSendingPicture();
          cam.resumeVideo();
        }
      }

      if (autoToggled)
        autoRun();
      else
      {
        if (pingDistance <= TOO_CLOSE && lastCommand == BTCommand::Forward)
        {
          command = BTCommand::Stop;
          stop();
        }
        if (commandReceived)
        {
          lastCommand = command;
          switch (command)
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
      if (turning())
      {
        if (doneTurning())
        {
          move(255);
        }
      } else if (moving())
      {

        if (pingDistance <= TOO_CLOSE)
        {
          stop();
          bool b = decideLeftOrRight();
          currentTime = millis();
          if (b)
          {
            turnRight();
            endTime = currentTime + 1000;
          } else
          {
            turnLeft();
            endTime = currentTime + 1000;
          }
        }else if (pingDistance <= TOO_CLOSE * 2)
          move(140); 
        else
          move(255);

      } else if (stopped())
          move(255);
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
      if (left <= right)
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
      if (currentTime >= endTime)
        return true;
      return false;
    }

    bool sendingPicture()
    {
      return sending;
    }

    void sendPicture()
    {
      sending = true;
    }

    void doneSendingPicture()
    {
      sending = false;
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
      leftMotor.setSpeed(0);
      rightMotor.setSpeed(0);
    }
};

//Create robotcar object
RobotCar RobotCar;

void setup()
{
  //Start Bluetooth serial
   Serial.begin(115200);
  //Initialize robot car for servo and camera
  RobotCar.init();
}

void loop()
{
  RobotCar.run();
}

