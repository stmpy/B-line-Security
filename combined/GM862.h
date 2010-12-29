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
#define AMOUNT_OF_PHONE_NUMBERS 10

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
  GM862(HardwareSerial *modemPort, byte onOffPin, String *phoneNumber, String *commands, String *responses, int numOfCommands);

  void init();
  boolean getStatus();
  void checkNetwork();

  boolean isOn();
  boolean isInitialized();
  boolean isRegistered();
  boolean isPosFixed();

  void switchOn();
  void switchOff();

  void sendSMS(String snumber, String message);
  byte checkForMessage(char *buf);
  void version();
  void sendCoordinates();

  void initGPRS();
  void enableGPRS();
  void disableGPRS();
  boolean openHTTP(char *domain);
  void requestHTTP();
  void send(char *buf);
  char *receive(char *buf);

  void warmStartGPS();
  Position requestGPS();
  Position getLastPosition();
  void clearMessages();
  int parseMessage(char *buf);

 private:
  void switchModem();
  byte requestModem(const char *command, uint16_t timeout, boolean check, char *buf);
  byte getsTimeout(char *buf, uint16_t timeout);
  void deleteMessage(String index);
  String getPhone(String phone);
  String getDate(String date);
  String getCommand(String s);

  void parseGPS(char *gpsMsg, Position *pos);
  void parsePosition(Position *pos, char *lat_str, char *lon_str, char *alt_str);
  void parseDegrees(char *str, int *degree, long *minutes);
  char *readToken(char *str, char *buf, char delimiter);
  char *skip(char *str, char match);
  int executeCommand(String command);
  int extractData(char *buf2);
  String returnMessage(int state);
  boolean verifyPhoneNumber(String number);

  byte state;
  byte onOffPin;
  HardwareSerial *modem;
  Position actPosition;
  String *validPhoneNumbers;
  String *commands;
  String *responses;
  int numOfCommands;

};


#endif
