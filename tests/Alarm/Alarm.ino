// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include "DHT.h"
#include<EEPROM.h>

String hours;
String mins;
int minuteNow=0;
int minutePrevious=0;

int hourPrevious = 25;
int hourNow = 0;
#define TFT_CS     10
#define TFT_RST    9  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     8

//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
#define TFT_SCLK 12   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#define DHTPIN 22
#define DHTTYPE DHT22 

int temp,inc,hours1,minut,add=11;
int next=2;    
int INC=3;
int set_mad=18;
#define buzzer 13
int HOUR,MINUT,SECOND;

RTC_DS1307 rtc;
DHT dht(DHTPIN, DHTTYPE);
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};



void setup () {
  pinMode(INC, INPUT);
 pinMode(next, INPUT);
 pinMode(set_mad, INPUT);
 pinMode(buzzer, OUTPUT);
 digitalWrite(next, HIGH);
 digitalWrite(set_mad, HIGH);
 digitalWrite(INC, HIGH);
 
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);
  Serial.println("Initialized");
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  
  dht.begin();
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
   //  rtc.adjust(DateTime(2016, 3, 6, 11, 26, 0));
  }
}

void loop () {
  
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  
    DateTime now = rtc.now();
    
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
    
    minuteNow = now.minute();
  if(minuteNow!=minutePrevious)
  {
    tft.fillRect(15,15,140,65,ST7735_BLACK);
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
  minutePrevious = minuteNow;
  }
  
  hourNow = now.hour();
  if(hourNow!=hourPrevious)
  {  
  tft.fillRect(0,100,160,100,ST7735_BLACK);
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
  hourPrevious = hourNow;
  } 
  
  if(digitalRead(set_mad) == 0)      //set Alarm time
   { 
     
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
     
     
     tft.fillScreen(ST7735_BLACK);
     
    
    //delay(1000);
    Serial.print("\n");
    //lcd.clear();
    //lcd.setCursor(0,0);
    Serial.print("  Alarm time ");
    //lcd.setCursor(0,1);
    Serial.print(" has been set  ");
   // delay(2000);
 }
 match();
}

//-------------------------------------------------------------

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
    // tft.setTextSize(3);
     //tft.print(HOUR);
   //tft.print(":");
   tft.print(MINUT);
     // lcd.clear();
     Serial.print("\n");
    //lcd.setCursor(0,1);
    Serial.print(HOUR);
    Serial.print(":");
    Serial.print(MINUT);
    

    Serial.print("\n");
    delay(100);
      if(digitalRead(next)==0)
      {
       minut=MINUT;
       EEPROM.write(add++, minut);
       temp=0;
       while(digitalRead(next)==0);
      }
    }
    
       
    delay(1000);
}

/* Function to chack medication time */

void match()
{
  int tem[17];
  for(int i=11;i<17;i++)
  {
    tem[i]=EEPROM.read(i);
    Serial.print("hour = ");
    Serial.println(tem[11]);
    Serial.print("minute = ");
    Serial.println(tem[12]);
    Serial.print("a=");
    Serial.println(tem[13]);
    Serial.print("b=");
    Serial.println(tem[14]);
    Serial.print("c=");
    Serial.println(tem[15]);
    Serial.print("d=");
    Serial.println(tem[16]);
  }
  if(HOUR == tem[11] && MINUT == tem[12]) 
  {
   beep();
   beep();
   beep();
   beep();
   //lcd.clear();
   Serial.print("\n");
   Serial.print("Wake Up........");
   //lcd.setCursor(0,1);
   Serial.print("Wake Up.......");
   beep();
   beep();
   beep();
   beep();
  }
}

/* function to buzzer indication */

void beep()
{
   digitalWrite(buzzer,HIGH);
   delay(500);
   digitalWrite(buzzer, LOW);
   delay(500);
}
