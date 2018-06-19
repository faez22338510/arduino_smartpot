#include <ESP8266WiFi.h> // TEST
#include <PubSubClient.h>
#include "DHT.h"


#define DHTTYPE DHT11
#define DHTPIN D2 
#define LIGHTPIN A0 // Analog light sensor connected to analog pin A0.

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor.

char ssid[] = "Irancell-Z6000"; 
char pass[] = "faez22338510"; 
char mqttUserName[] = "faez22338510"; 
char mqttPass[] = "Faez22338510";  
char writeAPIKey[] = "H499QAGGZLC2SHIZ";  
char readAPIKey[] = "HDX272C9ML6S0I9B"; 
long channelID = 516542;

static const char alphanum[] ="0123456789"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz";  // For random generation of client ID.

WiFiClient client;                         // Initialize the Wifi client library.
PubSubClient mqttClient(client);           // Initialize the PuBSubClient library.
const char* server = "mqtt.thingspeak.com"; 

unsigned long lastConnectionTime = 0; 
const unsigned long postingInterval = 10L * 1000L; // Post data every 20 seconds.


//------------------------------------------------------------------------
//SETUP
//------------------------------------------------------------------------

void setup() {
  
  Serial.begin(115200);
  int status = WL_IDLE_STATUS;  // Set a temporary WiFi status.
  setup_wifi();
  dht.begin();
  
  mqttClient.setServer(server, 1883);   // Set the MQTT broker details.
  mqttClient.setCallback(mqttCallback);
}

//------------------------------------------------------------------------
//MAIN LOOP
//------------------------------------------------------------------------

void loop() {

 // Reconnect if MQTT client is not connected.
  if (!mqttClient.connected()) 
  {
    reconnect();
  }

  mqttClient.loop();   // Call the loop continuously to establish connection to the server.

  // If interval time has passed since the last connection, publish data to ThingSpeak.
  if (millis() - lastConnectionTime > postingInterval) 
  {
    mqttpublish();
  }
}

//------------------------------------------------------------------------
//WIFI SETUP
//------------------------------------------------------------------------

void setup_wifi(){
  delay(10);

  Serial.print("connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid,pass);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.print("connect to Wifi \n IP Address : ");
  Serial.print(WiFi.localIP());
  WiFi.printDiag(Serial);
}

//------------------------------------------------------------------------
//RECONNECT
//------------------------------------------------------------------------

void reconnect() 
{
  char clientID[10];

  // Loop until reconnected.
  while (!mqttClient.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Generate ClientID
    for (int i = 0; i < 8; i++) {
        clientID[i] = alphanum[random(51)];
    }
    clientID[8]='\0';

    // Connect to the MQTT broker
    if (mqttClient.connect(clientID,mqttUserName,mqttPass)) 
    {
      Serial.print("Connected with Client ID:  ");
      Serial.print(String(clientID));
      Serial.print(", Username: ");
      Serial.print(mqttUserName);
      Serial.print(" , Passwword: ");
      Serial.println(mqttPass);
    } else 
    {
      Serial.print("failed, rc=");
      // Print to know why the connection failed.
      // See http://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
    delay(1000);
  }
  if(mqttClient.connected()){
      Serial.println("Connected ****************");
      String topicString ="channels/" + String( channelID ) + "/publish/"+String(writeAPIKey);
      String subscribeTopic = "channels/" + String( channelID ) + "/subscribe/fields/field3/"+String(readAPIKey);
      //String subscribeTopic = "sadfasdfqwec";
      int length=subscribeTopic.length();
      //int length=topicString.length();
      char topicBuffer[length];
      subscribeTopic.toCharArray(topicBuffer,length+1);
      //topicString.toCharArray(topicBuffer,length+1);
      //Serial.println(topicBuffer);
      mqttClient.subscribe("channels/516542/subscribe/fields/field2/HDX272C9ML6S0I9B");
    }
}

//------------------------------------------------------------------------
//PUBLISH
//------------------------------------------------------------------------

void mqttpublish() {
  
  float t = dht.readTemperature(); 
  float h = dht.readHumidity();  // Read humidity from DHT sensor.
  //int lightLevel = analogRead(LIGHTPIN); // Read from light sensor
  
  // Create data string to send to ThingSpeak
  String data = String("field1=" + String(t, DEC) + "&field2=" + String(h, DEC));
  int length = data.length();
  char msgBuffer[length];
  data.toCharArray(msgBuffer,length+1);
  Serial.println(msgBuffer);
  
  // Create a topic string and publish data to ThingSpeak channel feed. 
  String topicString ="channels/" + String( channelID ) + "/publish/"+String(writeAPIKey);
  length=topicString.length();
  char topicBuffer[length];
  topicString.toCharArray(topicBuffer,length+1);
 
  //Serial.println(topicBuffer);
  mqttClient.publish( topicBuffer, msgBuffer );

  lastConnectionTime = millis();
}


//------------------------------------------------------------------------
//MQTT CALLBACK FUNCTION
//------------------------------------------------------------------------

void mqttCallback(char* topic, byte* message, unsigned int length){
  for(int i = 0;i<length;i++){
    Serial.print((char)message[i]);
  }
  Serial.println(topic);
  // if(strcmp(topic,"temp/request") == 0){

  // }
}