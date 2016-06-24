#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

#define TFT_CS  10  // Chip select line for TFT display
#define TFT_RST  9  // Reset line for TFT (or see below...)
#define TFT_DC   8  // Data/command line for TFT

#define SD_CS    53 // Chip select line for SD card

#define BUFFPIXEL 20
#define TFT_SCLK  12  // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int pin19=19;
int pin18=3;
int pin2=2;
  
volatile int pawan = 0;
volatile int suma =0;
void setup() {
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) 
  {
    Serial.println("failed!");
    return;
  }
  Serial.println("OK!");
  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  time = millis() - time;
  Serial.println(time, DEC);
  delay(500);  
    
    pinMode(pin19, INPUT);
   pinMode(pin2, INPUT);
   pinMode(pin18, INPUT);
 attachInterrupt(digitalPinToInterrupt(pin18), inc, FALLING);
   attachInterrupt(digitalPinToInterrupt(pin19),homepage, FALLING);
attachInterrupt(digitalPinToInterrupt(pin2),functioninside, FALLING);


tft.initR(INITR_BLACKTAB);

  MainMenu();
  Serial.println("main menu dispayed");
  delay(1000);
  
    
}

void loop() {
    Serial.println(pawan);
    Serial.println("abc");
    Serial.println(suma);
    delay(1000);
}

//---------------------------------------- int1-------------------

void inc() {
  
  pawan = pawan+1;
  switch(pawan)
  {
  case(1):
  Health();
  break;
  
  case(2):
  WeConnect();
  break;
  
  case(3):
  WeSet();
  break;
  case(4):
  MainMenu();
  pawan=0;
  break;
  
  }
  delayMicroseconds(12000);
}

//-----------------------------int homepage--------------------------

void homepage()

{
  MainMenu();
  pawan=0;
  

}

//----------------------------------function inside function------------------------------------------------------------

void functioninside()
{
  suma = suma+1;
  switch(pawan)
  {
  case(1):
  
switch(suma)
{
  case(1):
  printcalories();
  break;
  case(2):
bloodpressure();
  break;
  
  case(3):
  heartrate();
  break;
  
  case(4):
  sugarlevel();
  break;
  
  case(5):
  Health();
  suma=0;
  break;
}
break;
  
  
  case(2):
  
    switch(suma)
    {
    case(1):
    gps();
    break;
    
    case(2):
    wi_fi();
    break;
    
    case(3):
    WeConnect();
    suma=0;
    break;
    }
    break;

 case(3):
 switch(suma)
    {
    
      
      case(1):
      Alarm();
      break;
      
      case(2):
      Time_Date();
      break;
      
      case(3):
      About();
      break;
      
      case(4):
      WeSet();
      suma=0;
      break;
    }
  break;
  }
      
}

//-----------------------------------------------------------------------------------------------------------------------

void printcalories()
{
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  int c;
  
  c=2000;
  tft.println(c);
  delay(1000);
  
}
//-------------------------------------------------------------------

void bloodpressure()
{
 tft.fillScreen(ST7735_RED);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  
  int bp;
  
  bp = 120;
  
  tft.println(" Blood Pressure");
  delay(1000);
}

//------------------------------------------------------------------------------

void heartrate()

{
  
  tft.fillScreen(ST7735_GREEN);
  tft.setRotation(1);
  
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_BLACK);
  tft.setTextSize(3);
  int hr = 72;
  
  tft.println(hr);
  delay(1000);
}

//-------------------------------------------------------------

void sugarlevel()
{
  tft.fillScreen(ST7735_YELLOW);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  
  int sl = 150;
  tft.println(sl);
  delay(1000);
}

//----------------------------------------------------------------------------------------------------------------

void gps()
{
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  
  int lat,lon;
  lat=12;
  lon=17;
  tft.println(lat);
  tft.println(lon);
  delay(1000);
}

void wi_fi()
{
tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  
  tft.println("sync");
  delay(1000);

}

//---------------------------------------------------------------------------------------------------------

 void Alarm()
 {
   
   tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  
  tft.println("Alarm");
  delay(1000);
 }
 
 void Time_Date()
 {
   tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  
  tft.println(" Displayed ");
  delay(1000);
  
 }
 
 void About()
 {
   tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  
  tft.println("WRISTLET");
  delay(1000);
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
  tft.println(" BP");
  bmpDraw("bp.bmp",100,23);
  tft.setCursor(18,116);
  tft.println("HeartRate");
  bmpDraw("HR2.bmp",20,75);
  tft.setCursor(104,116);
  tft.println("Sugar");
  bmpDraw("SL.bmp",100,75);
} 
//------------------------------------------------

void WeConnect() {
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setTextColor(ST7735_YELLOW);  tft.setTextSize(2);
  tft.setCursor(10,0);
  tft.println("CONNECTIVITY");
  tft.setTextColor(ST7735_GREEN);  tft.setTextSize(1);
  tft.setCursor(32,81);
  tft.println("WI-FI");
  bmpDraw("wifi.bmp",25,40);
  tft.setCursor(105,82);
  tft.println("GPS");
  bmpDraw("GPS.bmp",95,40);
 
} 
//--------------------------------------------------------
void WeSet() {
  tft.fillScreen(ST7735_BLACK);
tft.setRotation(1);
 tft.setTextColor(ST7735_BLUE);  tft.setTextSize(2);
  tft.setCursor(30,0);
  tft.println("SETTINGS");
  
  tft.setTextColor(ST7735_RED);  tft.setTextSize(1);
  tft.setCursor(21,67);
  tft.println("Clock");
  bmpDraw("Clock.bmp",20,25);
  tft.setCursor(102,67);
  tft.println("Alarm");
  bmpDraw("Alarm1.bmp",100,25);
  tft.setCursor(57,116);
  tft.println("About");
  bmpDraw("text.bmp",55,75);
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

  
  
