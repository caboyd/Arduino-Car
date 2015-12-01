
#include "Adafruit_VC0706.h"
#include <SoftwareSerial.h>   


SoftwareSerial cameraconnection = SoftwareSerial(A5,A4);
SoftwareSerial BTSerial(10,11);

Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);



void setup()
{

	Serial.begin(9600);
    BTSerial.begin(115200);
	Serial.println("VC0706 Camera snapshot test");


	// Try to locate the camera
	if(cam.begin())
	{
		Serial.println("Camera Found:");
	} else
	{
		Serial.println("No camera found?");
		//return;
	}

	// Print out the camera version information (optional)
	char *reply = cam.getVersion();
	if(reply == 0)
	{
		Serial.print("Failed to get version");
	} else
	{
		Serial.println("-----------------");
		Serial.print(reply);
		Serial.println("-----------------");
	}

	// Set the picture size - you can choose one of 640x480, 320x240 or 160x120 
	// Remember that bigger pictures take longer to transmit!
    delay(200);
    Serial.println("Changing image size. Wait .2 second");
	//cam.setImageSize(VC0706_640x480);        // biggest
	cam.setImageSize(VC0706_320x240);        // medium
	//cam.setImageSize(VC0706_160x120);          // small
    delay(200);
	// You can read the size back from the camera (optional, but maybe useful?)
	uint8_t imgsize = cam.getImageSize();
	Serial.print("Image size: ");
	if(imgsize == VC0706_640x480) Serial.println("640x480");
	if(imgsize == VC0706_320x240) Serial.println("320x240");
	if(imgsize == VC0706_160x120) Serial.println("160x120");

    delay(200);
    if(!cam.setCompression(90))
       Serial.println("Failed to set compression");
    else
       Serial.println("Compression set.");
       Serial.print("Compression factor: ");
       Serial.println(cam.getCompression());
    delay(200);
    char *c;
    c = cam.setBaud57600();
	Serial.println("Snap in 2 secs...");
	delay(2000);



}

void loop()
{
	
    if(!cam.takePicture())
        Serial.println("Failed to snap!");
    else
        Serial.println("Picture taken!");
    delay(10);



    // Get the size of the image (frame) taken  
    uint16_t jpglen = cam.frameLength();
    unsigned char jpeglenMSB = jpglen >> 8;
    unsigned char jpeglenLSB = jpglen;
    Serial.print("Length is ");
    Serial.print(jpeglenMSB, HEX);
    Serial.print(jpeglenLSB, HEX);
    //Commands to sync with android
    BTSerial.write(217);
    BTSerial.write(217);
    BTSerial.write(217);
    //Send 16 bit length as 2 bytes
    BTSerial.write(jpeglenMSB);
    BTSerial.write(jpeglenLSB);
    //Command to sync with android
    BTSerial.write(217);
    Serial.println("\nDone sending image size");

    Serial.print("Storing ");
    Serial.print(jpglen, DEC);
    Serial.print(" byte image.");

    int32_t time = millis();
    
    // Read all the data up to # bytes!
    byte wCount = 0; // For counting # of writes
    while(jpglen > 0)
    {
        // read 32 bytes at a time;
        uint8_t *buffer;
        uint8_t bytesToRead = min(128, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
        buffer = cam.readPicture(bytesToRead);

        
        BTSerial.write(buffer, bytesToRead);
      /*  for(int i = 0; i < bytesToRead; i++)
        {
            Serial.print(buffer[i], HEX);
        }*/
        if(++wCount >= 11)
        { // Every 2K, give a little feedback so it doesn't appear locked up
         Serial.print('.');
            wCount = 0;
        }
        //Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");
        jpglen -= bytesToRead;
    }
    //End of File character
    //BTSerial.write('\n');
    //BTSerial.write('\r');
    
    time = millis() - time;
    Serial.println("done!");
    Serial.print(time); Serial.println(" ms elapsed");
    
    if(!cam.resumeVideo())
        Serial.println("Failed to resume video!");
    else
        Serial.println("Video Resumed");
    delay(10);
}
