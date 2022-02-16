#include <Arduino.h>
#include <Hash.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <SimpleDHT.h>
#include <NewPing.h>
#include <ArduinoJson.h>
#include "html.h"
/*
By Jonas Krister Villamayor.
"Simple IoT" is a simple iot project
You can also customize the pins in the way you like it.
Known to work: WeMos D1 R1
*/
//pins
int dht11pin = D3;
long uptime[3] = {0,0,0};
int digitalpins[9] = {D6,D7,D8,D9,D10,D11,D12,D13,D14};
int analogpins[1] = {A0};

SimpleDHT11 dht11(dht11pin);
NewPing hcsr04(D4,D5,450);

ESP8266WebServer server(80);
WebSocketsServer wsserver = WebSocketsServer(81);
DNSServer dnsServer;

IPAddress local_ip(192,168,1,1);
IPAddress subnet(255,255,255,0);

String newconobj;

void setup() {
  pinMode(D2,OUTPUT);
  
  StaticJsonDocument<512> obj;
  obj["type"] = "init";
  JsonArray digitalpins1 = obj.createNestedArray("digitalPins");
  JsonArray analogpins1 = obj.createNestedArray("analogPins");
  for(int i=0;i<9;i++) {
    digitalpins1.add("D"+String(i+6));
    pinMode(digitalpins[i],INPUT);
  }
  for(int i=0;i<1;i++) {
    analogpins1.add("A"+String(i));
  }
  serializeJson(obj,newconobj);
  
  WiFi.softAP("Simple IoT");
  WiFi.softAPConfig(local_ip,local_ip,subnet);
  
  delay(1000);
  
  wsserver.begin();
  wsserver.onEvent(handleWS);
  
  server.on("/",handleRoot);
  server.on("/generate_204",handleRoot);
  server.on("/fwlink",handleRoot);
  server.begin();
  
dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", local_ip);
  MDNS.begin("simpleiot");
  MDNS.addService("http","tcp",80);
  MDNS.addService("ws","tcp",81);
}

long lastcall = millis();
long interval = 1500;//dht11

long lastcall1 = millis();
long interval1 = 200;//pin states

long lastcall2 = millis();
long interval2 = 1000;//general

long lastcall3 = millis();
long interval3 = 200;//HCSRO4

void loop() {
  //server
  wsserver.loop();
  server.handleClient();
  MDNS.update();
  dnsServer.processNextRequest();
  
  //general
  if (curcall - lastcall2 >= interval2) {
    uptime[0]++;
    if (uptime[0] == 60) {
      uptime[1]++;
      uptime[0] = 0;
      if (uptime[1]==60) {
        uptime[2]++;
        uptime[1] = 0;
      }
    }
    //check if anyone wants an update
    if (wsserver.connectedClients(false)<1) return;
    StaticJsonDocument<1000> doc;
    doc["type"] = "update";
    JsonArray upt = doc.createNestedArray("uptime");
    upt.add(uptime[0]);
    upt.add(uptime[1]);
    upt.add(uptime[2]);
    String ret;
    serializeJson(doc,ret);
    wsserver.broadcastTXT(ret);
    lastcall2 = curcall;
    curcall = millis();
  }
  //hcsr04
  long curcall = millis();
  if (curcall - lastcall3 >= interval3) {
    StaticJsonDocument<256> doc;
    int ping1 = hcsr04.ping_median(5);
    doc["type"] = "update-hcsr04";
    doc["cm"] = hcsr04.convert_cm(ping1);
    doc["in"] = hcsr04.convert_in(ping1);
    String ret;
    serializeJson(doc,ret);
    wsserver.broadcastTXT(ret);
    lastcall3 = curcall;
    curcall = millis();
  }
  //pins
  if (curcall - lastcall1 >= interval1) {
    StaticJsonDocument<512> doc;
    doc["type"] = "update-pin";
    JsonArray digitalpinstates = doc.createNestedArray("digitalPinStates");
    JsonArray analogpinstates = doc.createNestedArray("analogPinStates");
    for(int i=0;i<9;i++) {
      digitalpinstates.add(digitalRead(digitalpins[i])==HIGH?"HIGH":"LOW");
    }
    for(int i=0;i<1;i++) {
      analogpinstates.add(String(analogRead(analogpins[i])));
    }
    String ret;
    serializeJson(doc,ret);
    wsserver.broadcastTXT(ret);
    lastcall1 = curcall;
  }
  curcall = millis();
  //dht11
  if (curcall - lastcall >= interval) {
    StaticJsonDocument<100> doc;
    doc["type"] = "update-dht11";
    byte temp;
    byte humid;
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(&temp, &humid, NULL)) != SimpleDHTErrSuccess) {
      interval=500;
      return;
    }
    doc["temperature"] = (int)temp;
    doc["humidity"] = (int)humid;
    String ret;
    serializeJson(doc,ret);
    wsserver.broadcastTXT(ret);
    interval = 1500;
    lastcall = curcall; 
  }
}
void handleRoot() {
  server.send(200,"text/html",getIndexHtml());
}
void handleWS(uint8_t num,WStype_t type,uint8_t*payload,size_t length) {
  switch(type) {
    case WStype_CONNECTED:
      delay(50);
      wsserver.sendTXT(num,newconobj);
    break;
    case WStype_TEXT:
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc,String((char*)payload));
      if (!error) {
        JsonObject obj = doc.as<JsonObject>();
        if (!obj.containsKey("type")) {
          doc.clear();
          return;
        }
        if (obj["type"] == "activate" && obj.containsKey("hz")) {
          int hzs = (int)(obj["hz"].as<String>()).toInt();
          if (hzs < 0 || hzs > 2000) return;
          tone(D2,hzs);
        } else if (obj["type"] == "disable") {
          noTone(D2);
        }
      }
    break;
  }
}
