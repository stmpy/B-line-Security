/*
 * GM862.cpp
 * http://tinkerlog.com
 */

#include "GM862.h"
#include <string.h>
#include <avr/pgmspace.h>

#define BUF_LENGTH 100

GM862::GM862(HardwareSerial *modemPort, byte onOffPin, char *pin) {
  state = STATE_NONE;
  modem = modemPort;
  modem->begin(19200);
  this->onOffPin = onOffPin;
  pinMode(onOffPin, OUTPUT);
  strcpy(this->pin, pin);
}


boolean GM862::isOn() {
  return (state & STATE_ON);
}


boolean GM862::isInitialized() {
  return (state & STATE_INITIALIZED);
}


boolean GM862::isRegistered() {
  return (state & STATE_REGISTERED);
}


boolean GM862::isPosFixed() {
  return (state & STATE_POSFIX);
}


void GM862::switchOn() {
  Serial.println("switching on");
  if (!isOn()) {
    switchModem();
    state |= STATE_ON;
  }
  Serial.println("done");
}


void GM862::switchOff() {
  Serial.println("switching off");
  if (isOn()) {
    switchModem();
    state &= ~STATE_ON;
  }
  Serial.println("done");
}


void GM862::switchModem() {
  digitalWrite(onOffPin, HIGH);
  delay(2000);
  digitalWrite(onOffPin, LOW);
  delay(1000);
}


byte GM862::requestModem(const char *command, uint16_t timeout, boolean check, char *buf) {
			
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


byte GM862::getsTimeout(char *buf, uint16_t timeout) {
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


void GM862::init() {
  Serial.println("initializing modem ...");
  char buf[BUF_LENGTH];
  char cmdbuf[30] = "AT+CPIN=";
  strcat(cmdbuf, pin);
  requestModem("AT", 1000, true, buf);
  requestModem("AT+IPR=19200", 1000, true, buf);
  requestModem("AT+CMEE=2", 1000, true, buf);
  requestModem(cmdbuf, 1000, true, buf);
  state |= STATE_INITIALIZED;
  Serial.println("done");
}


void GM862::version() {
  char buf[BUF_LENGTH];
  Serial.println("version info ...");
  requestModem("AT+GMI", 1000, false, buf);
  requestModem("AT+GMM", 1000, false, buf);
  requestModem("AT+GMR", 1000, false, buf);
  requestModem("AT+CSQ", 1000, false, buf);
  Serial.println("done");
}


void GM862::sendSMS(char *number, char *message) {
  char buf[BUF_LENGTH];
  char cmdbuf[30] = "AT+CMGS=\"";
  Serial.println("sending SMS ...");
  requestModem("AT+CMGF=1", 1000, true, buf);             // send text sms
  strcat(cmdbuf, number);
  strcat(cmdbuf, "\"");
  requestModem(cmdbuf, 1000, true, buf);
  modem->print(message);
  modem->print(0x1a, BYTE);
  getsTimeout(buf, 2000);
  Serial.println(buf);
  Serial.println("done");
}


void GM862::checkNetwork() {
  char buf[BUF_LENGTH];
  char result;
  Serial.println("checking network ...");
  requestModem("AT+CREG?", 1000, false, buf);
  result = buf[20];
  if (result == '1') {
    state |= STATE_REGISTERED;
  }
  else {
    state &= ~STATE_REGISTERED;
  }
  Serial.println("done");
}


/*
 * eplus
 *   internet.eplus.de, eplus, eplus
 * o2
 *   internet, <>, <>
 */
void GM862::initGPRS() {
  char buf[BUF_LENGTH];
  Serial.println("initializing GPRS ...");
  requestModem("AT+CGDCONT=1,\"IP\",\"internet\",\"0.0.0.0\",0,0", 1000, false, buf);
  requestModem("AT#USERID=\"\"", 1000, false, buf);
  requestModem("AT#PASSW=\"\"", 1000, false, buf);
  Serial.println("done");
}    


void GM862::enableGPRS() {
  char buf[BUF_LENGTH];
  Serial.println("switching GPRS on ...");
  requestModem("AT#GPRS=1", 1000, false, buf);
  Serial.println("done");
}


void GM862::disableGPRS() {
  char buf[BUF_LENGTH];
  Serial.println("switching GPRS off ...");
  requestModem("AT#GPRS=0", 1000, false, buf);
  Serial.println("done");
}


boolean GM862::openHTTP(char *domain) {
  char buf[BUF_LENGTH];
  char cmdbuf[50] = "AT#SKTD=0,80,\"";
  byte i = 0;
  boolean connect = false;
  Serial.println("opening socket ...");
  strcat(cmdbuf, domain);
  strcat(cmdbuf, "\",0,0\r");
  requestModem(cmdbuf, 1000, false, buf);
  do {
    getsTimeout(buf, 1000);
    Serial.print("buf:");
    Serial.println(buf);
    if (strstr(buf, "CONNECT")) {;
      connect = true;
      break;
    }
  } while (i++ < 10);
  if (!connect) {
    Serial.println("failed");
  }
  return (connect);
}


void GM862::send(char *buf) {
  modem->print(buf);
}


char *GM862::receive(char *buf) {
  getsTimeout(buf, 1000);
  return buf;
}


void GM862::warmStartGPS() {
  char buf[BUF_LENGTH];
  Serial.println("warm start GPS ...");
  requestModem("AT$GPSR=2", 1000, false, buf);
  Serial.println("done");
}


Position GM862::requestGPS(void) {
  char buf[150];
  Serial.println("requesting GPS position ...");
  requestModem("AT$GPSACP", 2000, false, buf);
  if (strlen(buf) > 29) {
    actPosition.fix = 0;	// invalidate actual position			
    parseGPS(buf, &actPosition);
    if (actPosition.fix > 0) {
      Serial.println(actPosition.fix);
      state |= STATE_POSFIX;
    }
  }
  else {
    actPosition.fix = 0;
    Serial.println("no fix");
    state &= ~STATE_POSFIX;
  }
  return actPosition;
}


Position GM862::getLastPosition() {
  return actPosition;
}


/*
 * Parse the given string into a position record.
 * example:
 * $GPSACP: 120631.999,5433.9472N,00954.8768E,1.0,46.5,3,167.28,0.36,0.19,130707,11\r
 */
void GM862::parseGPS(char *gpsMsg, Position *pos) {

  char time[7];
  char lat_buf[12];
  char lon_buf[12];
  char alt_buf[7];
  char fix;
  char date[7];
  char nr_sat[4];
	
  gpsMsg = skip(gpsMsg, ':');                // skip prolog
  gpsMsg = readToken(gpsMsg, time, '.');     // time, hhmmss
  gpsMsg = skip(gpsMsg, ',');                // skip ms
  gpsMsg = readToken(gpsMsg, lat_buf, ',');  // latitude
  gpsMsg = readToken(gpsMsg, lon_buf, ',');  // longitude
  gpsMsg = skip(gpsMsg, ',');                // hdop
  gpsMsg = readToken(gpsMsg, alt_buf, ',');  // altitude
  fix = *gpsMsg++;                           // fix, 0, 2d, 3d
  gpsMsg++;
  gpsMsg = skip(gpsMsg, ',');                // cog, cource over ground
  gpsMsg = skip(gpsMsg, ',');                // speed [km]
  gpsMsg = skip(gpsMsg, ',');                // speed [kn]
  gpsMsg = readToken(gpsMsg, date, ',');     // date ddmmyy
  gpsMsg = readToken(gpsMsg, nr_sat, '\n');  // number of sats

  if (fix != '0') {
    parsePosition(pos, lat_buf, lon_buf, alt_buf);
    pos->fix = fix;
  }

}


/*
 * Skips the string until the given char is found.
 */
char *GM862::skip(char *str, char match) {
  uint8_t c = 0;
  while (true) {
    c = *str++;
    if ((c == match) || (c == '\0')) {
      break;
    }
  }
  return str;
}


/*
 * Reads a token from the given string. Token is seperated by the 
 * given delimiter.
 */
char *GM862::readToken(char *str, char *buf, char delimiter) {
  uint8_t c = 0;
  while (true) {
    c = *str++;
    if ((c == delimiter) || (c == '\0')) {
      break;
    }
    else if (c != ' ') {
      *buf++ = c;
    }
  }
  *buf = '\0';
  return str;
}


/*
 * Parse and convert the position tokens. 
 */
void GM862::parsePosition(Position *pos, char *lat_str, char *lon_str, char *alt_str) {
  char buf[10];
  parseDegrees(lat_str, &pos->lat_deg, &pos->lat_min);
  parseDegrees(lon_str, &pos->lon_deg, &pos->lon_min);
  readToken(alt_str, buf, '.');
  pos->alt = atol(buf);
}


/*
 * Parse and convert the given string into degrees and minutes.
 * Example: 5333.9472N --> 53 degrees, 33.9472 minutes
 * converted to: 53.565786 degrees 
 */
void GM862::parseDegrees(char *str, int *degree, long *minutes) {
  char buf[6];
  uint8_t c = 0;
  uint8_t i = 0;
  char *tmp_str;
	
  tmp_str = str;
  while ((c = *tmp_str++) != '.') i++;
  strlcpy(buf, str, i-1);
  *degree = atoi(buf);
  tmp_str -= 3;
  i = 0;
  while (true) {
    c = *tmp_str++;
    if ((c == '\0') || (i == 5)) {
      break;
    }
    else if (c != '.') {
      buf[i++] = c;
    }
  }
  buf[i] = 0;
  *minutes = atol(buf);
  *minutes *= 16667;
  *minutes /= 1000;
}

