#include <virtuabotixRTC.h>
//#include <NewSoftSerial.h>
// Creation of the Real Time Clock Object
#include <Wire.h>  
#include "time.h"                    
#include <LiquidCrystal_I2C.h>   
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 
#include <Servo.h>
virtuabotixRTC myRTC(19, 18,5);
int lcdColumns = 20;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27,lcdColumns,lcdRows); 
String var1 ;  
String var2 ;
String messageToScroll = "Welcome To PetFeeder";
/*****************MQTT********************/
const char* mqtt_server = "192.168.137.185";
//const char* humidity_topic = "home/livingroom/humidity";
const char* pot_topic = "petfeeder/distance";
const char* eau_topic = "petfeeder/eau";
const char* mqtt_username = "amine"; // MQTT username
const char* mqtt_password = "amine"; // MQTT password
const char* clientID = "client_petfeeder"; // MQTT client ID
//const char* clientID = "client_livingroom"; // MQTT client ID
WiFiClient wifiClient;
PubSubClient client(mqtt_server,1883, wifiClient);
long lastMsg = 0;
char msg[50];
int value = 0;
String msgStr = ""; 
/**********HCR****************/
#define trigPin 2
#define echoPin 15
float duration, distance;
/**********NTP***************/
const char* ssid     = "Amine";
const char* password = "amine123";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
/***********LDR+led*****************/
const int Lampe = 27;   //the number of the LED pin
const int ldrPin = 10;  //the number of the LDR pin
/**********servo*********/
Servo servo_test;      //initialize a servo object for the connected servo  
                
 int angle = 0;    
 int potentio = A0;      // initialize the A0analog pin for potentiometer
 int pos = 0;

void setup()  {
 Serial.begin(115200);
  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  myRTC.setDS1302Time(30, 48, 21, 20, 20, 02, 2022);
  /********lcd********/
lcd.init();                    
// Print a message to the LCD.
  lcd.backlight();                
  lcd.setCursor(0,0);           
  scrollText(1, messageToScroll, 250, lcdColumns);
/**************NTP**************/
 // Connect to Wi-Fi
 // Serial.print("Connecting to ");
  //Serial.println(ssid);
  lcd.clear();
  lcd.setCursor(0,11);
  lcd.print("Wifi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
   // Serial.print(".");
  }
 // Serial.println("");
  //Serial.println("WiFi connected.");
  lcd.clear();
  lcd.setCursor(0,11);
  lcd.print("Wifi: Y");
  delay(2000);
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
/********************MQTT***************/
  if(client.connect(clientID, mqtt_username, mqtt_password)) {
    lcd.setCursor(4,8);
    lcd.print("MQTT: Y");
  }
  else {
    lcd.setCursor(4,8);
    lcd.print("MQTT: N");
  }
  
  /**********HCR***************/
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  /******************ldr+led***************/
  pinMode(Lampe, OUTPUT);  //initialize the LED pin as an output
  pinMode(ldrPin, INPUT);   //initialize the LDR pin as an input
  /**************servo moteur**********/
 servo_test.attach(13); 
  servo_test.write(angle);
  // attach the signal pin of servo to pin9 of arduino
 

}
void loop()  {
  /*************************/
  lcd.clear();
 
  // This allows for the update of variables for time or accessing the individual elements.
  myRTC.updateTime();
  // Start printing elements as individuals
 // Serial.print("Current Date / Time: ");
  //lcd.print("Current Date / Time: ");
 // Serial.print(myRTC.dayofmonth);
 // Serial.print("/");
 
 // Serial.print(myRTC.month);
 // Serial.print("/");
 // Serial.print(myRTC.year);
   //lcd.setCursor(0,1);
//  Serial.print("  ");
 // Serial.print(myRTC.hours);
  //lcd.print(myRTC.hours);
 // Serial.print(":");
  //lcd.print(":");
 // Serial.print(myRTC.minutes);
  //lcd.print(myRTC.minutes);
 // Serial.print(":");
  //lcd.print(":");
 // Serial.println(myRTC.seconds);
  //lcd.println(myRTC.seconds);
  // Delay so the program doesn't print non-stop
   /*************************NTP***************************/
  printLocalTime();
  
  /***********HCR*****************/
   // Write a pulse to the HC-SR04 Trigger Pin
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  
  duration = pulseIn(echoPin, HIGH); 
  distance = (duration / 2) * 0.0343;
  
 
 // Serial.print("Distance = ");
  if (distance >= 100) {
   //  Serial.println("Out of range");
   lcd.clear();
   lcd.setCursor(1,1);
   lcd.print("Alert Stock");
  }
  else {
    lcd.setCursor(1,1);
    lcd.print(distance);
  }
  
  /*******************ldr+led**********************/
   float
   ldrStatus = analogRead(ldrPin); 
  lcd.setCursor(2,2);
  lcd.print(ldrStatus);
   if (ldrStatus <=3600) {
   digitalWrite(Lampe, HIGH);              
  //  Serial.println("LDR is DARK, LED is ON");
  lcd.setCursor(2,2);
  lcd.print("Jour");
   }
  else {
    digitalWrite(Lampe, LOW);          //turn LED off
  //  Serial.println("Led OFF");
  lcd.setCursor(1,2);
  lcd.print("Nuit");
  }
  msgStr = "{\"action\": \"notification/insert\",\"deviceId\": \"s3s9TFhT9WbDsA0CxlWeAKuZykjcmO6PoxK6\",\"notification\":{\"notification\": \"Distance\",\"parameters\":{\"distance\":" + String(distance) + ",\"ldrStatus\":" + String(ldrStatus) + "}}}";
byte arrSize = msgStr.length() + 1;
    char msg[arrSize];
 
    //Serial.print("PUBLISH DATA:");
    //Serial.println(msgStr);
    msgStr.toCharArray(msg, arrSize);
    String hs="pot"+String((float)distance)+"C";
    if(client.publish(pot_topic, msg)) {
    //Serial.println("Distance sent!");
    }
    else{
    //Serial.println("Potentiometre failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(pot_topic, msg);
    }
    ///////////////////////////////////////////////////////////////
  // if (client.publish(eau_topic, String(ldrStatus).c_str())) {
   // Serial.println("Eau sent!");
  //}
    // disconnect from the MQTT broker
     msgStr = "";
    delay(50);
  delay(10000);
 
 }
  
 void servo() {
  for(angle = 0; angle < 180; angle += 1)   // command to move from 0 degrees to 180 degrees 
  {                                  
    servo_test.write(angle);                 //command to rotate the servo to the specified angle
    delay(15);                       
  } 
 
  delay(2000);
  
  for(angle = 180; angle>=1; angle-=5)     // command to move from 180 degrees to 0 degrees 
  {                                
    servo_test.write(angle);              //command to rotate the servo to the specified angle
    delay(5);                       
  } 

    delay(1000*120);
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
  //  Serial.println("Failed to obtain time");
    return;
  }
  
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
 // Serial.println("Time variables");
  char timeMinute[3];
  char timeHour[3];
  char timeSeconde[3];
  strftime(timeHour,3, "%H", &timeinfo);
  strftime(timeMinute,3, "%M", &timeinfo);
  strftime(timeSeconde,3, "%S", &timeinfo);
  lcd.setCursor(0,16);
   lcd.print(timeHour);
   lcd.print(":");
   lcd.print(timeMinute);
   lcd.print(":");
   lcd.print(timeSeconde);
   //delay(1000);
  //Serial.println(timeHour);
  //Serial.println(timeMinute);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
 // Serial.println(timeWeekDay);
 // Serial.println();
  if (strcmp(timeHour,"15") == 0 && strcmp(timeMinute,"74") == 0 )
    {
     
    lcd.clear();
    lcd.setCursor(3,3);
    lcd.print("Petfeeder On---!");  
    delay(2000); 
    var1 = "ON" ;
    var2 = "OFF" ;
    Serial.println(var1 + '&' + var2);
    servo();
      
    }
}

void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message; 
  } 
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}
void reconnect() {
  while (!client.connected()) {
    if (client.connect(clientID, mqtt_username, mqtt_password)) {
      //Serial.println("MQTT connected");
      client.subscribe(pot_topic);
      //Serial.println("Topic Subscribed");
    }
    else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      //delay(5000);  // wait 5sec and retry
    }
  }
}
