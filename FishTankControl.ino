  /*indent size*/
const byte                          //These constants should only be changed if the circuit is changed
           CSENSORPOWER=2,          //Digital Pin that provides power to Conductivity Sensor (CS)
           CSENSORINPUT=0,          //Analog Pin that is used to read in a value from the CS
           TXPIN=1,                 //Digital Pin that is used to transmit data via "Serial"
           FRESHRELAY=3,            //Digital Pin that is used to energize fresh water relay (FWR)
           SALTYRELAY=4,            //Digital Pin that is used to energize salty water relay (SWR)
           HEATER=5;                //Digital Pin that provides power to the heater
           
const byte                          //These constants are used to make code more readable and should NEVER be changed
           CLOSE=0,CLOSED=0,        //Used in solenoid()
           OPEN=1,                  //Used in solenoid()
           TOOSALTY=0,              //Assigned to cStatus if output of readConductivity() is too high
           TOOFRESH=1,              //Assigned to cStatus if output of readConductivity() is too low
           JUSTRIGHT=2;             //Assigned to cStatus if output of readConductivity() is within acceptable range

byte                                //These variables are used throughout the program to store data
     swsStatus,                     //Status of Salt-Water-Solenoid
     fwsStatus,                     //Status of Fresh-Water-Solenoid
     cStatus;                       //Status of Conductivity of water

void setup(){
  Serial.begin(9600);                          // Set baud rate of LCD to 9600 bps
  pinMode(TXPIN,OUTPUT);                       // Set Transmit pin to output mode
  pinMode(CSENSORPOWER,OUTPUT);                // Set pin that powers conductivity sensor to output mode
  pinMode(FRESHRELAY,OUTPUT);                  // Set pin used to energize FWR to output mode
  pinMode(SALTYRELAY,OUTPUT);                  // Set pin used to energize SWR to output mode
  pinMode(HEATER,OUTPUT);                      // Set pin used to power heater to output mode
  formatLCD(true,false,false);                 // Turn display on, cursor off, character blink off
  clearLCD();                                  // Clear the LCD's screen
  backLightLCD(true);                          // Turn the LCD backlight on
  update();                                    // Update values of variables to accurately reflect what they represent
  solenoid(CLOSE,FRESHSOLENOID);               // Close the Freshwater solenoid (in case it's open)
  solenoid(CLOSE,SALTYSOLENOID);               // Close the Saltwater solenoid (in case it's open)
}

void loop(){

}

int readConductivity(){                        // Usage example: int saltLevel = readConductivity();
  digitalWrite(CSENSORPOWER,HIGH);             // 
  delay(100);                                  // 
  int level = analogRead(CSENSORINPUT);        // 
  digitalWrite(CSENSORPOWER, LOW);             // Turn off power ASAP to prevent corrosion
  return level;
}

void formatLCD(boolean display, boolean cursor, boolean blink){
  if (display){                                // Usage example: formatLCD(true,false,true);
    if (cursor){                               // 
      if (blink){                              // 
        Serial.write(25); //<------------------//-(true,true,true): ALL ON
      } else {                                 // 
        Serial.write(24); //<------------------//-(true,true,false): Display & Cursor ON, blink OFF
      }                                        // 
    } else {                                   // 
      if (blink) {                             // 
        Serial.write(23); //<------------------//-(true,false,true): Display ON, Cursor OFF, blink ON
      } else {                                 // 
        Serial.write(22); //<------------------//-(true,false,false): Display ON, Cursor & blink OFF
      }                                        // 
    }                                          // 
  } else {                                     // 
    Serial.write(21); //<----------------------//-(false,false,false): Display OFF
  }                                            // 
}

void clearLCD(){                               // Clear LCD Screen and move cursor to
  Serial.write(12);                            // top-left position (row 1, column 1)
}

void backLightLCD(boolean on){                 // Usage example: backLightLCD(false);
  if (on)                                      // 
    Serial.write(17);                          // Turn LCD BackLight ON
  else                                         // 
    Serial.write(18);                          // Turn LCD BackLight OFF
}

void outputLCD(int row, int col, int arg){     // Usage example: outputLCD(1,9,1337);
  int pos = (107 + (20 * row) + col);          // Calculate what number is needed to pass to Serial.write() in order to
  Serial.write(pos);                           // move to the row and column needed
  Serial.print(arg);                           // Serial.print() must be used for variables
}

void outputLCD(int row, int col, String arg){  // Usage example: outputLCD(2,11,"Hello!!");
  int pos = (107 + (20 * row) + col);          // Calculate what number is needed to pass to Serial.write() in order to
  Serial.write(pos);                           // move to the row and column needed
  Serial.print(arg);                           // Serial.print() must be used for variables
}

void outputLCD(int row, int col, float arg, int prec){
                                               // Usage example: outputLCD(3,2,3.1415,4);
  int pos = (107 + (20 * row) + col);          // Calculate what number is needed to pass to Serial.write() in order to
  Serial.write(pos);                           // move to the row and column needed
  Serial.print(arg,prec);                      // Serial.print() must be used for variables
}

float toVolts(int reading){                    // Usage example: float volts = toVolts(readConductivity());
  return ((( float(reading)) / 1023.0) * 5.0); // Derived from ratio: (reading/1023) = (volts/5V)
}

void solenoid(byte action, byte relay){        // Usage example: solenoid(OPEN,SALTYRELAY);
  if (action==CLOSE){
    /*INSERT CODE TO CLOSE SOLENOID ON PIN relay*/
  } else {
    /*INSERT CODE TO OPEN SOLENOID ON PIN relay*/
  }
}

void update(){                                 // Usage example: update();
  /*INSERT CODE TO CHECK STATUS
   *OF THINGS AND REASSIGN
   *VARIABLES TO BE ACCURATE
   */
}
