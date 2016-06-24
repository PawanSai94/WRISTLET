#include <SoftwareSerial.h>
#include <SD.h> //Load SD card library
#include<SPI.h> //Load SPI Library

#include <TinyGPS.h>

/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

TinyGPS gps;


static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);

int chipSelect = 53; //chipSelect pin for the SD card Reader
File mySensorData; //Data object you will write your sesnor data to

void setup()
{
  Serial.begin(9600);
  
  Serial.print("Testing TinyGPS library v. "); Serial.println(TinyGPS::library_version());
  
  Serial.println("-------------------------------------------------------------------------------------------------------------------------------------");
 pinMode(53, OUTPUT); //Must declare 10 an output and reserve it to keep SD card happy
  SD.begin(chipSelect); //Initialize the SD card reader
  Serial3.begin(9600);
  
//  smartdelay(1000);
  
  
}

void loop()
{
//  smartdelay(1);
  float flat, flon;
  int prec;
  gps.f_get_position(&flat, &flon);

if(flat!=1000.00000000 && flon!=1000.00000000)
{
    Serial.print (flat,8);
    Serial.print(' ');
    Serial.print(flon,8);
    Serial.print(' ');
    Serial.println();
     
    
    mySensorData = SD.open("gpstest.txt", FILE_WRITE);  
    mySensorData.print(flat,8); //Write measured latitude to file
    mySensorData.print(",");
    mySensorData.print(flon,8); //Write measured longitude to file
    mySensorData.println();
    mySensorData.close();
    smartdelay(1000);
}
else
{
  Serial.println("no tracking");
  smartdelay(1000);
}
  
  
 // delay(1000);
}

 static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial3.available())
      gps.encode(Serial3.read());
  } while (millis() - start < ms);
}



