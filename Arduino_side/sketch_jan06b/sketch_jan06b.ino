#include <Wire.h>

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
char inStringArr[10];

bool writeReadBack(char addr, char data){
  Wire.beginTransmission(0x27);
  Wire.write(addr);
  Wire.write(data);
  Wire.endTransmission();

  Wire.beginTransmission(0x27);
  Wire.write(addr);
  Wire.endTransmission();
  Wire.requestFrom(0x27, 1);
  char readback = Wire.read();

  return readback == data;
}

char readBack(char addr){
  Wire.beginTransmission(0x27);
  Wire.write(addr);
  Wire.endTransmission();
  Wire.requestFrom(0x27, 1);
  char readback = Wire.read();
  return readback;
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void setup(){
  Wire.begin(); // join i2c bus
  Serial.begin(9600); 
  inputString.reserve(10);
}

void loop(){
  if (stringComplete) {
    unsigned char command = inputString.charAt(0);
    String addr_s = inputString.substring(1, 4);
    unsigned char addr = (unsigned char)addr_s.toInt();
    String data_s = inputString.substring(4, 7);
    unsigned char data = (unsigned char)data_s.toInt();
    switch(command){
      case 'r':
        Serial.println(readBack(addr),DEC);
      break;
      case 'w':
        Serial.println(writeReadBack(addr, data),DEC);
      break;
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}
