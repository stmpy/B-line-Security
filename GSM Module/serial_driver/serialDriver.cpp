/*
 * serialDriver.cpp
 * http://tinkerlog.com
 */

#include "serialDriver.h"
#include <string.h>
#include <avr/pgmspace.h>

#define BUF_LENGTH 100

serialDriver::serialDriver(HardwareSerial *modemPort) {
  state = STATE_NONE;
  modem = modemPort;
  modem->begin(19200);
}

byte serialDriver::requestModem(const char *command, uint16_t timeout, boolean check, char *buf) {
			
  byte count = 0;
  char *found = 0;
  
  *buf = 0;
  Serial.println(command);
  modem->print(command);
  modem->print('\r');
  count = getsTimeout(buf, timeout);
  if (count) {
    if (check) {
      found = strstr(buf, "\r\nOK\r\n");
      if (found) {
	Serial.println("->ok");
      }
      else {
	Serial.print("->not ok: ");
	Serial.println(buf);  
      } 
    }
    else {
      Serial.print("->buf: ");
      Serial.println(buf);  
    }
  }
  else {
    Serial.println("->no respone");
  }
  return count;
}

byte serialDriver::getsTimeout(char *buf, uint16_t timeout) {
  byte count = 0;
  long timeIsOut = 0;
  char c;
  *buf = 0;
  timeIsOut = millis() + timeout;
  while (timeIsOut > millis() && count < (BUF_LENGTH - 1)) {  
    if (modem->available()) {
      count++;
      c = modem->read();
      *buf++ = c;
      timeIsOut = millis() + timeout;
    }
  }
  if (count != 0) {
    *buf = 0;
    count++;
  }
  return count;
}

void serialDriver::send(char *buf) {
  modem->print(buf);
}


char *serialDriver::receive(char *buf) {
  getsTimeout(buf, 1000);
  return buf;
}