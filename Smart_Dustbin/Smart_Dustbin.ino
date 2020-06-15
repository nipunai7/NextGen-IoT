
#include "LiquidCrystal.h"
#include "WiFi.h"
#include "PubSubClient.h"

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int greenLedPin = 8;
const int yellowLedPin = 9;
const int redLedPin = 10;
const int buzzer = 6;

const int lightSensorPin = A0;
int analogValue = 0;
int temp = 0;
const int tempsensor = 34;

int inches = 0;
int cm = 0;

String apiKey = "DA0IPPIB7U92JAQ1";
const char* server = "api.thingspeak.com";
WiFiClient client;

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  
  return pulseIn(echoPin, HIGH);
}

void setup()
{
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Setup done");
    WiFi.begin("MNK-LAPTOP","Mnk@dialog");

    lcd.begin(16, 2);
    lcd.print("Weight of the Bin:");
    pinMode(greenLedPin, OUTPUT);
    pinMode(yellowLedPin,OUTPUT);
    pinMode(redLedPin,OUTPUT);
    pinMode(buzzer,OUTPUT);
}

void loop()
{

    // Wait a bit before scanning again
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(analogValue);
 
  
  analogValue = analogRead(lightSensorPin);
  temp = analogRead(tempsensor);
  temp = temp * 0.48828125;
  if(analogValue < 50){            
    
    lcd.setCursor(2, 1);
   lcd.print("KG");
    lcd.setCursor(4, 2);
    lcd.print(" ");
  }
  else if(analogValue >= 50 && analogValue <= 100){
    lcd.setCursor(2, 1);
    lcd.print("KG");
    lcd.setCursor(4, 1);
    lcd.print(" ");
  }
  else{
     lcd.setCursor(3, 1);
     lcd.print("KG");
     digitalWrite(buzzer,HIGH);
  }
 
  
  cm = 0.01723 * readUltrasonicDistance(7, 7);
  inches = (cm / 2.54);
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.println("cm");
  delay(100); // Wait for 100 millisecond(s)
  
  if (inches<33.33){
  digitalWrite(redLedPin, HIGH); 
  }
  else if(inches>33.33 && inches<66.66){
  digitalWrite(yellowLedPin, HIGH); 
  }
  else{
  digitalWrite(greenLedPin, HIGH); 
  }
  
  if (temp >= 45 && temp <=60 ){
  digitalWrite(greenLedPin, HIGH); 
  }
  
  else if (temp < 40 || temp > 60 ){
  digitalWrite(redLedPin, HIGH); 
  }
  
   delay(200);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(yellowLedPin, LOW);
  digitalWrite(redLedPin, LOW);
  digitalWrite(buzzer,LOW); 
 
  Serial.println("Connected to the WiFi network");
 
    delay(5000);

if (client.connect(server,80))
    {  
                            
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(temp);
    postStr += "&field2=";
    postStr += String(analogValue);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
 
    Serial.print("Temperature: ");
    Serial.println(temp);
    Serial.print("Weight:");
    Serial.print(analogValue);
    Serial.println(" KG");
                             
    Serial.println("%. Send to Thingspeak.");
}
    client.stop();
}
