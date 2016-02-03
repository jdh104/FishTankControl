
byte therm = 1, heater = 5;
int normal;

void setup(){
  Serial.begin(9600);
  pinMode(heater,OUTPUT);
  
  normal = analogRead(therm) + 20;
}

void loop(){
  if (analogRead(therm) > normal){
    digitalWrite(heater,HIGH);
  } else {
    digitalWrite(heater,LOW);
  }
  Serial.println(analogRead(therm));
}
