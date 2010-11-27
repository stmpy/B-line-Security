/*
 * GM862.h
 * http://tinkerlog.com
 */

#ifndef GM862_h
#define GM862_h

#include "WProgram.h"
// #include "NewSoftSerial.h"


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

class GM862 {

 public:
  GM862(HardwareSerial *modemPort, byte onOffPin, char *pin);

  void init();
  void checkNetwork();

  boolean isOn();
  boolean isInitialized();
  boolean isRegistered();
  boolean isPosFixed();

  void switchOn();
  void switchOff();

  void sendSMS(char *number, char *message);
  void version();

  void initGPRS();
  void enableGPRS();
  void disableGPRS();
  boolean openHTTP(char *domain);
  void send(char *buf);
  char *receive(char *buf);

  void warmStartGPS();
  Position requestGPS();
  Position getLastPosition();

 private:
  void switchModem();
  byte requestModem(const char *command, uint16_t timeout, boolean check, char *buf);
  byte getsTimeout(char *buf, uint16_t timeout);

  void parseGPS(char *gpsMsg, Position *pos);
  void parsePosition(Position *pos, char *lat_str, char *lon_str, char *alt_str);
  void parseDegrees(char *str, int *degree, long *minutes);
  char *readToken(char *str, char *buf, char delimiter);
  char *skip(char *str, char match);

  char pin[5];
  byte state;
  byte onOffPin;
  HardwareSerial *modem;
  Position actPosition;

};


#endif
