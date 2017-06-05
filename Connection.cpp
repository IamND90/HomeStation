#include "Connection.h"


void Connection::setup(){

  IPAddress ip(IP); // where xx is the desired IP Address
  IPAddress gateway(GATEWAY);
  IPAddress subnet(SUBNET); // set subnet mask to match your network
  

  Serial.print(F("\nSSID:"));
  Serial.println(LAN);
  Serial.print(F("IP:"));
  Serial.println(ip);
  Serial.print(F("GATEWAY:"));
  Serial.println(gateway);
  Serial.print(F("SUBNET:"));
  Serial.println(subnet);

  WiFi.config(ip, gateway, subnet);
  WiFi.begin(LAN, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Start the server
  server = new WiFiServer(PORT);
  server->begin();
  Serial.print(F("\nServer runs http://"));
  Serial.println(WiFi.localIP());
}

int Connection::listen(){

  client = server->available();
  if (!client) {
    return 0;
  }

  // Wait until the client sends some data
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  method = client.readStringUntil(' ');
  link = client.readStringUntil(' ');
  protocol = client.readStringUntil('\r');
  Serial.println(link);
  client.flush();
  return 1;
}
WiFiClient* Connection::getClient(){
  return &client;
}


void Connection::replyHTML(String buffer){

  client.println(F("HTTP/1.1 200 OK\nContent-Type: text/html\n\n<!DOCTYPE HTML>"));
  client.println("<html>");
  client.println(buffer);
  client.println("</html>");

}

void Connection::replyJSON(String buffer){

  client.println(F("HTTP/1.1 200 OK\nContent-Type: application/json\n\n"));
  client.println("{");
  client.println(buffer);
  client.println("}");
 
}
void Connection::replyRaw(){
   client.println(F("HTTP/1.1 404 Nf\nContent-Type: text/html\n\n"));
}
void Connection::replyRawJSON(){
   client.println(F("HTTP/1.1 404 Nf\nContent-Type: application/json\n\n"));
}
void Connection::print(char c){
   client.print(c);
}
void Connection::replyError(String buffer){

  client.println(F("HTTP/1.1 404 Nf\nContent-Type: text/html\n\n<!DOCTYPE HTML>"));
  client.println("<html>");
  client.println(buffer);
  client.println("</html>");

}
void Connection::reply404(){
  client.println(F("HTTP/1.1 404"));

}


String Connection::getData(){
  return method + " " + link + "\n" + protocol;
}
String Connection::getLink(){
  return link;
}
String Connection::getMethod(){
  return method;
}
void Connection::end(){
  client.flush();

  //client.stop();
}
