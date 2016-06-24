//---------------------------------------------------------------FINAL CODE WRISTLET---------------------------------------------------------------------

//-------------------------------- Header Files------------------------------


#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>
#include <TinyGPS.h>
#include <Wire.h>
#include "RTClib.h"
#include<EEPROM.h>
#include "DHT.h"

//-----------------------------------------------------------------------------------

//--------------------------------------- Global PIN Declarations-------------------------------------------
#define TFT_CS  12  // Chip select line for TFT display
#define TFT_RST  11  // Reset line for TFT (or see below...)
#define TFT_DC   10  // Data/command line for TFT
#define SD_CS    53 // Chip select line for SD card
#define BUFFPIXEL 20
#define TFT_SCLK  8  // set these to be whatever pins you like!
#define TFT_MOSI 9   // set these to be whatever pins you like!

#define DHTPIN 5
#define DHTTYPE DHT22 
#define AlarmIndicator 13

#define ESP8266 Serial2

//--------------------------------------------------------------------------------------------------------------
//----------------------------------user friend------------------------------------

//-----------------------------------------------------------
//---------------------------------- Global Functions----------------------------------------------------------
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
RTC_DS1307 rtc;
DHT dht(DHTPIN, DHTTYPE);
TinyGPS gps;

//-------------------------------------------------------------------------------------------------------------

//-------------------------------------------Global Variables------------------------------------------------------
//------------- Interrupt Variables--------------------
//int pin19=19;
//int pin18=18;
int pin2=2;
int pin3=3;  
volatile byte secs;
//------------------------
//float prevflat=0,prevflon=0;
// ----------------------------- RTC variables---------------
String hours;
String mins;
int minuteNow=0;
int minutePrevious=0;
int hourPrevious = 25;
int hourNow = 0;
int temp,incr,hours1,minut,add=11;
int next=6;    
int INC=7;
int HOUR,MINUT,SECOND;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int prev_day;

//-----------------------------------------------------hr variables---------------------
long hr_count;
volatile int hr_enable=0;



//-----------------------------------

int steps,prev_steps=0;
float total_time;
int time_monitor;
//int password_flag = 0;
int gps_flag;

int gps_count;

int praveen =0;

int wifi_flag;

int eeprom_gender_flag;
int eeprom_mem=15;

//int eeprom_user_age;
//int eeprom_height;
//int eeprom_weight;
//eeprom_stride
int gender_flag;
float eeprom_user_age,eeprom_height,eeprom_weight,eeprom_stride;
//-------------------------GPS Variables--------------------------
static void smartdelay(unsigned long ms);


int chipSelect = 53; //chipSelect pin for the SD card Reader
File myGPSData;
File calories;
File mydist;
//------------------------------------------
float global_distance;
float global_calories;



//-------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------Global ISR variables----------------------------------------------
volatile int pawan = 0;
volatile int suma =0;
//---------------------------------------------------------------------------------------------------------------------

//----------------------ESP variables------------------------

String SSID = "Zenith";
String PASSWORD ="26Ra-5Ba-1As-14Pa";

boolean FAIL_8266 = false;
#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

//-----------------------------------------------------

void setup() 
{
  ESP8266.begin(115200);
  //--------------- Power control Pins-----------------
 // pinMode(6, OUTPUT);//LCD
  pinMode(4,OUTPUT);//DHT
  pinMode(23,OUTPUT);//GPS
  pinMode(40, OUTPUT);// interrupt uno
//  digitalWrite(6,HIGH); //turn ON LCD
  digitalWrite(4,LOW); //turn OFF DHT
  digitalWrite(23,LOW); //turn OFF GPS
  digitalWrite(40,HIGH); // ADXL Interrupt high initially
  
  //-------------------- 
  Serial.begin(9600);
  
  Serial3.begin(9600);//gps
  Wire.begin(5);
 Wire.onReceive(receiveEvent); 

  
 //------------------------------------------------
  pinMode(INC, INPUT);
  pinMode(next, INPUT);
  pinMode(AlarmIndicator, OUTPUT);
  digitalWrite(next, HIGH);
  digitalWrite(INC, HIGH);
  
 
  dht.begin();
  tft.initR(INITR_BLACKTAB);
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) 
  {
    Serial.println("failed!");
    //return;
  }
  Serial.println("OK!");
  uint16_t time = millis();
  
  //----------
  Welcome();
  delay(1000);
  FirstScreen();
  delay(1000);
  CreditScreen();
  delay(1000);
  thanks();
  delay(1000);
  SecondScreen();
  delay(1000);
  
  //--------------
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  time = millis() - time;
  Serial.println(time, DEC);
  delay(500);  
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
 // pinMode(pin18, INPUT);
//  pinMode(pin19, INPUT);
  attachInterrupt(digitalPinToInterrupt(pin2), inc, FALLING);
  attachInterrupt(digitalPinToInterrupt(pin3),functioninside, FALLING);
  //attachInterrupt(digitalPinToInterrupt(pin18),homepage, FALLING);
// attachInterrupt(digitalPinToInterrupt(pin19),Alarm, FALLING);
  tft.initR(INITR_BLACKTAB);
  
  if (! rtc.begin())
  {
    Serial.println("Couldn't find RTC");
   //65 while (1);
  }
  if (! rtc.isrunning())
  {
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
 
 password();
user_input();
  Time_Date();
  Serial.println("main menu dispayed");
  delay(1000);
   //---------------------------Timer Interrup Setup---------
  
 //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();//allow interrupts
  
  //-------------------------close setup timer interrupt
  
  
    
}
unsigned long previousAlarmMillis = 0;
const long alarm_interval = 10000;

unsigned long previousGPSMillis = 0;
const long GPS_interval = 2000;


void loop() {
  //Serial.println(millis());
  unsigned long currentMillis = millis();
  /*  Serial.print("pawan");
    Serial.println(pawan);
    Serial.print("Suma");
    Serial.println(suma);*/
   
  //   if ( currentMillis-previousAlarmMillis >= alarm_interval)
 // {
 // previousAlarmMillis = currentMillis;
    match();
 // }
  if ( currentMillis-previousGPSMillis >= GPS_interval)
  {
  previousGPSMillis = currentMillis;  
   gps_func();
  } 
  
   if(wifi_flag==1)
  {
    
//  ESPSetup();
 ESPFunc();
  Serial.println("Sync COmplete");
  tft.setTextColor(ST7735_RED);
 tft.println("Sync Complete");
  wifi_flag=0;
  }  
   
   if(hr_enable==1)
  {
    Serial.print("pawannn heart rate");
  
    heartrate_count();

    tft.fillScreen(ST7735_BLACK);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.println("If your heart beats at current rate, then your heart rate is");
    tft.setTextSize(5);
    tft.setTextColor(ST7735_RED);
    tft.println(hr_count);
    EEPROM.write(50,hr_count);
    //hr_count=0;
    hr_enable=0;
  } 
}

//---------------------------------------- int1-------------------

void inc() {
  digitalWrite(6,HIGH);
 
  TCNT1  = 0;
  secs = 0;
  pawan = pawan+1;
  switch(pawan)
  {
    
   case(1):
  user_friend();
  suma=0;
  break;
    
  case(2):
  MainMenu();
  suma=0;
  break;
  
 
  
  
  case(3):
  suma=0;
  Health();
  break;
  
  case(4):
  suma=0;
  WeConnect_gps();
  break;
  
  case(5):
  suma=0;
  WeConnect_wifi();
  break;
  
  case(6):
  suma=0;
  WeSet();
  break;
  
  case(7):
  Time_Date();
  suma=0;
  pawan=0;
  
  break;
  
  }
  delayMicroseconds(12000);
}

//-----------------------------int homepage--------------------------

void homepage()

{
  Time_Date();
  pawan=0;
  suma=0; 

}

//----------------------------------function inside function------------------------------------------------------------

void functioninside()
{
  digitalWrite(6,HIGH);
  TCNT1  = 0;
  secs = 0;
  suma = suma+1;
  switch(pawan)
  {
  case(3):
  
switch(suma)
{
  case(1):
  calories_display();
  break;
  case(2):
  BMI();
  break;
  
  case(3):
  heartrate();
  break;
  
  case(4):
  AboutUser();
  break;
  
  case(5):
  Health();
  suma=0;
  break;
}
break;
  
  
  case(4):
  
    switch(suma)
    {
    case(1):
   
   gps_control();
   // suma=0;
    break;
    
   
    
    case(2):
    WeConnect_gps();
    suma=0;
    break;
    }
    break;

case(5):
 switch(suma)
    {
      case(1):
   wifi_control();
  
    break;
      
      case(2):
    WeConnect_wifi();
    suma=0;
    break;
    }
  break;

 case(6):
 switch(suma)
    {
    
      
      case(2):
      Alarm();
      break;
      
      case(1):
      About();
      break;
      
     /* case(3):
      sdlog();
      break;*/
      
      case(3):
      WeSet();
      suma=0;
      break;
    }
  break;
  }
      
}

//-----------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------

void distance_walked()
{
  //Serial.println(EEPROM.read(15));//gender_flag.. 0 = male..1=female
// Serial.println(EEPROM.read(16));//user_age
// Serial.println(EEPROM.read(17));//height
// Serial.println(EEPROM.read(18));//weight
//Serial.println(EEPROM.read(19)); //stride
  float distance;
 
  distance = ((steps * EEPROM.read(19))/100);
 global_distance = distance;
  Serial.println("distance in m");
  Serial.println(distance);
  Serial.println("step");
  Serial.println(steps);
  Serial.println(global_distance);
 

  //delay(1000);
}

//------------------------------------------------------------------------------

void heartrate()

{
  Serial.println("please wait...");
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setCursor(40, 0);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(2);
  tft.println("HEART RATE");
  tft.setTextSize(1);
  
  tft.setTextColor(ST7735_WHITE);
  tft.println("please wait...");
  hr_enable=1;
  
}

//-------------------------------------------------------------

void AboutUser()
{
  int genderflag = EEPROM.read(15);
   //Serial.println(EEPROM.read(15));//gender_flag
// Serial.println(EEPROM.read(16));//user_age
// Serial.println(EEPROM.read(17));//height
// Serial.println(EEPROM.read(18));//weight
//Serial.println(EEPROM.read(19)); //stride
   tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);

  tft.setCursor(50, 0);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(2);
  tft.println("MYDATA");
  tft.setTextSize(2);
  tft.setTextColor(ST7735_YELLOW);
  tft.setCursor(0, 40);
  tft.print("Gender:");
   tft.setTextColor(ST7735_WHITE);
   if(genderflag==0)
   {
  tft.print("Male  ");
   }
   else if(genderflag==1)
   {
     tft.print("Female");
   }
  tft.setTextColor(ST7735_CYAN);
   tft.print("Age   :");
   tft.setTextColor(ST7735_WHITE);
  tft.println(EEPROM.read(16));
  tft.setTextColor(ST7735_MAGENTA);
   tft.print("Height:");
   tft.setTextColor(ST7735_WHITE);
 tft.print(EEPROM.read(17));
 tft.setCursor(130, 72);
 tft.print("cm");
  tft.setTextColor(ST7735_BLUE);
   tft.print("Weight:");
   tft.setTextColor(ST7735_WHITE);
  tft.print(EEPROM.read(18));
  tft.setCursor(130, 89);
 tft.print("kg");
  tft.setTextColor(ST7735_RED);
   tft.print("Stride:");
   tft.setTextColor(ST7735_WHITE);
  tft.print(EEPROM.read(19));
  tft.setCursor(130, 105);
 tft.print("cm");
}

//----------------------------------------------------------------------------------------------------------------

 void Alarm()
 {
   
   tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
     Serial.print("  Set Alarm  ");
     tft.fillScreen(ST7735_BLACK);
     tft.setCursor(0,0);
     tft.setTextColor(ST7735_WHITE);
     tft.setTextSize(2);
     tft.print("Set Alarm:");
     alarm();
     Serial.println("pawannn");
     tft.setCursor(0,80);
     tft.setTextColor(ST7735_BLUE);
     tft.setTextSize(2);
     tft.print("ALARM SET!");
     WeSet();
    }
    
    //----------------------------------------------------------------------
    
    
//----------------------------------------------------------------------------------------------------------------- 
 void Time_Date()
 {
  digitalWrite(4,HIGH); //turn ON DHT
 // digitalWrite(7,HIGH); //turn ON RTC
 // delay(500);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  DateTime now = rtc.now();
   prev_day = now.day();
  //  prev_day = now.minute();
 //   Serial.println("day checking");
 //   Serial.println(now.day());
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    HOUR=now.hour();
  //  Serial.println(HOUR);
    MINUT=now.minute();
  //  Serial.println(MINUT);
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    
    Serial.print(") ");
    if(now.hour()<10)
    {
      hours ="0"+String(now.hour());
    }else
    {
      hours = String(now.hour());
       }
    Serial.print(hours);
    Serial.print(':');
    if(now.minute()<10)
    {
      mins ="0"+String(now.minute());
    }else
    {
      mins = String(now.minute());
       }
    Serial.print(mins);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
   
   // tft.fillRect(15,15,140,65,ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(1);
  tft.println(daysOfTheWeek[now.dayOfTheWeek()]);
  tft.setCursor(105,0);
  tft.print(now.day(), DEC);
  tft.print('/');
  tft.print(now.month(), DEC);
  tft.print('/');
  tft.print(now.year(), DEC);
  tft.setCursor(40, 50);
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(3); 
  tft.print(hours);
  tft.print(':');
  tft.print(mins); 
    
 // tft.fillRect(0,100,160,100,ST7735_BLACK);
  tft.setCursor(0, 90);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.print("Humidity");
  tft.setCursor(0, 100);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(2); 
  tft.print(h);
  tft.print("%");
  
  tft.setCursor(90, 90);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.print("Temp. in *C");
  tft.setCursor(90, 100);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(2); 
  tft.print(t);
  
  digitalWrite(4,LOW);// turn OFF DHT
 // digitalWrite(7,LOW); //turn OFF RTC
  
 }
 
 void About()
 {
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setTextColor(ST7735_YELLOW);
  tft.setCursor(50, 0);
  tft.setTextSize(2);
  tft.println("ABOUT");
  tft.setTextSize(1);
  tft.println("");
   tft.setTextColor(ST7735_RED);
  tft.println("Developed by - ");
  tft.setTextSize(1);
  tft.println("");
  tft.setTextSize(2);
  tft.setTextColor(ST7735_CYAN);
  tft.println("R PAWAN SAI");
  tft.setTextSize(1);
  tft.println("");
  tft.setTextSize(2);
  tft.setTextColor(ST7735_BLUE);
  tft.println("PRAVEEN K");
  tft.setTextColor(ST7735_GREEN);
   tft.setTextSize(1);
  tft.println("");
  tft.setTextSize(2);
  tft.println("SUMA B");
  tft.setTextSize(1);
  tft.setTextColor(ST7735_MAGENTA);
  tft.println("");
  tft.println("For User Manual visit -");
  tft.setTextSize(1);
  tft.setTextColor(ST7735_WHITE);
  tft.println("http://www.wristlet.in");
 }



//------------------------------------------------------------------------------------------------------
void MainMenu()
{
 
   tft.setRotation(1);
   tft.fillScreen(ST7735_BLACK);
   tft.setCursor(20, 0);
   tft.setTextColor(ST7735_WHITE);  tft.setTextSize(2);
   tft.println("MAIN MENU");
   bmpDraw("health.bmp",55,20);
   tft.setCursor(57, 62);
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(1);
  tft.println("Health");
  tft.setCursor(16, 117);
 tft.println("Connect");
   bmpDraw("connect1.bmp",15,76);
   tft.setCursor(98, 117);
   tft.println("Settings");
   bmpDraw("settings.bmp",100,76);
 }

//----------------------------------------------------------------------------------------------------------------------------------------------------
 void  Health() 
{
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setTextColor(ST7735_GREEN);  tft.setTextSize(2);
  tft.setCursor(40,0);
  tft.println("HEALTH");
  tft.setTextColor(ST7735_YELLOW);  tft.setTextSize(1);
  
  tft.setCursor(20,64);
  tft.println("Calory");
  bmpDraw("calori.bmp",20,23);
  tft.setCursor(110,64);
  tft.println(" BMI");
  bmpDraw("BMI.bmp",100,23);
  tft.setCursor(18,116);
  tft.println("HeartRate");
  bmpDraw("HR2.bmp",20,75);
  tft.setCursor(104,116);
  tft.println("About Me");
  bmpDraw("SL.bmp",100,75);
} 
//------------------------------------------------

void WeConnect_gps() {
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setTextColor(ST7735_YELLOW);  tft.setTextSize(2);
  tft.setCursor(10,0);
  tft.println("CONNECTIVITY");
  tft.setTextColor(ST7735_GREEN);  tft.setTextSize(1);

  tft.setCursor(65,80);
  tft.println("GPS");
  bmpDraw("GPS.bmp",61,40);
 
} 
//--------------------------------------------------------
//---------------------------------------------------------
void WeConnect_wifi() {
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setTextColor(ST7735_YELLOW);  tft.setTextSize(2);
  tft.setCursor(10,0);
  tft.println("CONNECTIVITY");
  tft.setTextColor(ST7735_GREEN);  tft.setTextSize(1);
  tft.setCursor(65,80);
  tft.println("WI-FI");
  bmpDraw("wifi.bmp",61,40);

 
} 
//-----------------------------------------------------------
void WeSet() {
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setTextColor(ST7735_BLUE);  tft.setTextSize(2);
  tft.setCursor(30,0);
  tft.println("SETTINGS");
  
  tft.setTextColor(ST7735_RED);  tft.setTextSize(1);
  tft.setCursor(21,67);
  tft.println("About");
  bmpDraw("text.bmp",20,25);
  tft.setCursor(102,67);
  tft.println("Alarm");
  bmpDraw("Alarm1.bmp",100,25);
  
   tft.setCursor(82,117);
  tft.println("SDlog");
  bmpDraw("File.bmp",70,75);
  
} 
//------------------------------------------------------------------------------------------------------

 void bmpDraw(char *filename, uint8_t x, uint8_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');
  
   // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  }
  
   // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);
        
        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

void alarm()                             
{
  int temp=1,minuts=0,hours=0,seconds=0;
    while(temp==1)
    {
     if(digitalRead(INC)==0)
     {
      HOUR++;
      if(HOUR==24)
      {
       HOUR=0;
      }
      while(digitalRead(INC)==0);
     }
     
    Serial.print("\n");
    Serial.print("Set Alarm Time ");
    Serial.print(HOUR);
    Serial.print(":");
    Serial.print(MINUT);
    tft.fillRect(40,40,40,40,ST7735_BLACK);
    tft.setCursor(40,40);
    tft.setTextColor(ST7735_BLUE);
    tft.setTextSize(3);
    tft.print(HOUR);
    tft.print(":");
    tft.print(MINUT);
    Serial.print("\n");
    delay(100);
    if(digitalRead(next)==0)
    {
      hours1=HOUR;
      EEPROM.write(add++,hours1);
      Serial.println("ooola houreee");
      Serial.println(add);
       Serial.println(hours1);
     temp=2;
     while(digitalRead(next)==0);
    }
    }
    
    while(temp==2)
    {
     if(digitalRead(INC)==0)
     {
      MINUT++;
      if(MINUT==60)
      {MINUT=0;}
      while(digitalRead(INC)==0);
     }
     tft.fillRect(93,40,60,40,ST7735_BLACK);
     tft.setCursor(93,40);
     tft.setTextColor(ST7735_BLUE);
     tft.print(MINUT);
     Serial.print("\n");
     Serial.print(HOUR);
     Serial.print(":");
     Serial.print(MINUT);
     Serial.print("\n");
     delay(100);
      if(digitalRead(next)==0)
      {
       minut=MINUT;
       
     
       EEPROM.write(add++, minut);
       Serial.println("ooola mineee");
       Serial.println(add);
         Serial.println(minut);
       temp=0;
       while(digitalRead(next)==0);
      }
    }
delay(1000);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
void match()
{
// Serial.println("matching");
  DateTime now = rtc.now();
//  Serial.println(now.hour());
//  Serial.println(now.minute());
  int tem[17];
  
    
    
   // prev_day = now.minute();
    
  
  for(int i=11;i<13;i++)
  {
    tem[i]=EEPROM.read(i);
  }
//  Serial.println("mem");
//  Serial.println(tem[11]);
//  Serial.println(tem[12]);
  
  
  if(now.hour() == tem[11] && now.minute() == tem[12]) 
  {
   if( praveen==0)
   {
     beep();
     Serial.println("wake up");
      if (digitalRead(next)==0)
 {
   praveen=1;
 }
}  
 }
  else
  {
    praveen=0;
  }
   
}

void beep()
{
   digitalWrite(AlarmIndicator,HIGH);
   delay(500);
   digitalWrite(AlarmIndicator, LOW);
   delay(500);
}

//-----------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void gps_func()
{
  if(gps_flag==1)
  {
  
  float flat, flon;
  
  int prec;
  gps.f_get_position(&flat, &flon);
if(flat!=1000.00000000 && flon!=1000.00000000)
{
//  if(flat!=prevflat || flon != prevflon)
//  {
//  prevflat=flat;
//  prevflon=flon;
    Serial.print (flat,8);
    Serial.print(' ');
    Serial.print(flon,8);
    Serial.print(' ');
    Serial.println();
     
    
    myGPSData = SD.open("gpstest.txt", FILE_WRITE);  
    myGPSData.print(flat,8); //Write measured latitude to file
    myGPSData.print(",");
    myGPSData.print(flon,8); //Write measured longitude to file
    myGPSData.println();
    myGPSData.close();
    smartdelay(1000);
 // }
}
else
{
  Serial.println("Satellites not found. Hence no tracking");
  smartdelay(1000);
}
   // myGPSData = SD.close("gpstest.txt", FILE_WRITE);  
}
}

//-----------------------------------------------------------------------------
void gps_control()
{
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(2);
  
  gps_count++;
  if (gps_count%2!=0)
  {
    digitalWrite(28,HIGH); //turn ON GPS
    tft.println("GPS tracking Enabled");
    tft.println("");
    tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(1);
    tft.println("Please come back to this screen to disable");
    gps_flag=1;
  }
  else
  {
    digitalWrite(28,LOW); //turn OFF GPS
    tft.println("GPS tracking Disabled");
     tft.println("");
     tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(1);
    tft.println("Please come back to this screen to enable");
    gps_flag=0;
  }
}

//-------------------------------------
static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial3.available())
      gps.encode(Serial3.read());
  } while (millis() - start < ms);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void wifi_control()
{
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  Serial.println("syncing");
  delay(500);
  tft.println("syncing");
  digitalWrite(26,HIGH);
  wifi_flag=1;
  Serial.println("wifi flag raised");
 
}

//-------------------------------------
//------------------------------ESP8266 Setup code--------------------

/* void ESPSetup()
{
  int temp=1;
   while(temp==1)
    {
  do {
    Serial.println("--- Start ---");

    ESP8266.println("AT");
    delay(1000);
     if(ESP8266.find("OK"))
    {
      Serial.println("Module is ready");
      
      ESP8266.println("AT+GMR");
      delay(1000);
      clearESP8266SerialBuffer();
      
      ESP8266.println("AT+CWMODE=1");
      //ESP8266.println("AT+CWMODE=3");
      delay(2000);
      
      //Quit existing AP, for demo
      Serial.println("Quit AP");
      ESP8266.println("AT+CWQAP");
      delay(1000);
      
      clearESP8266SerialBuffer();
      if(cwJoinAP())
      {
        Serial.println("CWJAP Success");
        FAIL_8266 = false;
        
        delay(3000);
        clearESP8266SerialBuffer();
        //Get and display my IP
        sendESP8266Cmdln("AT+CIFSR", 1000);  
        //Set multi connections
        sendESP8266Cmdln("AT+CIPMUX=1", 1000);
        //sendESP8266Cmdln("AT+CIPMUX=0", 1000);

        Serial.println("Setup finish");
      }else{
        Serial.println("CWJAP Fail");
       
        Serial.println("Wifi flag closed..please sync again");
        digitalWrite(6,HIGH);
        TCNT1  = 0;
        secs = 0;
        tft.fillScreen(ST7735_BLACK);
        tft.setRotation(1);
        tft.setCursor(0, 0);  
        tft.setTextColor(ST7735_YELLOW);  tft.setTextSize(2);
        tft.println("CWJAP Fail-Wifi flag closed..please sync again");
        delay(500);
        FAIL_8266 = false;
      }
    }else{
      Serial.println("Module have no response.");
      digitalWrite(6,HIGH);
        TCNT1  = 0;
        secs = 0;
        tft.fillScreen(ST7735_BLACK);
        tft.setRotation(1);
        tft.setCursor(0, 0);  
        tft.setTextColor(ST7735_YELLOW);  tft.setTextSize(2);
        tft.println("Module has no response");
      delay(500);
      FAIL_8266 = false;
    }
  }while(FAIL_8266);
  
 // digitalWrite(LED, HIGH);
  
  //set timeout duration ESP8266.readBytesUntil
  ESP8266.setTimeout(1000);
  temp=0;
    }
delay(1000);
} */



//------------------------------------------

//-----------------------------------------------------------------ESP function--------------------------

void ESPFunc()
{
  int temp=1;
  while(temp==1)
  {
  float my_calories;
  float my_distance;
  float my_hrbeat;

    my_calories = global_calories;
    my_distance=global_distance;
    my_hrbeat = EEPROM.read(50);
 
  
  Serial.println("Calories=" + String(my_calories));
  Serial.println("Distance=" + String(my_distance));
  Serial.println("Hrbeat=" + String(my_hrbeat));
        //Set multi connections
        sendESP8266Cmdln("AT+CIPMUX=1", 1000);
  
  /*
  AT+CIPSTART=id,"type","addr",port
  id = 0
  type = "TCP"
  addr = "www.example.com"
  port = 80
  */
  String TARGET_ID="0";
  String TARGET_TYPE="TCP";
  String TARGET_ADDR="184.106.153.149";  //ThingSpeak IP Address
  String TARGET_PORT="80";

  String cmd="AT+CIPSTART=" + TARGET_ID;
  cmd += ",\"" + TARGET_TYPE + "\",\"" + TARGET_ADDR + "\"";
  cmd += ","+ TARGET_PORT;

  Serial.println(cmd);
  ESP8266.println(cmd);
  delay(1000);
  //Assume OK
  //display and clear buffer
  clearESP8266SerialBuffer();
  
  /*
  POST /update?key=<API KEY>&field1=xx.xx&field2=xx.xx HTTP/1.1\r\n
  Host: api.thingspeak.com\r\n\r\n
  */
  
  String ThingSpeakMethod = "GET";  //GET should also work
  String ThingSpeakHost = "/update";
  String ThingSpeakApiKey = "3G5QIEOLKJTDV8Y6";
  String ThingSpeakField1 = "field1";
  String ThingSpeakField2 = "field2";
  String ThingSpeakField3 = "field3";
  String ThingSpeakHttp = "HTTP/1.1";
  
  String ThingSpeak_2ndLine = "Host: api.thingspeak.com\r\n\r\n";
  
  String HTTP_RQS = ThingSpeakMethod;
  HTTP_RQS += " " + ThingSpeakHost;
  HTTP_RQS += "?key=" + ThingSpeakApiKey;
  HTTP_RQS += "&" + ThingSpeakField1 + "=" + my_calories;
  HTTP_RQS += "&" + ThingSpeakField2 + "=" + my_distance;
  HTTP_RQS += "&" + ThingSpeakField3 + "=" + my_hrbeat;
  HTTP_RQS += " " + ThingSpeakHttp + "\r\n";
  HTTP_RQS += ThingSpeak_2ndLine;
  
  String cmdSEND_length = "AT+CIPSEND=";
  cmdSEND_length += TARGET_ID + "," + HTTP_RQS.length() +"\r\n";
  
  ESP8266.print(cmdSEND_length);
  Serial.println(cmdSEND_length);
  
  Serial.println("waiting >");
  
  if(!ESP8266.available());
  
  if(ESP8266.find(">")){
    Serial.println("> received");
    ESP8266.println(HTTP_RQS);
    Serial.println(HTTP_RQS);
    
    boolean OK_FOUND = false;
    
    //program blocked untill "SEND OK" return
    while(!OK_FOUND){
      if(ESP8266.readBytesUntil('\n', buffer, BUFFER_SIZE)>0){
        Serial.println("...");
        Serial.println(buffer);
        
        if(strncmp(buffer, "SEND OK", 7)==0){
          OK_FOUND = true;
          Serial.println("SEND OK found");
        }else{
          Serial.println("Not SEND OK...");
        }
      }
    }

    if(OK_FOUND){
      delay(1000);
      //Dummy display received data
      while (ESP8266.available() > 0) {
        char a = ESP8266.read();
        Serial.write(a);
      }
    }
    
  }else{
    Serial.println("> NOT received, something wrong!");
  }
  
  //Close connection
  String cmdCIPCLOSE = "AT+CIPCLOSE=" + TARGET_ID; 
  ESP8266.println(cmdCIPCLOSE);
  Serial.println(cmdCIPCLOSE);
  
  delay(1000);
  
  temp=0;
  }
  delay(500);
  digitalWrite(26,LOW);
}

//-----------------------------------------------------------------------------
boolean waitOKfromESP8266(int timeout)
{
  do{
    Serial.println("wait OK...");
    delay(1000);
    if(ESP8266.find("OK"))
    {
      return true;
    }

  }while((timeout--)>0);
  return false;
}

boolean cwJoinAP()
{
  String cmd="AT+CWJAP=\"" + SSID + "\",\"" + PASSWORD + "\"";
  ESP8266.println(cmd);
  return waitOKfromESP8266(10);
}

//Send command to ESP8266, assume OK, no error check
//wait some time and display respond
void sendESP8266Cmdln(String cmd, int waitTime)
{
  ESP8266.println(cmd);
  delay(waitTime);
  clearESP8266SerialBuffer();
}

//Basically same as sendESP8266Cmdln()
//But call ESP8266.print() instead of call ESP8266.println()
void sendESP8266Data(String data, int waitTime)
{
  //ESP8266.print(data);
  ESP8266.print(data);
  delay(waitTime);
  clearESP8266SerialBuffer();
}

//Clear and display Serial Buffer for ESP8266
void clearESP8266SerialBuffer()
{
  Serial.println("= clearESP8266SerialBuffer() =");
  while (ESP8266.available() > 0) {
    char a = ESP8266.read();
    Serial.write(a);
  }
  Serial.println("==============================");
}

//-------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------Analog Sensors--------------------------------------------------------

void adc()
{
}
 
//--------------------------------------------------------------------------- UNO ADXL interface
void receiveEvent(int howMany)
{
 
  while(Wire.available())
  {
    steps = Wire.read();
    if(steps!=prev_steps)
    {
    time_monitor++;
    Serial.print("timeread = ");
    Serial.println(time_monitor);
  Serial.print("eepromwrite = ");
    EEPROM.write(25,1 + EEPROM.read(25)); // steps_timer =25 memory of eeprom
   Serial.println(EEPROM.read(25));
    Serial.print("steps = ");
    Serial.println(steps);
    prev_steps=steps;
    }
    

   else
   {
    Serial.println("No increment in steps");
  }
}
}

//------------------------------------------------------------------------------------------------------------- first screen------------------------------------
void FirstScreen()
{
    tft.setRotation(1);
    tft.fillScreen(ST7735_BLACK);
    bmpDraw("fitness1.bmp", 0,0);
 //  delay(10000);
} 

//-------------------------------------------------sceond screen=---------------------------------------------------
void Welcome() 
{
  tft.fillScreen(ST7735_BLACK);
 
 tft.setRotation(1);
  tft.setCursor(20, 55);
  tft.setTextColor(ST7735_WHITE);  tft.setTextSize(3);
  tft.println("WELCOME");
  tft.println("\n");
 // delay(1000);
  
  }
  
  
  void SecondScreen()
{
  tft.fillScreen(ST7735_BLACK);
 
  tft.setRotation(1);
  tft.setCursor(30,10);  
  tft.setTextColor(ST7735_RED);  tft.setTextSize(2);
  tft.println("WRISTLET");
  tft.println("\n");
    tft.setCursor(0,40);  
  tft.setTextColor(ST7735_YELLOW);  tft.setTextSize(1);
  tft.println(" Stop Saying <I CAN'T>!");
  tft.println( "\n");
  tft.println(" YOU CAN! ");
  tft.println( "\n");
  tft.println(" YOU just Choose not to!");
//delay(10000);
} 

//-------------------------------------------------------------------------------------------------------------------------credits------------------
void CreditScreen()
{
  tft.fillScreen(ST7735_BLACK);
 
  tft.setRotation(1);
  tft.setCursor(0, 0);  
  tft.setTextColor(ST7735_YELLOW);  tft.setTextSize(1);
  tft.println("Under the Relentless Guidance of: ");
  tft.setTextColor(ST7735_RED);  tft.setTextSize(2);
  tft.setCursor(0,20);
  tft.println("Prof.SUNIL MP");
  tft.setCursor(0,25);
  tft.setTextColor(ST7735_CYAN);  tft.setTextSize(1);
  tft.println("\n");
  tft.setTextColor(ST7735_GREEN);  tft.setTextSize(1);
  tft.println("Passion by -");
   
   tft.setTextColor(ST7735_CYAN);  tft.setTextSize(1);
    tft.println( "\n");
   tft.println("PRAVEEN K ");
   tft.setTextColor(ST7735_CYAN);  tft.setTextSize(1);
  tft.println( "\n");
  tft.setTextColor(ST7735_CYAN);  tft.setTextSize(1);
  tft.println("PAWAN SAI R ");
  tft.setTextColor(ST7735_CYAN);  tft.setTextSize(1);
  tft.println( "\n");
  tft.setTextColor(ST7735_CYAN);  tft.setTextSize(1);
  tft.println("SUMA B");
 // delay(10000);
} 

//------------------------------------------------------------------------------------------
  
//----------------Timer1 LCD control ISR-----------------
ISR(TIMER1_COMPA_vect)

{//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
secs++;
if(secs == 10)
{
secs = 0;
digitalWrite(6,LOW);
}
}

//------------------------------------------------

//------------------------------------------------------------------------------------------------

//---------------------------------------------------------------USer Data Input---------------------------------------
void user_input()                             
{
  tft.setCursor(0,0);
  int temp=1,gender_select,user_age=10,height=140,weight=30, stride=10;
  int gender_flag;
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setTextSize(1);
  tft.println("If you are using Wristlet for the first time, kindly configure your details or else skip it");
  tft.setCursor(0,60);
  tft.setTextSize(2);
  tft.setTextColor(ST7735_GREEN);
  tft.println("Button 3 =   Setup");
  tft.setTextColor(ST7735_RED);
  tft.println("Button 4 =   Skip");
  while(temp==1)
  {
  
  if(digitalRead(INC)==0)
  {
    tft.fillScreen(ST7735_BLACK);
  Serial.println("Please Select your gender");
  tft.setCursor(0,0);
    tft.setTextColor(ST7735_BLUE);
    tft.setTextSize(2);
  tft.println("Please Select your gender");
  temp=2;
  while(digitalRead(INC)==0);
  }
  if(digitalRead(next)==0)
    {
     temp=0;
     while(digitalRead(next)==0);
    }
  }
 
    while(temp==2)
    {
    tft.setCursor(30,40);
    tft.setTextColor(ST7735_CYAN);
    tft.setTextSize(3);
    tft.println("I am a ");
    tft.setCursor(0,105);
    tft.setTextSize(1);
    tft.println("Please press button 3 to  toggle");
    tft.setTextSize(3);
     if(digitalRead(INC)==0)
     {
      gender_select++;
      if (gender_select%2==0)
      {
        Serial.println("I am a Man");
        tft.fillRect(20,75,130,28,ST7735_BLACK);
        tft.setCursor(55,75);
        tft.println("MAN");
        gender_flag=0;
      }
      else
      {
        Serial.println("I am a Woman");
        tft.fillRect(20,75,130,28,ST7735_BLACK);
        tft.setCursor(42,75);
        tft.println("WOMAN");
        gender_flag=1;
      }
      
      while(digitalRead(INC)==0);
     }
     
    if(digitalRead(next)==0)
    {
      eeprom_gender_flag = gender_flag;
      EEPROM.write(eeprom_mem++,eeprom_gender_flag);
      temp=3;
      tft.fillScreen(ST7735_BLACK);
      tft.setTextSize(2);
      tft.setCursor(0,0);
      tft.setTextColor(ST7735_BLUE);
      tft.println("Enter your   age");
      Serial.println("Enter your age");
      
     while(digitalRead(next)==0);
    }
    }
    //-------------
    while(temp==3)
    {
     tft.setTextSize(4);
     tft.setTextColor(ST7735_CYAN);
     
     while(digitalRead(INC)==0)
     {
      user_age++;
      if(user_age==60)
      {
      user_age=10;
      }   
     tft.fillRect(50,50,50,50,ST7735_BLACK);
     tft.setCursor(55,55);
     tft.println(user_age);
     Serial.print("\n");
     Serial.print(user_age);
     }
     Serial.print("\n");
     delay(100);
      if(digitalRead(next)==0)
      {
       eeprom_user_age=user_age;
       EEPROM.write(eeprom_mem++, eeprom_user_age);
       temp=4;
       
       tft.fillScreen(ST7735_BLACK);
      tft.setTextSize(1);
      tft.setCursor(0,0);
      tft.setTextColor(ST7735_YELLOW);
      tft.println("Enter your height in cm");
      Serial.println("Enter your height in cm");
       while(digitalRead(next)==0);
      }
    }
    //-------------
    while(temp==4)
    {
     tft.setTextSize(4);
     tft.setTextColor(ST7735_YELLOW);
     while(digitalRead(INC)==0) {
      height++;
      if(height==200)
      {
      height=140;
      }
     tft.fillRect(45,45,80,50,ST7735_BLACK);
     tft.setCursor(45,45);
     tft.println(height);
     Serial.print("\n");
     Serial.print(height);
     Serial.print("\n");
     }
     delay(100);
      if(digitalRead(next)==0)
      {
       eeprom_height=height;
       EEPROM.write(eeprom_mem++, eeprom_height);
       temp=5;
       
       tft.fillScreen(ST7735_BLACK);
      tft.setTextSize(1);
      tft.setCursor(0,0);
      tft.setTextColor(ST7735_YELLOW);
      tft.println("Enter your weight in kg");
      Serial.println("Enter your weight in kg");
       while(digitalRead(next)==0);
      }
    }
    
    //--------
    
    while(temp==5)
    {
       tft.setTextSize(4);
     tft.setTextColor(ST7735_RED);
      
     while(digitalRead(INC)==0)
     {
      weight++;
      if(weight==120)
      {
      weight=30;
      }
     tft.fillRect(45,45,80,50,ST7735_BLACK);
     tft.setCursor(50,50);     
     tft.println(weight);
     Serial.print("\n");
     Serial.print(weight);
     Serial.print("\n");
     }
     delay(100);
      if(digitalRead(next)==0)
      {
       eeprom_weight=weight;
       EEPROM.write(eeprom_mem++, eeprom_weight);
       temp=6;
       tft.fillScreen(ST7735_BLACK);
      tft.setTextSize(1);
      tft.setCursor(0,0);
      tft.setTextColor(ST7735_YELLOW);
      tft.println("Enter your stride length in cm");
      Serial.println("Enter your stride length in cm");
      
       while(digitalRead(next)==0);
       
      }
    }
    
    while(temp==6)
    {
      tft.setTextSize(4);
     tft.setTextColor(ST7735_CYAN);
     
     while(digitalRead(INC)==0)
     {
      stride++;
      if(stride==150)
      {
      stride=10;
      }   
     tft.fillRect(50,50,50,50,ST7735_BLACK);
     tft.setCursor(55,55);
     tft.println(stride);
     Serial.print("\n");
     Serial.print(stride);
     }
     Serial.print("\n");
     delay(100);
      if(digitalRead(next)==0)
      {
       eeprom_stride=stride;
       EEPROM.write(eeprom_mem++, eeprom_stride);
       temp=0;
       
       
      }
    }
  
}
    
//-----------------------------------------------------------------------------------------------------------------------------------------

void calories_display()
{
  int user_age,height,weight,gender_flag;
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  float BMR_male, BMR_female,calorie_instant_male,calorie_instant_female;//Basal Metabolic Rate (over 24 hours) 
  float MET = 3.8;//Metabolic Equivalent (for selected activity)
  float time_walked;
 //Serial.println(EEPROM.read(15));//gender_flag
// Serial.println(EEPROM.read(16));//user_age
// Serial.println(EEPROM.read(17));//height
// Serial.println(EEPROM.read(18));//weight
//Serial.println(EEPROM.read(19)); //stride
 gender_flag = EEPROM.read(15);
 user_age = EEPROM.read(16);
 height= EEPROM.read(17);
 weight = EEPROM.read(18);
 time_walked = EEPROM.read(25);
 Serial.print("time Walked = ");
 Serial.println(time_walked);
 total_time = time_walked /360;//total time in hours
 
  if(gender_flag==0)
  {
    BMR_male =  (13.75 * weight) + (5 * height) - (6.76 * user_age) + 66;
    calorie_instant_male = (BMR_male / 24) * MET * total_time;
    EEPROM.write(35,calorie_instant_male);
 //   Serial.println(gender_flag);
  //   Serial.println(user_age);
  //    Serial.println(height);
  //     Serial.println(weight);
   // Serial.println(BMR_male);
   // Serial.println(calorie_instant_male);
    tft.setTextSize(2);
    tft.setCursor(0,0);
    tft.setTextColor(ST7735_MAGENTA);
    tft.print("BMR = ");
    tft.println("");
    tft.setTextColor(ST7735_CYAN);
    tft.setTextSize(3);
    tft.println(BMR_male);
    tft.println("");
    tft.setTextSize(1);
    tft.setTextColor(ST7735_MAGENTA);
    tft.println("You current calorie burn = ");
    tft.setTextColor(ST7735_CYAN);
    tft.setTextSize(3);
    tft.println(calorie_instant_male);
    
  }
  
   if(gender_flag==1)
  {
    BMR_female =  (9.56 * weight) + (1.85 * height) - (4.68 * user_age) + 655;
    calorie_instant_female = (BMR_female / 24) * MET *time_walked;
    EEPROM.write(35,calorie_instant_male);
 //   Serial.println(gender_flag);
  //  Serial.println(BMR_female);
  //  Serial.println(calorie_instant_female);
  //     Serial.println(user_age);
  //    Serial.println(height);
     //  Serial.println(weight);
    tft.setTextSize(2);
    tft.setCursor(0,0);
    tft.setTextColor(ST7735_MAGENTA);
    tft.print("BMR = ");
    tft.println("");
    tft.setTextColor(ST7735_CYAN);
    tft.setTextSize(3);
    tft.println(BMR_female);
    tft.println("");
    tft.setTextSize(1);
    tft.setTextColor(ST7735_MAGENTA);
    tft.println("You current calorie burn = ");
    tft.setTextColor(ST7735_CYAN);
    tft.setTextSize(3);
    tft.println(calorie_instant_female);
  }
}
//--------------------------------------------------------------------------------------------------------------------
void user_friend()
{
  distance_walked();
  calories_for_user();
  
 tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  
  float calorie,distance;
  calorie = global_calories;
  distance = global_distance;
  
  
    Serial.print(calorie);

    Serial.print(distance);
    tft.setTextSize(1); 
  tft.setCursor(35, 0);
  tft.setTextColor(ST7735_YELLOW);
  tft.print("You have ");
  //tft.setCursor(52, 20);
  tft.println("walked ");
  tft.setCursor(25, 20);
   tft.setTextSize(3); 
  tft.setTextColor(ST7735_RED);
  tft.print(distance);
  tft.setTextColor(ST7735_GREEN);
  tft.print("m");
  
  //--------------------------------------------
  tft.setTextSize(1); 
  tft.setCursor(35, 55);
  tft.setTextColor(ST7735_YELLOW);
  tft.println("You have burnt ");
  tft.setTextSize(3); 
  tft.setTextColor(ST7735_RED);
   tft.setCursor(10, 70);
  tft.print(calorie);
  tft.setTextColor(ST7735_GREEN);
  tft.setCursor(40, 95);
  tft.print("kcal");
 }
 
//---------------------------------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------Password input-------------------------------------

void password()
{
  
   tft.fillScreen(ST7735_BLACK);
   tft.setRotation(1);
  tft.setCursor(0,0);
    tft.setTextSize(2);
    tft.println("Password Pls");
  int temp=1,first_digit=0,second_digit=0,third_digit=0,fourth_digit=0;
int password_flag=0;
 while(password_flag != 1)
 {
    while(temp==1)
    {
      Serial.println(password_flag);
     if(digitalRead(INC)==0)
     {
      first_digit++;
      if(first_digit==10)
      {
       first_digit=0;
      }
      while(digitalRead(INC)==0);
     }
     
    Serial.print("\n");
    Serial.print("Enter Password ");
    Serial.print(first_digit);
    Serial.print(":");
    Serial.print(second_digit);
    tft.fillRect(40,40,25,33,ST7735_BLACK);
    tft.setCursor(40,40);
    tft.setTextColor(ST7735_BLUE);
    tft.setTextSize(4);
    tft.print(first_digit);
    tft.print(second_digit);
     tft.print(third_digit);
      tft.print(fourth_digit);
    Serial.print("\n");
    delay(100);
    if(digitalRead(next)==0)
    {
     
     temp=2;
     while(digitalRead(next)==0);
    }
    }
    
    while(temp==2)
    {
     if(digitalRead(INC)==0)
     {
      second_digit++;
      if(second_digit==10)
      {second_digit=0;}
      while(digitalRead(INC)==0);
     }
     tft.fillRect(64,40,25,33,ST7735_BLACK);
     tft.setCursor(65,40);
     tft.setTextColor(ST7735_BLUE);
 
    tft.print(second_digit);
    
     Serial.print("\n");
     Serial.print(first_digit);
     Serial.print(":");
     Serial.print(second_digit);
     Serial.print("\n");
     delay(100);
      if(digitalRead(next)==0)
      {
      
       temp=3;
       while(digitalRead(next)==0);
      }
    }
      while(temp==3)
    {
     if(digitalRead(INC)==0)
     {
      third_digit++;
      if(third_digit==10)
      {third_digit=0;}
      while(digitalRead(INC)==0);
     }
     tft.fillRect(89,40,25,33,ST7735_BLACK);
     tft.setCursor(90,40);
     tft.setTextColor(ST7735_BLUE);

     tft.print(third_digit);
 
     Serial.print("\n");
     Serial.print(first_digit);
     Serial.print(":");
     Serial.print(second_digit);
     Serial.print(":");
     Serial.print(third_digit);
     Serial.print("\n");
     delay(100);
      if(digitalRead(next)==0)
      {
      
       temp=4;
       while(digitalRead(next)==0);
      }
    }
    while(temp==4)
    {
     if(digitalRead(INC)==0)
     {
      fourth_digit++;
      if(fourth_digit==10)
      {fourth_digit=0;}
      while(digitalRead(INC)==0);
     }
     tft.fillRect(114,40,25,33,ST7735_BLACK);
     tft.setCursor(115,40);
     tft.setTextColor(ST7735_BLUE);
 
      tft.print(fourth_digit);
     Serial.print("\n");
     Serial.print(first_digit);
     Serial.print(":");
     Serial.print(second_digit);
     Serial.print(":");
     Serial.print(third_digit);
      Serial.print(":");
     Serial.print(fourth_digit);
     Serial.print("\n");
     delay(100);
      if(digitalRead(next)==0)
      {
        if((first_digit==2) && (second_digit==0) && (third_digit==1) && (fourth_digit==6))
        {      
       Serial.println("success");
       temp=0;
       password_flag = 1;
       Serial.println(password_flag);
        }
        else
        {
          temp=1;
          Serial.println("fail");
          tft.setTextSize(2);
           tft.setTextColor(ST7735_RED);
          tft.println("Access Denied");
          password_flag=0;
          Serial.println(password_flag);
        }
        
      
       while(digitalRead(next)==0);
      }
    }
 }
}

//----------------------------------------------calorie count------------------------------------------------------
void calories_for_user()
{
  int user_age,height,weight,gender_flag;
  float BMR_male, BMR_female,calorie_instant_male,calorie_instant_female;//Basal Metabolic Rate (over 24 hours) 
  float MET = 3.8;//Metabolic Equivalent (for selected activity)
  float time_walked;
 //Serial.println(EEPROM.read(15));//gender_flag
// Serial.println(EEPROM.read(16));//user_age
// Serial.println(EEPROM.read(17));//height
// Serial.println(EEPROM.read(18));//weight
//Serial.println(EEPROM.read(19)); //stride
 gender_flag = EEPROM.read(15);
 user_age = EEPROM.read(16);
 height= EEPROM.read(17);
 weight = EEPROM.read(18);
 time_walked = EEPROM.read(25);
 Serial.print("time Walked = ");
 Serial.println(time_walked);
 total_time = time_walked /360;//total time in hours
 
  if(gender_flag==0)
  {
    BMR_male =  (13.75 * weight) + (5 * height) - (6.76 * user_age) + 66;
    calorie_instant_male = (BMR_male / 24) * MET * total_time;
    global_calories = calorie_instant_male;

    
  }
  
   if(gender_flag==1)
  {
    BMR_female =  (9.56 * weight) + (1.85 * height) - (4.68 * user_age) + 655;
    calorie_instant_female = (BMR_female / 24) * MET *time_walked;
    global_calories = calorie_instant_female;

  }
}

//------------------------------------------------

void BMI()
{
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  float BMI;
  float weight, height;
  weight = EEPROM.read(18);
  height = EEPROM.read(17);
  BMI=(weight)/((height*height)/10000);
  Serial.println(BMI);
  tft.setTextSize(2); 
  tft.setCursor(20, 10);
  tft.setTextColor(ST7735_YELLOW);
  tft.println("Your BMI is ");
  tft.setCursor(30, 40);
  tft.setTextSize(3); 
  tft.setTextColor(ST7735_RED);
  tft.println(BMI);
  tft.setTextSize(1); 
  tft.setCursor(0, 80);
  tft.setTextColor(ST7735_GREEN);
  tft.println("Body Mass Index is an     index that tells you the  percentage of fat content in your body");
}
//-----------------------------------------------------------------------------------------------------------------------

//----------------------------------heart rate--------------------------------------------------------------

void heartrate_count()
{
  int temp=1;
  Serial.println("entered");
  
  while(temp==1)
  {
     randomSeed(analogRead(0));
   hr_count = random(72, 83);
  Serial.println(hr_count);
  temp =0;
  }
  delay(2000);
}


//--------------------------------------------------------------------------------------------------------------------------

///----------------------------------------------------sd log-----------------------------------------------------

void sdlog()
{
  int temp =1;
  while (temp==1)
  {
  DateTime now = rtc.now();
  char timeStamp[22];
  sprintf(timeStamp, "[%02d/%02d/%02d][%02d:%02d:%02d] ",now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());
 Serial.println(timeStamp);
 
   
  //  digitalWrite(40, LOW);    
 // delay(10); 
 // digitalWrite(40, HIGH); 
    Serial.println("Clear EEPROM and send data log to SD card");
    
    calories = SD.open("calories.txt", FILE_WRITE);  
    calories.print(global_calories); //Write calories to file
    calories.print(',');
    calories.print(timeStamp);
    
    calories.println();
    calories.close();
  //  delay(5000);
    mydist = SD.open("mydist.txt", FILE_WRITE);  
    mydist.print(global_distance); //Write distance to file
    mydist.print(',');
    mydist.print(timeStamp);
    mydist.println();
    mydist.close();

    EEPROM.write(25, 0); // time walked
global_distance = 0;
global_calories=0;
temp=0;
  }
}

//---------------------------------------------------------------------------------
//------------------------------------------thanks------------------------------------
void thanks()
{
   tft.fillScreen(ST7735_BLACK);
 
  tft.setRotation(1);
  tft.setCursor(0, 0);  
  tft.setTextColor(ST7735_YELLOW);  tft.setTextSize(1);
  tft.println("Special Thanks to ");
  
   
   tft.setTextColor(ST7735_CYAN);  tft.setTextSize(1);
    tft.println( "\n");
   tft.println("JAYANTH N");
   tft.setTextColor(ST7735_CYAN);  tft.setTextSize(1);
  tft.println( "\n");
  tft.setTextColor(ST7735_CYAN);  tft.setTextSize(1);
  tft.println("SIDDHARTH P ");
  tft.setTextColor(ST7735_CYAN);  tft.setTextSize(1);
  tft.println( "\n");
  tft.setTextColor(ST7735_CYAN);  tft.setTextSize(1);
  tft.println("PRADEEP KUMAR");
  
  delay(3000);
}
