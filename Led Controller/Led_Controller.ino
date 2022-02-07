#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

bool b1c = false,
b2c = false,
b3c = false,

canLoop = true;
void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  WiFi.softAP("Led Controller");
  WiFi.softAPConfig(local_ip,gateway,subnet);
  delay(1000);
  server.on("/",HTTP_GET,handleRoot);
  server.on("/green",HTTP_GET,LedGreen);
  server.on("/yellow",HTTP_GET,LedYellow);
  server.on("/red",HTTP_GET,LedRed);
  server.begin();
  
  pinMode(D2,INPUT);
  pinMode(D3,INPUT);
  pinMode(D4,INPUT);
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
}
void loop() {
  if (!canLoop) return;
  bool b1 = digitalRead(D2)==1?true:false,
  b2 = digitalRead(D3)==1?true:false,
  b3 = digitalRead(D4)==1?true:false;
  if (b1 && !b1c) {
    b1c = true;
    digitalWrite(D5,!digitalRead(D5));
    digitalWrite(LED_BUILTIN,HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN,LOW);
  } else {
    b1c = false;
  }
  if (b2 && !b2c) {
    b2c = true;
    digitalWrite(D6,!digitalRead(D6));
    digitalWrite(LED_BUILTIN,HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN,LOW);
  } else {
    b2c = false;
  }
  if (b3 && !b3c) {
    b3c = true;
    digitalWrite(D7,!digitalRead(D7));
    digitalWrite(LED_BUILTIN,HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN,LOW);
  } else {
    b3c = false;
  }
  server.handleClient();
}

void LedGreen() {
  canLoop = false;
  digitalWrite(D5,!digitalRead(D5));
  digitalWrite(LED_BUILTIN,HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN,LOW);
  server.send(303,"text/html","<!DOCTYPE html><html lang=\"en\"><head><title>Success</title><meta http-equiv=\"refresh\" content=\"2; /\"/><meta name=\"viewport\" content=\"initial-scale=1,maximum-scale=3\"/></head><body><center><h1 style=\"font-family:sans-serif\">Success</h1></center></body></html>");
  canLoop = true;
}
void LedYellow() {
  canLoop = false;
  digitalWrite(D6,!digitalRead(D6));
  digitalWrite(LED_BUILTIN,HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN,LOW);
  server.send(303,"text/html","<!DOCTYPE html><html lang=\"en\"><head><title>Success</title><meta http-equiv=\"refresh\" content=\"2; /\"/><meta name=\"viewport\" content=\"initial-scale=1,maximum-scale=3\"/></head><body><center><h1 style=\"font-family:sans-serif\">Success</h1></center></body></html>");
  canLoop = true;
}
void LedRed() {
  canLoop = false;
  digitalWrite(D7,!digitalRead(D7));
  digitalWrite(LED_BUILTIN,HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN,LOW);
  server.send(303,"text/html","<!DOCTYPE html><html lang=\"en\"><head><title>Success</title><meta http-equiv=\"refresh\" content=\"2; /\"/><meta name=\"viewport\" content=\"initial-scale=1,maximum-scale=3\"/></head><body><center><h1 style=\"font-family:sans-serif\">Success</h1></center></body></html>");
  canLoop = true;
}
void handleRoot() {
  String ret = "<!DOCTYPE html";
  ret+="<html lang=\"en\"><head>";
  ret+="<title>Led Controller</title><meta http-equiv=\"refresh\" content=\"2; /\"/><meta name=\"viewport\" content=\"initial-scale=1,maximum-scale=3\"/><style type=\"text/css\">button { margin: 2px; border: 3px solid grey;border-radius: 5px;font-size: 30px; }</style></head><body style=\"margin-top:30%\"><center>";
  //btn green
  ret+="<button onclick=\"location='/green'\" style=\"background-color:";
  ret+=digitalRead(D5)==1?"lime;color:white":"white;color:lime";
  ret+="\">Green</button><br/>";
  //btn end
  //btn yellow
  ret+="<button onclick=\"location='/yellow'\" style=\"background-color:";
  ret+=digitalRead(D6)==1?"yellow;color:white":"white;color:yellow";
  ret+="\">Yellow</button><br/>";
  //btn end
  //btn red
  ret+="<button onclick=\"location='/red'\" style=\"background-color:";
  ret+=digitalRead(D7)==1?"red;color:white":"white;color:red";
  ret+="\">Red</button><br/>";
  //btn end
  ret+="</button><hr/><h3 style=\"font-family:sans-serif\">Led Controller</h3></body></html>";
  server.send(200,"text/html",ret);
}
