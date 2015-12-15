
const int CSENSORPOWER=3, TXPIN=1, CSENSORINPUT=0;

void setup(){
  Serial.begin(9600);
  pinMode(TXPIN,OUTPUT);
  pinMode(CSENSORPOWER,OUTPUT);
  Serial.write(12);
  Serial.write(189);
  Serial.write("Analog Input=");
}

void loop(){
  readConductivity();
  delay(1000);
}

int readConductivity(){
  digitalWrite(CSENSORPOWER,HIGH);
  delay(100);
  int analogS = analogRead(CSENSORINPUT);
  digitalWrite(CSENSORPOWER, LOW);
  return analogS;
}

void backLight(boolean on){
  if (on)
    Serial.write(17); //Turn BackLight ON
  else
    Serial.write(18); //Turn BackLight OFF
}
void output(int row, int col, int arg){
  int pos = (107 + (20 * row) + col);       //Calculate what number is needed to pass to Serial.write() in order to
  Serial.write(pos);                        //move to the row and column needed
  Serial.print(arg);
}
void output(int row, int col, String arg){
  int pos = (107 + (20 * row) + col);       //Calculate what number is needed to pass to Serial.write() in order to
  Serial.write(pos);                        //move to the row and column needed
  Serial.print(arg);
}       ///////DUMMMBB

