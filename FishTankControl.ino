  /*indent size*/
const byte                          //These constants should only be changed if the circuit is changed
          CSENSORPOWER=2,           //Digital Pin that provides power to Conductivity Sensor (CS)
          CSENSORINPUT=0,           //Analog Pin that is used to read in a value from the CS
          THINPUT=1,                //Analog Pin that is used to read in a value from the TH
          TXPIN=1,                  //Digital Pin that is used to transmit data via "Serial"
          FRESHRELAY=4,             //Digital Pin that is used to energize fresh water relay (FWR)
          SALTYRELAY=3,             //Digital Pin that is used to energize salty water relay (SWR)
          HEATERRELAY=5;            //Digital Pin that provides power to the heater
           
const byte                          //These constants are used to make code more readable and should NEVER be changed
          CLOSE=LOW,CLOSED=LOW,     //Used in solenoid() ex: solenoid(CLOSE,FRESHRELAY);
          OPEN=HIGH,                //Used in solenoid() ex: solenoid(OPEN,SALTYRELAY);
          ON=HIGH, OFF=LOW,         //Used by htrStatus  ex: if (htrStatus == ON){}
          SALTY=SALTYRELAY,         //Used by addWater() ex: addWater(SALTY,2000);
          FRESH=FRESHRELAY;         //Used by addWater() ex: addWater(FRESH,2000);

const int
          SPECIFIC_HEAT=4180;       //Represents the specific heat of water
          
const double
          HEATER_RESISTANCE=20.5;   //Represents the resistance of the heater
          
double                              //These constants represent desired salt levels
          MASS=87.8,                //Mass of water in tank (g)
          FLOWRATE=6.67,            //Flow Rate of valves (g/s)
          SETPOINT=0.001,           //Desired salinity level (Decimal)
          FRESHGAIN=1.80,           //Gain used when adding fresh water
          *SALTYGAIN=0.05,           //Gain used when adding salty water
          OVF=0.15,                 //Overflow fraction that is striaght from input
          STDEV = 4.159590487,      //Standard deviation of salinity data (Should be analogRead() value, not wt%)
          UCL,                      //Upper acceptable limit of desired salinity level (%)
          LCL,                      //Lower acceptable limit of desired salinity level (%)
          TLCL,                     //Lower acceptable limit of desired temperature (Celsius)
          TUCL;                     //Upper acceptable limit of desired temperature (Celsius)

                                    /***********************************************************************/
byte                                /*These variables are used throughout the program to store data       **/
          swsStatus=CLOSED,         /*Status of Salt-Water-Solenoid                                       **/
          fwsStatus=CLOSED,         /*Status of Fresh-Water-Solenoid                                      **/
          htrStatus=OFF;            /*Status of Heater                                                    **/
int                                 /*                                                                    **/
          csOutput=0,               /*Output of Conductivity Sensor                                       **/
          thOutput=0,               /*Output of Thermister                                                **/
          displaySet=1;             /*Symbolizes which set of data to print to LCD screen                 **/
double                              /*                                                                    **/
          sStatus,                  /*Status of Salinity of water (wt%)                                   **/
          tStatus;                  /*Status of Temperature of water (Degrees)                            **/
                                    /***********************************************************************/

const unsigned long                 //These constants used to define times and intervals
          DST=5000,                 //Represents the time between each display set switch (ms)
          LCD=500,                  //Represents the time between each update of LCD Screen (ms)
          TEMPCHECK=200,            //Represents the time between each read of the thermistor (ms)
          FRESHDEADTIME=12000,      //Represents the deadtime compensation for fresh water (ms)
          SALTYDEADTIME=8000;       //Represents the deadtime compensation for salty water (ms)

bool                                //These variables are used to schedule tasks to be run side-by-side
          readCS=false,             //Used when reading conductivity sensor  -> conductivitySchedule
          closeSWS=false,           //Used after opening saltwater solenoid  -> swsSchedule
          closeFWS=false,           //Used after opening freshwater solenoid -> fwsSchedule
          turnOffHeater=false,      //Used after turning on heater           -> heatSchedule
          tooSalty=false,           //Used when checking salinity            -> adjustSchedule
          tooFresh=false;           //Used when checking salinity            -> adjustSchedule
          
unsigned long                       //These variables are used to schedule tasks to be run side-by-side
          PRESENT=0,                //This variable represents the current time on the system clock
          conductivitySchedule=100, //Represents the time scheduled to read the CS
          thermistorSchedule=200,   //Represents the time scheduled to read the TH
          swsSchedule=0,            //Represents the time scheduled to close saltwater solenoid
          fwsSchedule=0,            //Represents the time scheduled to close freshwater solenoid
          heatSchedule=0,           //Represents the time scheduled to turn off heater
          displaySwitchSchedule=DST,//Represents the time scheduled to switch display set
          lcdUpdateSchedule=0,      //Represents the time scheduled to update LCD Screen
          checkSalinity=10000,      //Represents the time scheduled to check salinity (>DEADTIME<)
          adjustTemp=0,             //Represents the time scheduled to adjust temperature
          adjustSchedule=0;         //Represents the time scheduled to adjust salinity

void setup(){
  Serial.begin(9600);                          // Set baud rate of LCD to 9600 bps
  pinMode(TXPIN,OUTPUT);                       // Set Transmit pin to output mode
  pinMode(CSENSORPOWER,OUTPUT);                // Set pin that powers conductivity sensor to output mode
  pinMode(FRESHRELAY,OUTPUT);                  // Set pin used to energize FWR to output mode
  pinMode(SALTYRELAY,OUTPUT);                  // Set pin used to energize SWR to output mode
  pinMode(HEATERRELAY,OUTPUT);                 // Set pin used to power heater to output mode
  formatLCD(true,false,false);                 // Turn display on, cursor off, character blink off
  clearLCD();                                  // Clear the LCD's screen
  backLightLCD(true);                          // Turn the LCD backlight on
  
  LCL=toPercent(toReading(SETPOINT)-(3*STDEV));    // Calculate Lower Control Limit
  UCL=toPercent(toReading(SETPOINT)+(3*STDEV));    // Calculate Upper Control Limit
  
}

void loop(){
  
  PRESENT = millis();                                           // Update current time
  events();                                                     // Do scheduled events
  readConductivity();                                           // Read conductivity sensor
  
}

void events(){                                                  // Usage example: events();

  if (readCS && PRESENT>conductivitySchedule){                  // If readConductivity() is scheduled for now:
    csOutput = analogRead(CSENSORINPUT);                        //   Read the conductivity sensor
    digitalWrite(CSENSORPOWER,LOW);                             //   Turn off power to conductivity sensor
    sStatus = toPercent(csOutput);                              //   Convert output to wtpercent
    readCS=false;                                               //   Un-Schedule this event
  }
  if (thermistorSchedule < PRESENT){                            // If readThermistor() is scheduled for now:
    thOutput = analogRead(THINPUT);                             //   Read the thermistor
    tStatus = toTemp(thOutput);                                 //   Convert output to degrees Celsius
    thermistorSchedule = PRESENT + TEMPCHECK;                   //   Re-Schedule this event
  }
  if (closeFWS && PRESENT>fwsSchedule){                         // If closeFreshSolenoid() is scheduled for now:
    solenoid(CLOSE,FRESH);                                      //   Close the FWS
    closeFWS=false;                                             //   Un-Schedule this event
  }
  if (closeSWS && PRESENT>swsSchedule){                         // If closeSaltySolenoid() is scheduled for now:
    solenoid(CLOSE,SALTY);                                      //   Close the SWS
    closeSWS=false;                                             //   Un-Schedule this event
  }
  if (PRESENT>lcdUpdateSchedule){                               // If updateLCD() is scheduled for now:
    updateLCD();                                                //   Update the LCD Screen
    lcdUpdateSchedule += LCD;                                   //   Re-Schedule this event
  }
  if (PRESENT>checkSalinity && !tooSalty && !tooFresh){         // If checking salinity is scheduled for now:
    if (sStatus > UCL){                                         //   If wtpercent NaCl is too high:
      tooSalty = true;                                          //     Update status
      adjustSchedule = PRESENT + SALTYDEADTIME;                 //     Wait for readings to even out before adding water
      checkSalinity += 20000;                                   //     Check every 20 seconds
    } else if (sStatus < LCL){                                  //   If wtpercent NaCl is too low:
      tooFresh = true;                                          //     Update status
      adjustSchedule = PRESENT + FRESHDEADTIME;                 //     Wait for readings to even out before adding water
      checkSalinity += 20000;                                   //     Check every 20 seconds
    }                                                           // 
  }
  if (PRESENT>adjustSchedule && (tooSalty || tooFresh)){        // If adjusting salinity is scheduled for now:
    if (tooSalty){                                              //   If water is too salty:
      addWater(FRESH,getFreshOpenTime());                       //     Fix it
      tooSalty = false;                                         //     Update status
    } else if (tooFresh){                                       //   If water is too fresh:
      addWater(SALTY,getSaltyOpenTime());                       //     Fix it
      tooFresh = false;                                         //     Update status
    }                                                           // 
  }
  if (adjustTemp < PRESENT){
    if (tStatus < TLCL){
      heatUp(getHeaterUpTime());
    }
    adjustTemp = PRESENT + 100;
  }
  if (turnOffHeater && heatSchedule < PRESENT){
    digitalWrite(HEATERRELAY,LOW);
  }
}                                                               // End of events()

void readConductivity(){                       // Usage example: int saltLevel = readConductivity();
  if (!readCS){                                // If this event is not already scheduled:
    digitalWrite(CSENSORPOWER,HIGH);           //   Turn on power to conductivity sensor
    readCS = true;                             //   Schedule event to read sensor
    conductivitySchedule = millis()+100;       //   Schedule event for 100 milliseconds from now
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
//                                             // Usage example: outputLCD(3,2,3.1415,4);
  int pos = (107 + (20 * row) + col);          // Calculate what number is needed to pass to Serial.write() in order to
  Serial.write(pos);                           // move to the row and column needed
  Serial.print(arg,prec);                      // Serial.print() must be used for variables
}

void updateLCD(){
  
  Serial.flush();                             // Wait for LCD to finish printing before beginning
  if (displaySet==1){
    outputLCD(1,2,"LCL");                     // Print LCL label
    outputLCD(2,1,LCL*100,3);                 // Print LCL
    outputLCD(2,6,"%");
    
    outputLCD(1,9,"SP");                      // Print Setpoint reading label
    outputLCD(2,8,SETPOINT*100,3);            // Print Setpoint
    outputLCD(2,13,"%");
    
    outputLCD(1,16,"UCL");                    // Print UCL label
    outputLCD(2,15,UCL*100,3);                // Print UCL
    outputLCD(2,20,"%");
    
    outputLCD(4,1,"salty");                   // Print SWS status label
    if (swsStatus==CLOSED){                   // 
      outputLCD(3,1,"CLOSED");                // Print SWS status (SWS is CLOSED)
    } else {                                  // 
      outputLCD(3,1," OPEN ");                // Print SWS status (SWS is OPEN)
    }
    
    outputLCD(4,7,"current");                 // Print current percentage label
    outputLCD(3,8,sStatus*100,3);             // Print percentage
    outputLCD(3,13,"%");
    
    outputLCD(4,16,"DI");                     // Print FWS status label
    if (fwsStatus==CLOSED){                   // 
      outputLCD(3,14,"CLOSED");               // Print FWS status (FWS is CLOSED)
    } else {                                  // 
      outputLCD(3,14," OPEN ");               // Print FWS status (FWS is OPEN)
    }

  } else {
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
}

double toVolts(int reading){                   // Usage example: double volts = toVolts(readConductivity());
  return (((double(reading)) / 1023.0) * 5.0); // Derived from ratio: (reading/1023) = (volts/5V)
}

void solenoid(byte action, byte relay){        // Usage example: solenoid(OPEN,SALTYRELAY)
  digitalWrite(relay,action);                  // Set the appropriate pin to the appropriate status
  if (relay==FRESHRELAY){                      // If operating on the freshwater solenoid:
    fwsStatus=action;                          //   Update the FWS status variable
  } else {                                     // If operating on the saltwater solenoid:
    swsStatus=action;                          //   Update the SWS status variable
  }                                            // 
}                                              // 

void addWater(byte type, long ms){             // Usage example: addWater(SALTY,2000)
  if (sStatus > UCL || sStatus < LCL){         // Only pass if the salinity is not correct
    if (type==FRESH && !closeFWS){             //   Only pass this if closeFWS is not already scheduled:
      solenoid(OPEN,FRESHRELAY);               //     Open freshwater solenoid
      closeFWS=true;                           //     Schedule a task to close freshwater solenoid
      fwsSchedule = (millis()+ms);             //     Schedule ^ for (ms) milliseconds later
    }                                          // 
    if (type==SALTY && !closeSWS){             //   Only pass this if closeSWS is not already scheduled:
      solenoid(OPEN,SALTYRELAY);               //     Open saltwater solenoid
      closeSWS=true;                           //     Schedule a task to close saltwater solenoid
      swsSchedule = (millis()+ms);             //      Schedule ^ for (ms) milliseconds later
    }                                          // 
  } else {                                     // If false alarm
    tooFresh=false;                            //   Reset status
    tooSalty=false;                            //   Reset status
  }                                            // 
}                                              // 

void heatUp(unsigned long time){
  digitalWrite(HEATERRELAY, HIGH);
  turnOffHeater = true;
  heatSchedule = PRESENT + time;
}

double toPercent(int reading){                 // Usage example: double wtpercent = toPercent(csOutput);
  return pow(2.71828182846,((double(reading)-1598.93492766)/146.5571565956));    // Derived from conductivity 
//                                                                               // calibration spreadsheet
}
int toReading(double percent){                 // Usage example: int reading = toReading(wtpercent);
  return int(146.5571565956 * log(percent) + 1598.93492766);                     // Taken from conductivity
//                                                                               // calibration spreadsheet
}

double toTemp(int reading){
  return pow(2.71828182846,((double(reading)+272.7245412132)/243.2281096415));   // Derived from thermistor reading
//                                                                               // calibration spreadsheet
}

long getFreshOpenTime(){                       // Usage example: addWater(FRESH,getFreshOpenTime());
  double SALINITY = toPercent(csOutput);
  return abs(long(double(1000.0) * (MASS * FRESHGAIN * (SALINITY - SETPOINT)) / (FLOWRATE * SALINITY * (1.0 - OVF))));
}
long getSaltyOpenTime(){                       // Usage example: addWater(SALTY,getSaltyOpenTime());
  double SALINITY = toPercent(csOutput);
  return abs(long(double(1000.0) * (MASS * SALTYGAIN * (SALINITY - SETPOINT)) / (FLOWRATE * SALINITY * (1.0 - OVF))));
}
long getHeaterUpTime(){                        // Usage example: heatUp(getHeaterUpTime());
  if (tStatus > TLCL){
    return 0L;
  }
  return abs((MASS * SPECIFIC_HEAT * (TLCL - tStatus)) / (144 / HEATER_RESISTANCE));
}
