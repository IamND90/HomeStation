#include "FileManager.h"

FileManager::FileManager(Connection* c){
  connection =c;
 
  pinMode(A0,INPUT);
  Serial.println(SD.begin(SD_PIN) ?F("SD:OK"): F("SD:FAIL"));
  bmp.initialize();
  Serial.println(dht.isOk() ?F("DHT:OK"): F("DHT:FAIL"));
  Serial.println(bmp.testConnection() ?F("BMP:OK"): F("BMP:FAIL"));
}

bool FileManager::requestData(String name){

  String data  ="\"No data\"";
  if (name == "dh11") data = readDH11();
  else if (name == "bmp1") data = readBMP180();
  else if (name == "light") {
    connection->replyRawJSON();
    scanLight();
    return true;
  }
  else if (name == "all") {
    connection->replyRawJSON();
    connection->getClient()->print(F("{"));
    scanLight();
    connection->getClient()->print(F(","));
    connection->getClient()->print(readBMP180());
    connection->getClient()->print(F(","));
    connection->getClient()->print(readDH11());
    connection->getClient()->print(F("}"));
    return true;
  }
 
  connection->replyJSON(data);
  return true;

}
bool FileManager::requestFile(String name){
  File index = SD.open(name);
  if (index) {
    // read from the file until there's nothing else in it:
    connection->replyRaw();
    while (index.available()) {
      connection->print(index.read());
    }
    // close the file:
    index.close();
    return true;
  }
  return false;
}

bool FileManager::createFile(String name){
  File index = SD.open(name, FILE_WRITE);
  index.close();
  return SD.exists(name);
}

bool FileManager::writeFile(String name, String text){
  File index = SD.open(name, FILE_WRITE);
  index.println(text);
  index.close();
  return false;
}
String FileManager::readBMP180(){
  bmp.setControl(BMP085_MODE_TEMPERATURE);
    
  // wait appropriate time for conversion (4.5ms delay)
  lastMicros = micros();
  while (micros() - lastMicros < bmp.getMeasureDelayMicroseconds());

  // read calibrated temperature value in degrees Celsius
  float t = bmp.getTemperatureC();

  // request pressure (3x oversampling mode, high detail, 23.5ms delay)
  bmp.setControl(BMP085_MODE_PRESSURE_3);
  while (micros() - lastMicros < bmp.getMeasureDelayMicroseconds());

  // read calibrated pressure value in Pascals (Pa)
  long p = (long) bmp.getPressure();
  float altitude = bmp.getAltitude(p); //Uncompensated caculation - in Meters
  return String("\"temp_bmp\":" +  String(t) + ",\"pressure\":" + String(p)  + ",\"altitude\":" + String(altitude) );
}

String FileManager::readDH11(){
  int chk = dht.read();

  switch (chk)
  {
    case DHTLIB_OK:
      return String("\"temperature\":" +  dht.temperature + ",\"humidity\":" + dht.humidity );
     default:
		  return F("\"dh11\":\"Error\"");
  }
}
bool FileManager::scanLight(){
  connection->getClient()->print(F("\"light\":\""));
  int percent = (int)(analogRead(A0)/8.20f);
  connection->getClient()->print(percent);
  connection->getClient()->print(F("%\""));
  return true;  
}

bool FileManager::SDStatus(){
  return sdStatus;
}
bool FileManager::BMP180Status(){
  return bmp.testConnection();
}
bool FileManager::DHT11Status(){
  return dht.isOk();
}
