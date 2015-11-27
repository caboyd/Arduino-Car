#include <SoftwareSerial.h>

SoftwareSerial bTSerial(10,11); // RX, TX

void setup()
{
    bTSerial.begin(38400);
    Serial.begin(9600);
}

void loop()
{
    if(bTSerial.available())
        Serial.write(bTSerial.read());
    if(Serial.available())
        bTSerial.write(bTSerial.read());
}
