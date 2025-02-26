/*
  with i2c, use 8/9 to control first motor
*/

#include <Wire.h>

#define SLAVE_ADDRESS 0x08


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(119200);
  // Wait for serial port to connect (optional, mostly for boards like Leonardo)
  while (!Serial) {
    ; // Do nothing until serial connection is established
  }
  // Print a message when setup is complete
  Serial.println("Serial communication started!");
  // initialize digital pin LED_BUILTIN as an output.  
  Wire.begin(SLAVE_ADDRESS);

  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
}

byte wires[1][2] = {
  {8,9},
};

int ledAccu = 0;
byte ledState = LOW;

char cmd= ' ';
byte who = 0;
unsigned long turnTime = 0;

// the loop function runs over and over again forever
void loop() {
  ledAccu++;
  if (ledAccu>1000) {
    ledAccu = 0;
    digitalWrite(LED_BUILTIN, ledState);
    if (ledState == LOW) ledState = HIGH; else ledState = LOW;
  }

  if (cmd == 'c' || cmd == 'C') {
    if (turnTime < millis()) {
      cmd = ' ';
      digitalWrite(wires[who][0], LOW);
      digitalWrite(wires[who][1], LOW);
      Serial.print("Stopping");
    }
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
  digitalWrite(wires[who][0], w1);
  digitalWrite(wires[who][1], w2);
  turnTime = 0;
  while (Wire.available()) {
     // Shift existing result left 8 bits and add new byte
     turnTime = (turnTime << 8) | Wire.read();        
  }
  Serial.print("Doing " + String(cmd)+" who="+String(who)+" turnTime="+String(turnTime));
  turnTime = millis()+turnTime;
}

void sendData()
{
  //Wire.write(data_to_echo);
}
