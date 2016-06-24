#include <Wire.h>
//Add the SPI library so we can communicate with the ADXL345 sensor
#include <SPI.h>
#include<EEPROM.h>
//Assign the Chip Select signal to pin 10.
int CS=10;
int thresholdx,thresholdy,thresholdz;
int flag =0;
int steps=0;
//float distance = 0;
//This is a list of some of the registers available on the ADXL345.
//To learn more about these and the rest of the registers on the ADXL345, read the datasheet!
char POWER_CTL = 0x2D;	//Power Control Register--- Link bit is set... auto sleep is disabled... Measure bit is set,i,e. measurement mode.... sleep is enabled,i.e, only activity can be performed... 4Hz wake up bits is given to control the frequency of readings in sleep mode
char DATA_FORMAT = 0x31;//self test disabled.... 0 = 4 wire SPI mode... 1 = interrupt is low... || 0 = Full res-10 bit output.. 0 = right justification with sign extension.. 01 = 4g
char OFZ = 0x20;
char DATAX0 = 0x32;	//X-Axis Data 0
char DATAX1 = 0x33;	//X-Axis Data 1
char DATAY0 = 0x34;	//Y-Axis Data 0
char DATAY1 = 0x35;	//Y-Axis Data 1
char DATAZ0 = 0x36;	//Z-Axis Data 0
char DATAZ1 = 0x37;	//Z-Axis Data 1
char BW_RATE = 0x2C;// by dfault.. bandwith is 50hx.. code : 0x0A 

//This buffer will hold values read from the ADXL345 registers.
byte values[10]; // pawan edit - earlier it was only char
//These variables will be used to hold the x,y and z axis accelerometer values.
int x[100],y[100],z[100];
unsigned long previousMillis = 0;
  const long interval = 10000;

void setup(){
 Wire.begin(5); 
 SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  Serial.begin(9600);
  pinMode(2,INPUT);
  steps=EEPROM.read(10);
  //attachInterrupt(digitalPinToInterrupt(2),clear_eeprom, LOW);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  writeRegister(DATA_FORMAT, 0x02); // 00 = 2g range; 01= 4g range; 02 = 8g range; 03 = 16 g range
  writeRegister(POWER_CTL, 0x08);  //Measurement mode  , normM MODE.. SLEEP is deactivATED  
  writeRegister(BW_RATE, 0x09); // 50 hz output
  calibrate(thresholdx,thresholdy,thresholdz);
}

void loop(){
  int a,b,c;
  unsigned long currentMillis = millis();
  
  if ( currentMillis-previousMillis >= interval)
  {
  previousMillis = currentMillis;
  
  calibrate(thresholdx,thresholdy,thresholdz);
  }
  
  walking(a,b,c);
  
  if((a>thresholdx)&& (b>thresholdy)&&(c>thresholdz)&&flag==0)
  {
    steps = steps+1;
  //  distance = (steps*0.45);
    Serial.print("steps = ");
    Serial.println(steps);
  //  Serial.print("distance = ");
  //  Serial.println(distance);
    flag=1;
  }
  if((a<thresholdx)&&(b<thresholdy)&&(c>thresholdz)&&flag==1)
  {
    steps = steps+1;
   // distance = (steps*0.45);
    Serial.print("steps = ");
    Serial.println(steps);
  //  Serial.print("distance = ");
  //  Serial.println(distance);
    flag=0;
  }
 
  
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void writeRegister(char registerAddress, char value)
{
  digitalWrite(CS, LOW);
  SPI.transfer(registerAddress);
  SPI.transfer(value);
  digitalWrite(CS, HIGH);
}
void readRegister(char registerAddress, int numBytes, byte * values){
  char address = 0x80 | registerAddress;
    if(numBytes > 1)address = address | 0x40;
  digitalWrite(CS, LOW);
  SPI.transfer(address);
  for(int i=0; i<numBytes; i++){
    values[i] = SPI.transfer(0x00);
  }
  digitalWrite(CS, HIGH);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void calibrate(int &threshx, int &threshy, int &threshz)
{
  int j;
    byte valuesx[10];
   byte valuesy[10];
   byte valuesz[10];
   
    
  for ( j=0;j<100;j++)
  {
  readRegister(DATAX0, 2, valuesx);
  x[j] = (valuesx[1]<<8)|valuesx[0];
 
  readRegister(DATAY0, 2, valuesy);
  y[j] = (valuesy[1]<<8)|valuesy[0];

  readRegister(DATAZ0, 2, valuesz);
  z[j] = (valuesz[1]<<8)|valuesz[0];
  
  delay(20);
  
  }
   
  sort(x,100);
 threshx = (x[99]+x[0])/2;
 
   
  
  //-----------------------------------------thresh y ---------------------------------------
  
  sort(y,100);
  threshy = (y[99]+y[0])/2;
 
  
  // ------------------------------- thresh z ---------------------------
 
  sort(z,100);
  threshz = (z[99]+z[0])/2;
  
 //-----------------------------------------------
  Serial.println("10 Seconds Over.. Setting new calibration point");
  Wire.beginTransmission(5);
  Wire.write(steps);
  Wire.endTransmission();
   delay(20);
  EEPROM.write(10,steps);
//  Wire.write(EEPROM.read(10));

 
 }
 
void sort(int a[], int size) 
{
    for(int i=0; i<(size-1); i++)
    {
        for(int o=0; o<(size-(i+1)); o++)
        {
                if(a[o] > a[o+1])
                {
                    int t = a[o];
                    a[o] = a[o+1];
                    a[o+1] = t;
                }
        }
    }
}

//------------------------------------------------------ walking ---------------------------------------------------------------
void walking (int &a, int &b, int &c )
{
  byte values[10]; 
  readRegister(DATAX0, 6, values);
  a = (values[1]<<8)|values[0];
   b = (values[3]<<8)|values[2];
  c = (values[5]<<8)|values[4];
  delay(20); 
  
}

//----------------------wire receive-----------------


void clear_eeprom()
{
  EEPROM.write(10,0);
  Serial.println("EEPROM cleared for new day");
}

