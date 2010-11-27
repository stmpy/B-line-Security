/*
 * GM862-GPS testing sketch
 * used with Arduino Mega
 * http://tinkerlog.com
 */

#include "GM862.h"

int onPin = 22;                      // pin to toggle the modem's on/off
char PIN[5] = "XXXX";                // replace this with your PIN
Position position;                   // stores the GPS position
GM862 modem(&Serial3, onPin, PIN);   // modem is connected to Serial3
char cmd;                            // command read from terminal


void setup() {           
  delay(10000);                      
  Serial.begin(19200); 
  Serial.println("GM862 monitor");
  modem.switchOn();                   // switch the modem on
  delay(4000);                        // wait for the modem to boot
  modem.init();                       // initialize the GM862
  modem.version();                    // request modem version info
  while (!modem.isRegistered()) {
    delay(1000);
    modem.checkNetwork();             // check the network availability
  }
  Serial.println("---------------------");
  Serial.println("ready");
}


void requestHTTP() {
  char buf[100];
  byte i = 0;
  modem.initGPRS();                   // setup of GPRS context
  modem.enableGPRS();                 // switch GPRS on
  modem.openHTTP("search.twitter.com");    // open a socket
  Serial.println("sending request ...");
  modem.send("GET /search.atom?q=gm862 HTTP/1.1\r\n"); // search twitter for gm862
  modem.send("HOST: search.twitter.com port\r\n");     // write on the socket
  modem.send("\r\n");
  Serial.println("receiving ...");
  while (i++ < 10) {                  // try to read for 10s
    modem.receive(buf);               // read from the socket, timeout 1s
    if (strlen(buf) > 0) {            // we received something
      Serial.print("buf:"); Serial.println(buf);
      i--;                            // reset the timeout
    }
  }
  Serial.println("done");
  modem.disableGPRS();                // switch GPRS off
}


void loop() { 
  if (Serial.available()) {
    cmd = Serial.read();
    switch (cmd) {
    case 'o':
      modem.switchOff();              // switch the modem off
      break;
    case 's':                         // send a SMS. Replace with your number
      modem.sendSMS("6245", "your@email.com hello from arduino");   
      break;
    case 'w':
      modem.warmStartGPS();           // issue a GPS warmstart 
      break;
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
    case 'h':
      requestHTTP();                  // do a sample HTTP request
      break;
    default:
      Serial.println("command not recognized");
    }
  }
}
