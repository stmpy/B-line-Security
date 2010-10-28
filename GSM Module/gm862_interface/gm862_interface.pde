/*
 * GM862-GPS testing sketch
 * used with Arduino Mega
 * http://tinkerlog.com
 */

#include "GM862.h"

#define BUF_LENGTH 100

int onPin = 52;                      // pin to toggle the modem's on/off
int commandPin = 53;
Position position;                   // stores the GPS position
GM862 modem(&Serial3, onPin, commandPin);   // modem is connected to Serial3
char cmd;                            // command read from terminal
byte count;
char buf[BUF_LENGTH];
char outBuf[BUF_LENGTH];


void setup() {
  delay(1000);
  modem.getStatus();                    // identify if modem is turned on or not                      
  Serial.begin(19200); 
  Serial.println("GM862 MC Control");
  // test to see if modem is on
  if(!modem.isOn()){
    Serial.println("Turning on Modem ...");
    modem.switchOn();                   // switch the modem on
    delay(4000);                        // wait for the modem to boot
  }
  modem.init();                       // initialize the GM862
  modem.checkNetwork();             // check the network availability
//  modem.version();                    // request modem version info
/*  while (!modem.isRegistered()) {
    delay(1000);
    modem.checkNetwork();             // check the network availability
  }*/
  Serial.println("---------------------");
  Serial.println("Send Commands");
}

void loop() {
  if (Serial.available()) {
    cmd = Serial.read();
    switch (cmd) {
    case 'o':
      modem.switchOff();              // switch the modem off
      break;
    case 's':                         // send a SMS. Replace with your number
      modem.sendSMS("8012090098", "hello from my arduino!");   
      break;
    case 'w':
      modem.warmStartGPS();           // issue a GPS warmstart 
      break;
    case 'c':
      // read Serial and get the command
      break;
    case 'd':
      modem.clearMessages();          // delete text messages
      break;
    case 'n':
        while (!modem.isRegistered()) {
            delay(1000);
            modem.checkNetwork();             // check the network availability
        }  
    case 'p':
      position = modem.requestGPS();  // request a GPS position
      if (position.fix == 0) {        // GPS position is not fixed
	Serial.println("no fix");
      }
      else {                          // print lat, lon, alt
	Serial.print("GPS position: ");
	Serial.print(position.lat_deg);  Serial.print(".");
	Serial.print(position.lat_min);  Serial.print(", ");
	Serial.print(position.lon_deg);  Serial.print(".");
	Serial.print(position.lon_min);  Serial.print(", ");
	Serial.println(position.alt);
      }
      break;
/*    case 'h':
      modem.requestHTTP();                  // do a sample HTTP request
      break;
*/    default:
      Serial.println("command not recognized");
    }
  }
  count = modem.checkForMessage(buf);
  if (count){ // not working yet
    modem.parseMessage(buf);
  }
}
