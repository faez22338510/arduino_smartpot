#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

#if !defined(USE_WIFI101_SHIELD) && !defined(USE_ETHERNET_SHIELD) && !defined(ARDUINO_SAMD_MKR1000) && !defined(ARDUINO_AVR_YUN) && !defined(ARDUINO_ARCH_ESP8266) && !defined(ARDUINO_ARCH_ESP32)
  #error "Uncomment the #define for either USE_WIFI101_SHIELD or USE_ETHERNET_SHIELD"
#endif

char ssid[] = "Irancell-Z6000"; 
char pass[] = "faez22338510"; 
int status = WL_IDLE_STATUS;
WiFiClient  client;


unsigned long myChannelNumber = 516542;
const char * myReadAPIKey = "HDX272C9ML6S0I9B";

void setup() {


  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  ThingSpeak.begin(client);
}

void loop() {
  // Read the latest value from field 1 of channel 31461
  float temp = ThingSpeak.readFloatField(myChannelNumber, 1, myReadAPIKey);
  
  Serial.print("Latest TEMP is: "); 
  Serial.print(temp);
  Serial.println("temp"); 

  delay(10000);
}
