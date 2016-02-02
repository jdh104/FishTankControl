  /*indent size*/
const byte                          //These constants should only be changed if the circuit is changed
          THERMINPUT=1,             //Analog Pin that is used to read in a value from the thermistor (TH)
          TXPIN=1,                  //Digital Pin that is used to transmit data via "Serial"
          HEATER=5;                 //Digital Pin that provides power to the heater
           
const byte                          //These constants are used to make code more readable and should NEVER be changed
          OPEN=HIGH,                //Used in solenoid() ex: solenoid(OPEN,SALTYRELAY);
          ON=HIGH, OFF=LOW;         //Used by htrStatus  ex: if (htrStatus == ON){}

double                              //These constants represent desired salt levels
          MASS=0,                   //Mass of water in tank (g)
          FLOWRATE=0,               //Flow Rate of valves (g/s)
          SETPOINT=0,               //Desired salinity level (%)
          FRESHGAIN=0.8,            //Gain used when adding fresh water
          SALTYGAIN=0.8,            //Gain used when adding salty water
          OVF=0.15,                 //Overflow fraction that is striaght from input
          STDEV=0,                  //Standard deviation of salinity data (Should be analogRead() value, not wt%)
          UCL,                      //Upper acceptable limit of desired salinity level (%)
          LCL;                      //Lower acceptable limit of desired salinity level (%)

                                    /***********************************************************************/
byte                                /*These variables are used throughout the program to store data       **/
          htrStatus=OFF;            /*Status of Heater                                                    **/
int                                 /*                                                                    **/
          thOutput=0;               /*Output of Thermister                                                **/
double                              /*                                                                    **/
          tStatus;                  /*Status of Temperature of water (Degrees)                            **/
                                    /***********************************************************************/

const unsigned long                 //These constants used to define times and intervals
          LCD=500;                  //Represents the time between each update of LCD Screen (ms)
          
unsigned long                       //These variables are used to schedule tasks to be run side-by-side
          lcdUpdateSchedule=0;      //Represents the time scheduled to update LCD Screen

void setup(){
  Serial.begin(9600);                          // Set baud rate of LCD to 9600 bps
  pinMode(TXPIN,OUTPUT);                       // Set Transmit pin to output mode
  pinMode(HEATER,OUTPUT);                      // Set pin used to power heater to output mode
  formatLCD(true,false,false);                 // Turn display on, cursor off, character blink off
  clearLCD();                                  // Clear the LCD's screen
  backLightLCD(true);                          // Turn the LCD backlight on
}

void loop(){
  
  PRESENT = millis();                                           // Update current time
  events();                                                     // Do scheduled events
  
}

void events(){                                                  // Usage example: events();
  if (PRESENT>lcdUpdateSchedule){                               // If updateLCD() is scheduled for now
    updateLCD();                                                // Update the LCD Screen
    lcdUpdateSchedule += LCD;                                   // Re-Schedule this event
  }
}

void formatLCD(bool display, bool cursor, bool blink){
  if (display){                                // Usage example: formatLCD(true,false,true);
    if (cursor){                               // 
      if (blink){                              // 
        Serial.write(25); //<------------------//-(true,true,true): ALL ON
      } else {                                 // 
        Serial.write(24); //<------------------//-(true,true,false): Display & Cursor ON, blink OFF
      }                                        // 
    } else {                                   // 
      if (blink){                              // 
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

void backLightLCD(bool on){                    // Usage example: backLightLCD(false);
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

void outputLCD(int row, int col, double arg, int prec){
                                               // Usage example: outputLCD(3,2,3.1415,4);
  int pos = (107 + (20 * row) + col);          // Calculate what number is needed to pass to Serial.write() in order to
  Serial.write(pos);                           // move to the row and column needed
  Serial.print(arg,prec);                      // Serial.print() must be used for variables
}

void updateLCD(){
  
  Serial.flush();                           // Wait for LCD to finish printing before beginning
  outputLCD(1,3,"thReading=");              // Print TH reading label
  outputLCD(1,13,"    ");                   // Clear old TH reading
  outputLCD(1,13,thOutput);                 // Print TH reading
  
  outputLCD(2,5,"Temp=");                   // Print Temperature label
  outputLCD(2,10,tStatus,4);                // Print Temperature
  
  outputLCD(4,4,"Heater is");               // Print Heater status label
  if (htrStatus==OFF){                      // 
    outputLCD(4,14,"OFF");                  // Print Heater status (Heater is OFF)
  } else {                                  // 
    outputLCD(4,14,"ON ");                  // Print Heater status (Heater is ON)
  }
}

double toVolts(int reading){                    // Usage example: double volts = toVolts(readConductivity());
  return ((( double(reading)) / 1023.0) * 5.0); // Derived from ratio: (reading/1023) = (volts/5V)
}
