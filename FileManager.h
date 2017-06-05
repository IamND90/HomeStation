#ifndef FILEMANGER_H
#define FILEMANGER_H
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "dht11.h"
#include "BMP085.h"
#include "Connection.h"

#define SD_PIN 2


class FileManager {

private:
  bool sdStatus = true;
  int32_t lastMicros;
  Connection* connection;

  dht11 dht;
  BMP085 bmp;

  String readDH11();
  String readBMP180();
  bool scanLight();
  
public:
  FileManager(Connection* c);

  bool SDStatus();
  bool BMP180Status();
  bool DHT11Status();
  
  bool requestData(String name);
  bool requestFile(String name);

  bool createFile(String file);
  bool writeFile(String file, String text);
};
#endif
