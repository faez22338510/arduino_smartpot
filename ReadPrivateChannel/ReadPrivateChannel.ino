#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "DHT.h"

#define DHTTYPE DHT11
#define DHTPIN D2 
#if !defined(USE_WIFI101_SHIELD) && !defined(USE_ETHERNET_SHIELD) && !defined(ARDUINO_SAMD_MKR1000) && !defined(ARDUINO_AVR_YUN) && !defined(ARDUINO_ARCH_ESP8266) && !defined(ARDUINO_ARCH_ESP32)
  #error "Uncomment the #define for either USE_WIFI101_SHIELD or USE_ETHERNET_SHIELD"
#endif

char ssid[] = "Irancell-Z6000"; 
char pass[] = "faez22338510"; 
int status = WL_IDLE_STATUS;
WiFiClient  client;

DHT dht(DHTPIN, DHTTYPE);

unsigned long lastPostConnectionTime = 0;
unsigned long lastRiciveConnectionTime = 0;  
const unsigned long postingInterval = 10L * 1000L; // Post data every 10 seconds.
const unsigned long RiciveInterval = 5L * 1000L; // Post data every 10 seconds.

unsigned long myReadChannelNumber = 521815;
const char * myReadAPIKey = "CXOHVN4Z7SCYQBU5";
unsigned long myWriteChannelNumber = 516542;
const char * myWriteAPIKey = "H499QAGGZLC2SHIZ";

void setup() {
  Serial.begin(115200);
  pinMode(D1,OUTPUT);
  dht.begin();
  WiFi.begin(ssid, pass);
  ThingSpeak.begin(client);
}

void loop() {
  // Read the latest value from field 1 of channel 31461
  if (millis() - lastPostConnectionTime > postingInterval) {
    float t = dht.readTemperature(); 
    float h = dht.readHumidity(); 
    String data = String("field1=" + String(t, DEC) + "&field2=" + String(h, DEC));
    Serial.println(data); 
    ThingSpeak.setField(1,t);
    ThingSpeak.setField(2,h);
    ThingSpeak.writeFields(myWriteChannelNumber, myWriteAPIKey);  


    lastPostConnectionTime = millis();
  }
  if (millis() - lastRiciveConnectionTime > RiciveInterval) {
    float light = ThingSpeak.readFloatField(myReadChannelNumber, 1, myReadAPIKey);
    if(light == 1){
      digitalWrite(D1,1);
    }
    else{
      digitalWrite(D1,0);
    }
    Serial.print("Latest Conroller value is: "); 
    Serial.print(light);
    Serial.println(""); 
    lastRiciveConnectionTime = millis();
  }
//RiciveInterval
}
