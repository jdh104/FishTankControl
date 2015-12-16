void setup() {
  Serial.begin(9600);                           //use a baud rate of 9600 bps
  pinMode(1,OUTPUT);                            //set pin1 for output (pin1=TX)
  Serial.write(12);                             //clear screen + move to top left
  Serial.write(128);                            //move cursor to (0,0)
  Serial.write("CONDUCTIVITY SENSOR");          //print string starting at (0,0)
  Serial.write(152);                            //move cursor to (1,4)
  Serial.write("CALIBRATION");                  //start string at (1,4)
  Serial.write(189);                            //move cursor to (3,1)
  Serial.write("analog input = ");                //start string at (3,1);
  Serial.write(22);                             //turn cursor off to keep screen clean
}

void loop() {
  for (int i=1; i<=15;i++)
  {
    Serial.write(204);                        //move cursor to (3,16)
    Serial.print(i);                          //******print i at current position******
    Serial.write("  ");                       //print blanks to cover previous printing
    delay(1000);                              //delay 1 sec between numbers
  }
}
