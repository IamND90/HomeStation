#include <SPI.h>
#include <SD.h>


#include "Connection.h"
#include "FileManager.h"

const static int8_t IOPins[] = {D10,D1,D8,D11};

Connection connection;
FileManager* fm;

void setup() {
  //Initialize serial and wait for port to open:
  
  Serial.begin(115200);
  while(!Serial){}
  //delay(400);
  
  connection.setup();
  fm = new FileManager(&connection);
  Serial.println(F("Setup Done"));
}

void loop() {

  // if there's data available, read a packet
  int status = connection.listen();
  if (status) {
    Serial.println(F("InComing"));
    parseReply();
  }
  else if ( status == -1) {
    Serial.println(F("TimeOut"));
  }
  
  connection.end();
}

void parseReply(){
  String link = connection.getLink();
  Serial.println(link);
  bool request = false;
  if (link == "/") {
    request = printStatus();
  }
  else if (link == "/i2c") {
    connection.replyRawJSON();
    request = scanI2C();
  }
  else if (link == "/pins") {
    connection.replyRawJSON();
    request = scanPins();
  }
  else if (link.startsWith("/data/")) {
    request = fm->requestData(link.substring(6));
  }
  
  else {
    String file = link.substring(1)+".txt";
    request = fm->requestFile(file);
  }
  
  if (!request) connection.replyJSON(F("\"Status\":\"Not found\""));
}

bool scanI2C(){
  int nDevices;
  nDevices = 0;
  connection.getClient()->print("\"sda\": ");
  connection.getClient()->println(PIN_WIRE_SDA);
  connection.getClient()->print(",\"sdl\": ");
  connection.getClient()->println(PIN_WIRE_SDL);
  for (uint8_t address = 1; address < 127; address++)
  {
    // The i2c scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    int error = Wire.endTransmission();
    if (error == 0 || error == 4){
      connection.getClient()->print(",\"0x");
      if (address < 16) {
        connection.getClient()->print("0");
      }
      connection.getClient()->print(address, HEX);
      if (error == 0)
      {
        connection.getClient()->println("\":\"Yes\"");
        nDevices++;
      }
      else if (error == 4)
      {
        connection.getClient()->println("\":\"Error\"");
      }
    }
  }
  

  connection.getClient()->print(",\"total\": ");
  connection.getClient()->println(nDevices);

  connection.getClient()->println("}"); 
  return true; 
}

bool printStatus(){
  //connection.getClient()->print(F("<script>new Date().getTime()</script>"));
  connection.getClient()->print(F("{\"sdcard\":\""));
  connection.getClient()->print(fm->SDStatus() ? "OK\"" : "FAIL\"");
  connection.getClient()->print(F(",\"BMP180\":\""));
  connection.getClient()->print(fm->BMP180Status() ? "OK\"" : "FAIL\"");
  connection.getClient()->print(F(",\"DHT11\":\""));
  connection.getClient()->print(fm->DHT11Status() ? "OK\"" : "FAIL\"");
   connection.getClient()->print(F(",\"sda\": "));
  connection.getClient()->print(PIN_WIRE_SDA);
  connection.getClient()->print(F(",\"sdl\": "));
  connection.getClient()->print(PIN_WIRE_SDL);
  connection.getClient()->print(F(",\"IOPins\": \""));
  for (uint8_t address = 0; address < sizeof(IOPins); address++)
  {
    connection.getClient()->print(IOPins[address]);
    if(address < sizeof(IOPins)-1) connection.getClient()->print(",");
  }
  connection.getClient()->print("\"}");
  return true;  
}
bool scanPins(){

  connection.replyRawJSON();
  connection.getClient()->println("{\"pins\":{");
  
  for (uint8_t address = 0; address < sizeof(IOPins); address++)
  {
    int8_t pin = IOPins[address];
    pinMode(pin,INPUT);
   
    connection.getClient()->print(F("\"GPIO_"));
    connection.getClient()->print(pin);
    connection.getClient()->println(digitalRead(pin) == HIGH ? F("\": \"HIGH\",") : F(":\"LOW\","));
  }
  connection.getClient()->println("}}");
  connection.end();
  return true;
}
