
const int                          //These const variables should only be changed if the circuit is changed
          CSENSORPOWER=3,          //Digital Pin that provides power to Conductivity Sensor (CS)
          TXPIN=1,                 //Digital Pin that is used to transmit data via "Serial"
          CSENSORINPUT=0;          //Analog Pin that is used to read in a value from the CS

void setup(){
  Serial.begin(9600);
  pinMode(TXPIN,OUTPUT);
  pinMode(CSENSORPOWER,OUTPUT);
  formatLCD(true,false,true);
  clearLCD();
  backLightLCD(true);
}

void loop(){

}

int readConductivity(){                        //Usage example: int saltLevel = readConductivity();
  digitalWrite(CSENSORPOWER,HIGH);
  delay(100);
  int level = analogRead(CSENSORINPUT);
  digitalWrite(CSENSORPOWER, LOW);             //Turn off power ASAP to prevent corrosion
  return level;
}

void formatLCD(boolean display, boolean cursor, boolean blink){
  if (display){                                // Usage example: formatLCD(true,false,true);
    if (cursor){
      if (blink){
        Serial.write(25);                      // (true,true,true): ALL ON
      } else {
        Serial.write(24);                      // (true,true,false): Display & Cursor ON, blink OFF
      }
    } else {
      if (blink) {
        Serial.write(23);                      // (true,false,true): Display ON, Cursor OFF, blink ON
      } else {
        Serial.write(22);                      // (true,false,false): Display ON, Cursor & blink OFF
      }
    }
  } else {
    Serial.write(21);                          // (false,false,false): Display OFF
  }
}

void clearLCD(){                               //Clear LCD Screen and move cursor to
  Serial.write(12);                            //top-left position (row 1, column 1)
}

void backLightLCD(boolean on){                 //Usage example: backLightLCD(false);
  if (on)
    Serial.write(17);                          //Turn LCD BackLight ON
  else
    Serial.write(18);                          //Turn LCD BackLight OFF
}

void outputLCD(int row, int col, int arg){     //Usage example: outputLCD(1,9,200);
  int pos = (107 + (20 * row) + col);          //Calculate what number is needed to pass to Serial.write() in order to
  Serial.write(pos);                           //move to the row and column needed
  Serial.print(arg);
}

void outputLCD(int row, int col, String arg){  //Usage example: outputLCD(2,11,"Hello!!");
  int pos = (107 + (20 * row) + col);          //Calculate what number is needed to pass to Serial.write() in order to
  Serial.write(pos);                           //move to the row and column needed
  Serial.print(arg);
}
