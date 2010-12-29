/*
 * GM862-GPS testing sketch
 * used with Arduino Mega
 * http://tinkerlog.com
 */

#include "GM862.h"

#define BUF_LENGTH 100
#define AMOUNT_OF_PHONE_NUMBERS 10
#define NUMBER_OF_COMMANDS 5

byte onPin = 52;                      // pin to toggle the modem's on/off
int state = -1;
char cmd;                            // command read from terminal
char buf[BUF_LENGTH];                // buffer used to capture messages from the modem
String commands[NUMBER_OF_COMMANDS+1] = {"initialize",
                            "arm",
                            "disarm",
                            "trigger alarm",
                            "lock",
                            "unlock",
                            "clear alarm"};
String responses[NUMBER_OF_COMMANDS+1] = {"Initialized!!",
                            "System Armed",
                            "System Disarmed",
                            "Alarm Triggered",
                            "Doors Locked",
                            "Doors Unlocked"};
String phoneNumbers[AMOUNT_OF_PHONE_NUMBERS] = {"##########","##########"};


// Create Modem Object
GM862 modem(&Serial3, onPin, phoneNumbers, commands, responses, NUMBER_OF_COMMANDS);   // modem is connected to Serial3


void setup() {
  Serial.begin(19200);
  modem.init();                       // initialize the GM862
}

void loop() {
  if (modem.checkForMessage(buf)){
    state = modem.parseMessage(buf);
     Serial.print("state is '");    // debug
     Serial.print(state);           //   |
     Serial.println("'");           //   |
  }
}
