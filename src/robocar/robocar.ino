
//motors left side
const int enA = 6;
const int in1 = 13;
const int in2 = 12;
//motors right side
const int enB = 5; 
const int in3 = 7;
const int in4 = 8;

//ultrasonic sensor library
#include "NewPing.h"
//Ultrasonic sensor pins
const int trigpin = 4; // Arduino pin tied to trigger pin on ping sensor.
const int echopin = 2; // Arduino pin tied to echo pin on ping sensor.
const int max_distance = 100; // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
int distance = 25; //Global variable to store ping distance
const int speed = 255;

NewPing sonar(trigpin, echopin, max_distance); // NewPing setup of pins and maximum distance.

unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.

void setup()
 {
  //set motor pins to output
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  Serial.begin(9600);

  pingTimer = millis(); //start now
}

void loop()
{
  
  if (millis() >= pingTimer) 
  {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }

 if(distance <  11)
 {
  turnAround();
 }
 else if(distance >=11)
 {
  forward();
 }

  delay(25);
}

void echoCheck() 
{ // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer()) 
  { // This is how you check to see if the ping was received.
    // Here's where you can add code.
    distance = sonar.ping_result / US_ROUNDTRIP_CM;
    Serial.print("Ping: ");
    Serial.println(distance); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
    Serial.println("cm");
    
  }
  // Don't do anything here!
}

void forward()
{
  //FORWARD
  // turn on motor A
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, speed);// set speed out of possible range 0~255
  // turn on motor B
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, speed);// set speed out of possible range 0~255
 
}

void turnAround()
{
  //REVERSE
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  // set speed to 200 out of possible range 0~255
  analogWrite(enA, 2*speed/3);
  // turn on motor B
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  // set speed to 200 out of possible range 0~255
  analogWrite(enB, 2*speed/3);

  delay(500);

  //TURN AROUND
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(enA, speed);
  // turn on motor B
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  // set speed to 200 out of possible range 0~255
  analogWrite(enB, speed);

  delay(1300);
}

