#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Amine";
const char* wifi_password = "amine123";

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.137.176";
//const char* humidity_topic = "home/livingroom/humidity";
const char* pot_topic = "petfeeder/potentiometre";
const char* mqtt_username = "amine"; // MQTT username
const char* mqtt_password = "amine"; // MQTT password
const char* clientID = "client_petfeeder"; // MQTT client ID
//const char* clientID = "client_livingroom"; // MQTT client ID
WiFiClient wifiClient;
PubSubClient client(mqtt_server,1883, wifiClient);
long lastMsg = 0;
char msg[50];
int value = 0;
const int potPin = 35;

// variable for storing the potentiometer value
float potValue = 0;

void setup() {
  Serial.begin(115200);
   Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if(client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}
/*
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
}
*/

void loop() {
  // Reading potentiometer value
//   connect_MQTT();
  Serial.setTimeout(2000);
    
    // Temperature in Celsius
    potValue = analogRead(potPin);
    Serial.println(potValue);
  

    String hs="pot"+String((float)potValue)+"C";
    if(client.publish(pot_topic, String(potValue).c_str())) {
    Serial.println("Potentiometre sent!");
    }
    else{
    Serial.println("Potentiometre failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(pot_topic, String(potValue).c_str());
    }
    ///////////////////////////////////////////////////////////////
   //if (client.publish(humidity_topic, String(h).c_str())) {
    //Serial.println("Humidity sent!");
  //}
    // disconnect from the MQTT broker
  delay(10000); 
}
