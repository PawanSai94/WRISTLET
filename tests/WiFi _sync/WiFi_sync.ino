
#define ESP8266 Serial2
#include "DHT.h"
#define DHTTYPE DHT22
#define DHTPIN 22  //pin 2 connect to DHT11
DHT dht(DHTPIN, DHTTYPE);  //30 for Arduino Due

String SSID = "Zenith";
String PASSWORD = "26Ra-5Ba-1As-14Pa";

int LED = 13;

boolean FAIL_8266 = false;

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

void setup() {
  pinMode(LED, OUTPUT);
  
  digitalWrite(LED, LOW);
  delay(300);
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
  delay(300);
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);

  do{
    //Serial.begin(9600);
    //ESP8266.begin(9600);
    Serial.begin(9600);
    ESP8266.begin(115200);
  
    //Wait Serial Monitor to start
    while(!Serial);
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
        delay(500);
        FAIL_8266 = true;
      }
    }else{
      Serial.println("Module have no response.");
      delay(500);
      FAIL_8266 = true;
    }
  }while(FAIL_8266);
  
  digitalWrite(LED, HIGH);
  
  //set timeout duration ESP8266.readBytesUntil
  ESP8266.setTimeout(1000);
  
  dht.begin();
}

void loop(){
  
  float valHumidity;
  float valTemperature;
  do{
    valHumidity = dht.readHumidity();
    valTemperature = dht.readTemperature();
  }while(isnan(valHumidity) || isnan(valTemperature));
  
  Serial.println("Humidity=" + String(valHumidity));
  Serial.println("Temperature=" + String(valTemperature));
  
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
  String ThingSpeakApiKey = "JF288Z2U386KPU6C";
  String ThingSpeakField1 = "field1";
  String ThingSpeakField2 = "field2";
  String ThingSpeakHttp = "HTTP/1.1";
  
  String ThingSpeak_2ndLine = "Host: api.thingspeak.com\r\n\r\n";
  
  String HTTP_RQS = ThingSpeakMethod;
  HTTP_RQS += " " + ThingSpeakHost;
  HTTP_RQS += "?key=" + ThingSpeakApiKey;
  HTTP_RQS += "&" + ThingSpeakField1 + "=" + valHumidity;
  HTTP_RQS += "&" + ThingSpeakField2 + "=" + valTemperature;
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
  
  delay(5000);
  
}

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
