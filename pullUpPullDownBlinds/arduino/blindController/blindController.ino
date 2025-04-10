/*
  with i2c, use 8/9 to control first motor
*/

#include <Wire.h>

#define SLAVE_ADDRESS 0x08


#define NUM_WIRES 1
byte wires[1][2] = {
  {2,3},
};


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(19200);
  // Wait for serial port to connect (optional, mostly for boards like Leonardo)
  while (!Serial) {
    ; // Do nothing until serial connection is established
  }
  // Print a message when setup is complete
  Serial.println("Serial communication started! i2c on " + String(SLAVE_ADDRESS));
  // initialize digital pin LED_BUILTIN as an output.  
  Wire.begin(SLAVE_ADDRESS);

  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(2, INPUT);
  //pinMode(8, OUTPUT);
  //pinMode(9, OUTPUT);
  for (int i = 0; i < NUM_WIRES; i++) {
     for (int j = 0; j < 2; j++) {
      pinMode(wires[i][j], OUTPUT);
      digitalWrite(wires[i][j], LOW);
     }
  }

  pinMode(12, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
}


long ledAccu = 0;
byte ledState = LOW;

char cmd= ' '; //shoudl be ' '
byte who = 0;
unsigned long turnTime = 0; //should be 0

unsigned long lastChangeTime = 0;
int loopCount = 0;
// the loop function runs over and over again forever
int D12 = 0, D11=0;
int RUNNING = 0;
void loop() {

unsigned long timeFromLastChange = millis() - lastChangeTime;
  
  loopCount++;  
  ledAccu++;
  if (ledAccu>600000) {
    ledAccu = 0;
    digitalWrite(LED_BUILTIN, ledState);
    if (ledState == LOW) ledState = HIGH; else ledState = LOW;      
  }

  //digitalWrite(8, HIGH);
  //digitalWrite(9, HIGH);
  if (cmd == 'c' || cmd == 'C') {
    if (turnTime < millis()) {
      cmd = ' ';
      digitalWrite(wires[who][0], LOW);
      digitalWrite(wires[who][1], LOW);
      Serial.println("Stopping");
    }
  }


   D12 = digitalRead(12);
   D11 = digitalRead(11); 
   if (D12 == LOW) {
      Serial.println("LOW");
      digitalWrite(wires[who][0], HIGH);
      digitalWrite(wires[who][1], HIGH);
      RUNNING = 1;
   } if (D11 == LOW){
    digitalWrite(wires[who][0], HIGH);
      digitalWrite(wires[who][1], LOW);
      RUNNING = 1;
   } else {
    if (RUNNING) {
    Serial.println("stoop");
    }
      digitalWrite(wires[who][0], LOW);
      digitalWrite(wires[who][1], LOW);
      RUNNING = 0;
   }
}



//format: 5 bytes, 
// 1 cmd(c|C|s), clock wise, counter clock wise, or stop
// 2 who
// 3 bytes of mill seconds      
void receiveData(int bytecount)
{    
  byte w1= 0, w2=0;
  cmd = Wire.read();  
  who = Wire.read();
  if (cmd == 'c') {
    w1 = HIGH;
    w2 = LOW;
  } else if (cmd == 'C') {  
    w1 = LOW;
    w2 = HIGH;
  } else if (cmd == 's') {
    w1 = LOW;
    w2 = LOW;
  }
  digitalWrite(wires[who][0], w1);  //ENABLE
  digitalWrite(wires[who][1], w2);  //DIR
  turnTime = 0;
  while (Wire.available()) {
     // Shift existing result left 8 bits and add new byte
     turnTime = (turnTime << 8) | Wire.read();        
  }
  Serial.print("Doing " + String(cmd)+" who="+String(who)+" turnTime="+String(turnTime)+"\n");
  turnTime = millis()+turnTime;
}

void sendData()
{
  Serial.println("sending data");
  Wire.write("123456",3);
}
