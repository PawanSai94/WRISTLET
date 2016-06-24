
int sensorPin = A0;    // select the input pin for the potentiometer

int sensorValue = 0;  // variable to store the value coming from the sensor
int flag=1;
int count;
int prev_sens_val =0;
int pawan=1;
void setup() {
  Serial.begin(9600);
  



  int hr_millis = millis();
  Serial.println("hr_millis");
  Serial.println(hr_millis);
  int c = millis();
  Serial.println(c);
  
  while(c<= (hr_millis+15000))
  {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
   
  //Serial.println(sensorValue);
  
  if(((sensorValue > prev_sens_val)&& (flag==1)))
  {
   // count=count+1;
    Serial.print("sensorValue = ");
    Serial.println(sensorValue);
    Serial.print("Your Heart beat count  = ");
    Serial.println(count);
    prev_sens_val = sensorValue;
    
    flag=0;
  }
 
  else if(((sensorValue <= prev_sens_val)&& (flag==0)))
  {
    count=count+1;
    Serial.println("heart relaxing...........");
    Serial.print("sensorValue = ");
    Serial.println(sensorValue);
    prev_sens_val = sensorValue;
    flag=1;
  }
  delay(125);
  c=millis();
 Serial.println(c);
  }
}
void loop() {}

