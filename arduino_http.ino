#include <ESP8266WiFi.h>
#include <WiFiConnector.h>
WiFiConnector *wifi;

#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <ESP_Adafruit_SSD1306.h>
//#include <DHT.h>
#include "declare.h"
#include "utility.c"
#include <ArduinoJson.h>

#define WIFI_SSID        "YOUR SSID"
#define WIFI_PASSPHARSE  "YOUR PWD"
#include "init_wifi.h"


void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("BEGIN");
}

void setup()   {                
  
  uint8_t SMARTCONFIG_PIN = 0;
  init_hardware();
  init_wifi(SMARTCONFIG_PIN);
  
  pinMode(SW_MODE,INPUT);
  
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  pinMode(output1, OUTPUT);
  digitalWrite(output1, LOW);

  pinMode(output2, OUTPUT);
  digitalWrite(output2, LOW);
 
  
  
  
  // WiFi Connect
  Serial.print("CONNECTING TO ");
  Serial.println(wifi->SSID() + ", " + wifi->psk());

  wifi->on_connecting([&](const void* message)
  {
    char buffer[70];
    //lp++;
    //if(lp>=20) break;
  });

  
  
  wifi->on_smartconfig_waiting([&](const void* message)
  {
       
  });
  
  

  wifi->on_connected([&](const void* message)
  {
    // Print the IP address
    Serial.print("WIFI CONNECTED ");
    Serial.println(WiFi.localIP());
  });

  wifi->connect();
  
  
  //WiFi.begin(ssid, password);
  Serial.println("");
 
  

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {    
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
    lp++;
    if(lp>=20) break;
  }
  
  
  Serial.println("");
  Serial.print("Connected to ");
  //Serial.println(ssid);
  Serial.print("IP address: ");  Serial.println(WiFi.localIP());
  
  display.display();
  delay(2000);
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}//setup

/*
 * *********************************
 * *********************************
 */

void loop() {
 
  buttonState = digitalRead(SW_MODE); 
  if(buttonState == LOW){   
    imode++;   
    if(imode >= 2) imode = 0; 
    Serial.println("Mode "); 
    Serial.print(imode);
  }
  
 
   server.handleClient();  
   unsigned long currentMillis = millis();
   unsigned long currentMillis_led = millis();
    
  if(currentMillis - previousMillis >= interval) {     
      previousMillis = currentMillis;
    
            static const char* host = "api.thingspeak.com";
            static const char* apiKey = "XIE7DWLDBZZ79S5Q";
          
            // Use WiFiClient class to create TCP connections
            WiFiClient client;
            const int httpPort = 80;
            if (!client.connect(host, httpPort)) {
              Serial.println("connection failed");
              return;
            }
            
            url = "/channels/38675/feeds/last";                   
            Serial.print("Requesting URL: ");
            Serial.println(url);
                 
            client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                         "Host: " + host + "\r\n" +
                         "Connection: close\r\n\r\n");
          
            while(!client.available()) {
              delay(200); 
            }             
          
            while(client.available()){
              strData = client.readStringUntil('\n');
              Serial.println(strData);
            }
                
            //Control OutPut   
    
    if(strData != "") {
      String jsonReq = strData; //if send json data, it have only 1 argument
      int size = jsonReq.length() + 1;
      char json[size];
      jsonReq.toCharArray(json, size);
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json_parsed = jsonBuffer.parseObject(json);

        if (json_parsed.containsKey("field1")){
            String field1 = json_parsed["field1"];
  
            Serial.println(field1);
            
            if (field1 == "1" || field1 == "1.0" || field1 == "1.00" ){
              digitalWrite(output1,HIGH);
              digitalWrite(BUILTIN_LED, LOW); 
              display.println("Out 1 ON"); 
              //display.setTextColor(BLACK, WHITE); // 'inverted' text            
            }
           
            else if (field1 == "0" || field1 == "-1" || field1 == "0.1" || field1 == "0.10"){
              digitalWrite(output1,LOW);
              digitalWrite(BUILTIN_LED, HIGH);
              display.println("Out 1 OFF");                   
            }
            else{
              Serial.println("UNKNOWN");
              display.println("Out 1 ??"); 
            }      
        }


        if (json_parsed.containsKey("field2")){     
            String field2 = json_parsed["field2"];  
            Serial.println(field2);
            
            if (field2 == "1" || field2 == "1.0" || field2 == "1.00" ){
              digitalWrite(output2,HIGH);
              digitalWrite(BUILTIN_LED, LOW); 
              display.println("Out 2 ON");                    
            }
           
            else if (field2 == "0" || field2 == "-1" || field2 == "0.1" || field2 == "0.10"){
              digitalWrite(output2,LOW);
              digitalWrite(BUILTIN_LED, HIGH);
              display.println("Out 2 OFF");      
            }
            else{
              Serial.println("UNKNOWN");
              display.println("Out 2 ??"); 
            }      
        }      
      }    
  } 
}//Loop



void handleRoot() {
  digitalWrite(BUILTIN_LED, 1);
  server.send(200, "text/plain", "hello from esp8266! 555");
  digitalWrite(BUILTIN_LED, 0);
}

void handleNotFound(){
  digitalWrite(BUILTIN_LED, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(BUILTIN_LED, 0);
}

void uploadThingsSpeak(float d1,float d2) {
  static const char* host = "api.thingspeak.com";
  static const char* apiKey = "XIE7DWLDBZZ79S5Q";

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  while(client.available()){
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }
  
  String url = "/update/";
  //  url += streamId;
  url += "?key=";
  url += apiKey;
  url += "&field1=";
  url += d1;
  url += "&field2=";
  url += d2;

  Serial.print("Requesting URL: ");
  Serial.println(url);


  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  while(!client.available()) {
    delay(200); 
  }             

  while(client.available()){
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }
               
}


