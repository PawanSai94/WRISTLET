// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include "DHT.h"

#define TFT_CS     10
#define TFT_RST    9  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     8
#define TFT_SCLK 12   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

#define DHTPIN 22
#define DHTTYPE DHT22 
RTC_DS1307 rtc;
DHT dht(DHTPIN, DHTTYPE);
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
  Wire.begin(0x68);
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);
  Serial.println("Initialized");
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);
  
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
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
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
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    tft.fillScreen(ST7735_BLACK);
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
  tft.setCursor(50, 40);
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(3); 
  tft.print(now.hour(),DEC);
  tft.print(':');
  tft.print(now.minute(), DEC); 
  
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
  //tft.print("*C");
    
    Serial.println();
    delay(3000);
}
