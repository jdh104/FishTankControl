
#define THERM 1

void setup(){
  Serial.begin(9600);
  for (byte i=0; i<20; i++){
    Serial.println(analogRead(THERM));
    delay(1000);
  }
}

void loop(){
  delay(1000000);
}
