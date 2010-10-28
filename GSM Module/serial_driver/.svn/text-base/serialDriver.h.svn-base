/*
 * serialDriver.h
 * http://tinkerlog.com
 */

#ifndef serialDriver_h
#define serialDriver_h

#include "WProgram.h"

#define STATE_NONE 0
#define STATE_ON 1
#define STATE_INITIALIZED 2
#define STATE_REGISTERED 4
#define STATE_POSFIX 8

typedef struct {
	int lat_deg;
	long lat_min;
	int lon_deg;
	long lon_min;
	long alt;
	byte fix;
} Position;

class serialDriver {

 public:
  serialDriver(HardwareSerial *modemPort);

  void send(char *buf);
  char *receive(char *buf);
  
 private:
  byte requestModem(const char *command, uint16_t timeout, boolean check, char *buf);
  byte getsTimeout(char *buf, uint16_t timeout);
  byte state;
  HardwareSerial *modem;
};


#endif