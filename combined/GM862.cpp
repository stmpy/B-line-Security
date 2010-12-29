/*
 * GM862.cpp
 * http://tinkerlog.com
 */

#include "GM862.h"
//#include <string.h>
//#include <Time.h>
#include <avr/pgmspace.h>

#define BUF_LENGTH 200
#define AMOUNT_OF_PHONE_NUMBERS 10
//Define States
#define START 0 //Start
#define ARM 1 //ARM the Alarm
#define DISARM 2 //Disarm the Alarm
#define Trigger_Alarm 3 //Alarm Triggered
#define Lock_Doors 4 //Lock Doors
#define Unlock_Doors 5 //Unlock Doors

GM862::GM862(HardwareSerial *modemPort, byte onOffPin, String *phoneNumbers, String *commands, String *responses, int numOfCommands) {
  state = STATE_NONE;
  modem = modemPort;
  modem->begin(19200);
  this->onOffPin = onOffPin;
  this->validPhoneNumbers = phoneNumbers;
  this->commands = commands;
  this->responses = responses;
  this->numOfCommands = numOfCommands;
  pinMode(onOffPin, OUTPUT);
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
//  Serial.println(command);
  modem->print(command);
  modem->print('\r');
  count = getsTimeout(buf, timeout);
  if (count) {
    if (check) {                        // if a check is desired
      found = strstr(buf, "\r\nOK\r\n");
      if (found) {
//	Serial.println("->ok");
      }
      else {
//	Serial.print("->not ok: ");
//	Serial.println(buf); 
        count = 0;
      } 
    }
    else {
//      Serial.print("->buf: ");
//      Serial.println(buf);  
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
  //Serial.println("getsTimout timeout: " + String(timeout));
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
  delay(1000);
  getStatus();                    // identify if modem is turned on or not
  Serial.println("GM862 MC Control");
  // test to see if modem is on
  if(!isOn()){
    Serial.println("Turning on Modem ...");
    switchOn();                   // switch the modem on
    delay(4000);                        // wait for the modem to boot
  }
  Serial.println("initializing modem ...");
  char buf[BUF_LENGTH];
  requestModem("AT", 1000, true, buf);
  requestModem("AT+IPR=19200", 1000, true, buf);
  requestModem("AT+CMEE=2", 1000, true, buf);
  requestModem("AT+CNMI=2,1,2,1,0", 1000, true, buf);
  requestModem("AT+CMGF=1", 1000, true, buf);             // send text sms
  state |= STATE_INITIALIZED;
  checkNetwork();             // check the network availability
//  modem.version();                    // request modem version info
/*  while (!modem.isRegistered()) {
    delay(1000);
    modem.checkNetwork();             // check the network availability
  }*/
  Serial.println("Modem is ready");
}
boolean GM862::getStatus(){
  char buf[BUF_LENGTH];
  byte r = 0;
  if(state == STATE_NONE){
  r = requestModem("AT",1000,true,buf);
//  Serial.println("r: " + r);
    if(r){
        state |= STATE_ON;
    }
  }
  return state;
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
void GM862::sendSMS(String number, String message) {
  char buf[BUF_LENGTH];
  char cmdbuf[30];
  String Scmdbuf = "AT+CMGS=\"";
  Serial.println("sending SMS ...");
  Serial.println("number: "+number+" and message: "+message);
  Scmdbuf.concat(number + "\"");
  Scmdbuf.toCharArray(cmdbuf,30);
  requestModem(cmdbuf, 1000, false, buf);
  modem->print(message);
  modem->print(0x1a, BYTE);
  getsTimeout(buf, 2000);
//  Serial.println(buf);
//  Serial.println("done");
}
byte GM862::checkForMessage(char *buf) {
  return getsTimeout(buf,1000);
}
void GM862::clearMessages(){
  deleteMessage("0");
}
int GM862::parseMessage(char *buf){
    char buf2[BUF_LENGTH];
    int state;
    String loc;
//    Serial.print("pm -->");
//    Serial.println(buf);
    if(strstr(buf,"CMTI")){ // incoming txt message
        Serial.println("getting message ... ");
        loc = strstr(buf,",")+1;
        loc = loc.trim();
//        Serial.println("mem loc: " + loc);
        requestModem("AT+CMGL=\"REC UNREAD\"",1000,false,buf2); // list message
	   while(!strstr(buf2,"+CMGL:")){	// wait until the txt message response is sent.
	       getsTimeout(buf2,100);
	   }
//      Serial.print(buf2);
    state = extractData(buf2);
    delay(4500);
    deleteMessage(loc);
    }
  return state;
}
String GM862::getCommand(String s){
    int location = s.lastIndexOf("OK");
//    Serial.println("location: " + location);
    s = s.substring(0,location - 2).trim();
//    Serial.println("extracted command as: '" + s + "'");
    return s;
}
String GM862::getPhone(String phone){
  if(!phone.length() >= 13){
    Serial.println("Phone number was an invalid length: " + phone.length());
    return "";	// illegal amount of characters and will crash something ...
  }
  return phone.substring(3,13);
}
String GM862::getDate(String date){
  return "";
}
int GM862::extractData(char *buf2){
  String phoneNumber;
  String date;
  String command;
  String tempS;
  int counter = 0;
  int counter2 = 0;
  int state = -1;
  char * temp2;
  char * temp = strtok(buf2,",");
  while(temp != NULL){
    tempS = temp;
    switch(counter){
      case 2:    // phone number
//	      Serial.println("phone number (pre): " + tempS);
//	      Serial.println("length: " + tempS.length());
	phoneNumber = getPhone(tempS);
//	      Serial.println("phone number: " + phoneNumber);
	break;
      case 4:    // year month and day
//	      Serial.println("ymd: " + tempS);
//	      Serial.println("current date: " + year() +"/"+ month() +"/"+ day());
	date = tempS;
	break;
      case 5:    // time
//	      Serial.println("time: " + tempS);
//	      Serial.println("current date: " + hour() +":"+ minute() +":"+ second() + "-24");
	temp2 = strtok(temp,"\"");
	while(temp2 != NULL){
	  tempS = temp2;
	  tempS = tempS.trim();
//            Serial.println("inner temp: " + tempS);
	  switch(counter2){
	    case 1:
	      command = getCommand(tempS);
	      break;
	    default:
//                    Serial.println("don't care: " + tempS);
	      break;
	  }
	  counter2++;
	  temp2 = strtok(NULL,"\"");
	}
	date += tempS;
	break;
      default:
//        Serial.println("don't care: " + tempS);
	break;
    }
    counter++;
//	  Serial.println("temp: " + tempS);
//	  Serial.println("counter: " + counter);
    temp = strtok(NULL,",");
  }
  if(verifyPhoneNumber(phoneNumber)){
    state = executeCommand(command);
    sendSMS(phoneNumber, returnMessage(state));
  } else {
    sendSMS(phoneNumber, "This phone number is not authorized.");
  }
  return state;
}
int GM862::executeCommand(String command){
    //  Serial.println("command: '" + command + "'\nlength: " + command.length());
	// parse command and execute
	int state = -1;
    if(command.equalsIgnoreCase("reboot")){
	  switchOff();
	  delay(8000);
	  switchOn();
	  delay(16000);
	  init();
	  state = 97;
	} else if(command.equalsIgnoreCase("delete messages")){
	  deleteMessage("0");
	  state = 96;
	} else if(command.equalsIgnoreCase("help")){
	  state = 95;
	}
	for(int i=0;i<=numOfCommands;i++){
//	   Serial.println("'" + command + "' ?? '" + commands[i] + "'");
	   if(command.equalsIgnoreCase(commands[i])){
	       state = i;
	       break;
	   }
	}
    return state;
}
String GM862::returnMessage(int state){
  String message = "Command not recognized";
  if(state >= 0)
      message = responses[state];
  switch(state){
    case 95:
      message = "Commands:\nON -- turn the LED on\nOFF -- turn the LED off\nREBOOT -- reboot the modem\nDELETE MESSAGES -- delete all read messages\nHELP -- prints help";
      break;
    case 96:
      message = "Messages deleted.";
      break;
    case 97:
      message = "modem has successfully rebooted";
      break;
    case 98:
      message = "LED turned OFF";
      break;
    case 99:
      message = "LED turned ON";
      break;
  }
  return message;
}
boolean GM862::verifyPhoneNumber(String number){
//    Serial.println("checking phone number: " + number);
    for(int i=0;i<AMOUNT_OF_PHONE_NUMBERS;i++){
//        Serial.println("checking number: " + this->validPhoneNumbers[i]);
        if(this->validPhoneNumbers[i].equals(number)){
//            Serial.println("found a match");
            return true;
        }
    }
    return false;
}
void GM862::deleteMessage(String index){
    char buf[BUF_LENGTH];
    char cmdBuf[BUF_LENGTH];
    char delflag;
    String cmd = "AT+CMGD=";
    if(index.equalsIgnoreCase("0")){
        cmd.concat("1,3");  // delete all read messages from <memr> storage, sent and unsent mobile originated messages, leaving unread messages untouched
    } else {
        index.concat(",0");
        cmd.concat(index);
    }
    cmd.toCharArray(cmdBuf,BUF_LENGTH);
    requestModem(cmdBuf, 2000,true,buf);
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
void GM862::requestHTTP() {
  char buf[100];
  byte i = 0;
  initGPRS();                   // setup of GPRS context
  enableGPRS();                 // switch GPRS on
  openHTTP("search.twitter.com");    // open a socket
  Serial.println("sending request ...");
  send("GET /search.atom?q=gm862 HTTP/1.1\r\n"); // search twitter for gm862
  send("HOST: search.twitter.com port\r\n");     // write on the socket
  send("\r\n");
  Serial.println("receiving ...");
  while (i++ < 10) {                  // try to read for 10s
    receive(buf);               // read from the socket, timeout 1s
    if (strlen(buf) > 0) {            // we received something
      Serial.print("buf:"); Serial.println(buf);
      i--;                            // reset the timeout
    }
  }
  Serial.println("done");
  disableGPRS();                // switch GPRS off
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
    if (strstr(buf, "CONNECT")) {
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