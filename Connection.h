#ifndef CONNECTION_H
#define CONNECTION_H
#include <Arduino.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


#ifndef LAN
  #define LAN "LANzelot-2.4GHZ"
#endif
#ifndef PASSWORD
  #define PASSWORD "Maienstrasse8"
#endif

#ifndef GATEWAY
  #define GATEWAY 185,110,187,1
#endif

#ifndef SUBNET
  #define SUBNET 255,255,255,0
#endif

#ifndef IP
  #define IP 185,110,187,191
#endif

#ifndef PORT
  #define PORT 80
#endif


class Connection {

private:
  WiFiServer* server;
  WiFiClient client;

  String method ="";
  String link = "";
  String protocol ="";

public:

  void setup();

  int listen();

  void end();

  String getData();
  String getLink();
  String getMethod();

  WiFiClient* getClient();

  void replyRaw();
  void replyRawJSON();
  void reply404();
  void replyError(String buffer);
  void replyHTML(String buffer);
  void replyJSON(String buffer);
  void print(char c);
};
#endif
