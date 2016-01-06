  //This should make the on-board LED blink once every second
unsigned long time;

void setup(){
  Serial.begin(9600);
  pinMode(13,OUTPUT);
}

void loop(){
  time = millis()%2000;
  Serial.println(time);
  if (time>490 && time<510){
    digitalWrite(13,HIGH);
  }
  if (time>1490 && time<1510){
    digitalWrite(13,LOW);
  }
  
  delay(1);
}
