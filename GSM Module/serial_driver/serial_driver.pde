/*
 * GM862-GPS testing sketch
 * used with Arduino Mega
 * http://tinkerlog.com
 */

#include "serialDriver.h"

//int onPin = 52;                      // pin to toggle the modem's on/off
//char PIN[5] = "XXXX";                // replace this with your PIN
//Position position;                   // stores the GPS position
serialDriver modem(&Serial3/*, onPin, PIN*/);   // modem is connected to Serial3
char *cmd;                            // command read from terminal
char buf;


void setup() {           
  delay(1000);                      
  Serial.begin(19200); 
  Serial.println("Serial Comm");
  Serial.println("ready");
  Serial.println("---------------------");
}

void loop() {
  if (Serial.available()) {
    cmd = Serial.read();
    Serial.print(cmd);
//    modem.send(cmd);
  }
}
