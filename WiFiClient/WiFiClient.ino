

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  pinMode(5,OUTPUT);
  digitalWrite(15,LOW);
  delay(400);

}

void loop() {
  // put your main code here, to run repeatedly:
  if( Serial.available()){
      char c =  Serial.read();

      if( c == 'a') {
         digitalWrite(5,HIGH);
      }
      if( c == 'b') {
         digitalWrite(5,LOW);
      }
   }
}
