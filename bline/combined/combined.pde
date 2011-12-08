/*
* Derived from code written by Clinton Mullins, Travis Jeppson, June Cho
* Revision Dates:
* Aug 18, 2010; Oct 22, 2010    Pin Assignments and moved Alarm Functions to End of File
* Dec 21, 2010;                 State definitions moved from block diagram to code
*                              Additional utility functions to process state functions added
*
* Dec 23, 2010                  Revised
*
*
* Future changes:
* 1) Integrate XBee module so that the vehicle can detect the owner w/in a specific proximity
* 2) Change the code of the arduino to use interupts for the digital line communication to the vehicle
*
* Car Alarm Monitor Rev. 0.3
* Additional libraries shall be documented with include statements
*/

#include "WProgram.h"
//http://www.arduino.cc/playground/uploads/Code/FSM.zip
#include "FiniteStateMachine.h"
#include "car.h"
#include "GM862.h"

//Define & Initialize States
State INITIAL = State(Monitor);                         //Start, 0
State ARM = State(Arm);                                 //ARM the Alarm, 1
State DISARM = State(Disarm);                           //Disarm the Alarm, 2
State Trigger_Alarm = State(AlarmTrigger);              //Alarm Triggered, 3
State Lock_Doors = State(DoorLock_Function);            //Lock Doors, 4
State Unlock_Doors = State(DoorUnLock_Function);        //Unlock Doors, 5
State CLEAR = State(ClearAlarm_Function);            //Clear Alarm, 6
State LIGHTS_OFF = State(Lights_Off_Function);
State LIGHTS_ON = State(Lights_On_Function);

//Initialize variables & Pin Assignments
// OUTPUTS -- these will never change within the program
#define NUM_OF_OUTPUTS          4
#define UNLOCK_CONTROL          23 // door_unlock_R
#define LOCK_CONTROL            25 // door_lock_R
#define LIGHT_CONTROL           29 // light
#define HORN_CONTROL            31 // horn
#define ARMED                   27 //Status LED armed or not

// INPUTS -- these will never change within the program
#define NUM_OF_INPUTS           4
#define UNLOCK_CHECK            47 // SW_door_unlock_R --From Clinton, Unlock Switch when LOW  Duplicate instatiation
#define LOCK_CHECK              49 // SW_door_lock_R
#define DOOR_CHECK              53 // door_check
#define IGNITION_CHECK          51 // ignition_check

// presentation input
#define PRESENTATIONBUTTON      45

// modem defines
#define BUF_LENGTH              100
#define AMOUNT_OF_PHONE_NUMBERS 10
#define NUMBER_OF_COMMANDS      8
#define ON_PIN                  33 // pin to toggle the modem's on/off

// States
#define MONITOR_STATE           0
#define ARM_STATE               1
#define DISARM_STATE            2
#define TRIGGER_ALARM_STATE     3
#define LOCK_STATE              4
#define UNLOCK_STATE            5
#define CLEAR_ALARM_STATE       6
#define LIGHTS_OFF_STATE        7
#define LIGHTS_ON_STATE         8

int alarm_trigger=26;                //From Clinton
int val_SW_door_lock_R=0;
int val_door_check=0;
int val_ignition_check=0;
int val_SW_door_unlock_R=0;
int val_alarm_trigger=0;           //Initially not triggered
byte IsArmed = 0;                    //initially disarmed
int state = 0;                      //Set start state to case 0
int currentState = 0;
byte lastState = 0;
int x_alarm=0;
bool messageReceived = false;
int prevState = -1;
bool presentation = false;

// Car object variables
int outputs[NUM_OF_OUTPUTS] = { // need to be in this order
                UNLOCK_CONTROL,
                LOCK_CONTROL,
                LIGHT_CONTROL,
                HORN_CONTROL};
int inputs[NUM_OF_INPUTS] = { // need to be in this order
                UNLOCK_CHECK,
                LOCK_CHECK,
                DOOR_CHECK,
                IGNITION_CHECK};
// modem object variables
char cmd;                            // command read from terminal
char buf[BUF_LENGTH];                // buffer used to capture messages from the modem
String commands[NUMBER_OF_COMMANDS+1] = {"initialize",
                            "arm",
                            "disarm",
                            "trigger alarm",
                            "lock",
                            "unlock",
                            "clear alarm",
                            "lights off",
                            "lights on"};
String responses[NUMBER_OF_COMMANDS+1] = {"Initialized!!",
                            "System Armed",
                            "System Disarmed",
                            "Alarm Triggered",
                            "Doors Locked",
                            "Doors Unlocked",
                            "Alarm Cleared",
                            "Headlights are now off",
                            "Headlights are now on"};
String phoneNumbers[AMOUNT_OF_PHONE_NUMBERS] = {"##########","##########"};
// declarations
Car car = Car(inputs,outputs);
FSM AlarmStateMachine = FSM(INITIAL);    //Initialize the state machine to the start state
GM862 modem(&Serial1 , ON_PIN, phoneNumbers, commands, responses, NUMBER_OF_COMMANDS);   // modem is connected to Serial3

/***********************************************************************************************
                            Program setup and program to run.
************************************************************************************************/
//Program setup
void setup()
{
    pinMode(UNLOCK_CHECK,INPUT);          //2    (5V-->5V)   ###From Clinton
    pinMode(IGNITION_CHECK,INPUT);            //4    (12V-->5V)  ####Check with resister 
    pinMode(DOOR_CHECK,INPUT);                //5    (12V-->5V)  ####Check with resister
    pinMode(LOCK_CHECK,INPUT);            //3    (5V-->5V)   ###From CLinton
    pinMode(alarm_trigger,INPUT);             //26    (5V-->5V)   ###From Clinton
    // presentation
    pinMode(PRESENTATIONBUTTON, INPUT);
    
    pinMode(UNLOCK_CONTROL,OUTPUT);            //22   (5V-->12V)
    pinMode(LOCK_CONTROL,OUTPUT);              //23   (5V-->12V)
    pinMode(LIGHT_CONTROL,OUTPUT);                    //28   (5V-->12V)
    pinMode(HORN_CONTROL, OUTPUT);                    //30   (5V-->12V)
    pinMode(ARMED, OUTPUT);
    
    val_SW_door_unlock_R = digitalRead(UNLOCK_CONTROL);   //2    (5V-->5V)   ###From Clinton
    val_ignition_check = digitalRead(IGNITION_CHECK);       //4    (12V-->5V)  ###Check with resister 
    val_door_check = digitalRead(DOOR_CHECK);               //5    (12V-->5V)  ###Check with resister -- this needs to be HIGH
    val_SW_door_lock_R = digitalRead(LOCK_CONTROL);       //3    (5V-->5V)   ###From CLinton
    val_alarm_trigger = digitalRead(alarm_trigger);         //26   (5V-->5V)   ###From Clinton
    
    // modem setup
    Serial.begin(19200);
    modem.init();                       // initialize the GM862
}

//Program to run
void loop(){
    currentState = state;
    switch (state){ // changes state
      case MONITOR_STATE      : AlarmStateMachine.transitionTo(INITIAL);       break; //Start State, initialization
      case ARM_STATE          : AlarmStateMachine.transitionTo(ARM);           break; //ARM the Alarm
      case DISARM_STATE       : AlarmStateMachine.transitionTo(DISARM);        break; //Disarm the Alarm
      case TRIGGER_ALARM_STATE: AlarmStateMachine.transitionTo(Trigger_Alarm); break; //Alarm Triggered
      case LOCK_STATE         : AlarmStateMachine.transitionTo(Lock_Doors);    break; //Lock Doors
      case UNLOCK_STATE       : AlarmStateMachine.transitionTo(Unlock_Doors);  break; //Unlock Doors
      case CLEAR_ALARM_STATE  : AlarmStateMachine.transitionTo(CLEAR);         break; //Clear Alarm-remove noise  
      case LIGHTS_OFF_STATE   : AlarmStateMachine.transitionTo(LIGHTS_OFF);    break;
      case LIGHTS_ON_STATE    : AlarmStateMachine.transitionTo(LIGHTS_ON);     break;
    }
    AlarmStateMachine.update();
    if(state == currentState){
        state = 0;   // always return to the monitor state;
    }
}

/***********************************************************************************************
The following functions were created by Clinton Mullins to facilitate state change functionality.
Default to the monitor state
************************************************************************************************/

void Monitor(){
  Serial.println("Monitor State");
  if (modem.checkForMessage(buf) && !messageReceived){
    Serial.println("Message found");
    state = modem.parseMessage(buf);
    if(state >= 0){
        Serial.print("State: ");
        Serial.println(state);
        messageReceived = true;
//    Serial.println(modem.parseMessage(buf));
    // STOP TRAVIS --- GO CLINTON!
        if(presentation==true){
            while(digitalRead(PRESENTATIONBUTTON)==LOW){}
            presentation = false;
        }
        prevState = state;
    } else {
        state = 0;
    }
    return;
  }
  if (messageReceived && prevState != -1) { // action has completed, respond to previous txt
      messageReceived = false;
      modem.sendSMS(modem.getMessagePhoneNumber(),responses[prevState]);
      modem.clearMessagePhoneNumber();
      prevState = -1;
  }
  Serial.print("Door status: ");
  if(car.check_doors()==HIGH){
    Serial.println("HIGH");
  } else {
    Serial.println("LOW");
  }
  if (val_ignition_check != car.check_ignition() || (val_door_check != car.check_doors() && car.check_doors()==LOW) && IsArmed){
    state = 3;
    return;
  }
  if (val_SW_door_unlock_R != car.check_lock()/*digitalRead(LOCK_CONTROL)*/){
    val_SW_door_unlock_R = car.check_lock(); // update
    if (val_SW_door_unlock_R==LOW/*digitalRead(LOCK_CONTROL)*/){
    state = 5;//unlock
    return;
    }
    if (val_SW_door_unlock_R==HIGH /*digitalRead(LOCK_CONTROL)*/){
    state = 4;//lock
    return;
    }
  }
//  if (Check_ClearAlarm != digitalRead(ClearAlarm Code)){
//    state = 6;
//  }  
//  else {
//    state = 0;                                          //If no changes keep monitoring inputs
//  }

//  ArmControlStatus -- check button status ... 
//  if(digitalRead(Arm_Control)==HIGH){
//    if(IsArmed == 1){
//      return;
//    }
//      else{
//            Serial.println("Armed");
//            state = 1;}
//  }
//  if(digitalRead(Arm_Control)==LOW){
//    if(IsArmed == 0){
//      return;
//    }
//      else{
//            Serial.println("Disarmed");
//            state = 2;}
//  }
}

void Arm(){
  Serial.println("Arm State");
  digitalWrite(ARMED, HIGH);
  car.lock();
  IsArmed = 1;                           //Sets armed status to on
//  state = 0;                             //After arming the alarm, immediately go to monitoring the trigger
}

void Disarm(){
  Serial.println("Disarm State");
  digitalWrite(ARMED, LOW);
  car.unlock();
  IsArmed = 0;
//  state = 0;
}

void AlarmTrigger(){ 
    Serial.println("Alarm Trigger State");   
//  Serial.println("Alarm has been triggered");
//  if ((car.check_doors()/*digitalRead(DOOR_CHECK)*/ == HIGH or car.check_ignition()/*digitalRead(IGNITION_CHECK)*/ == HIGH) and IsArmed == 1){
    //Send GPS data function();             //  Fetch GPS Data
        if(x_alarm == 0){
            modem.sendAllSMS(phoneNumbers[0], "YOUR ALARM IS GOING OFF!!");
        }
        x_alarm++;
//    while(x < 5){
        car.alarm();      
//      Serial.println("Alarm in action");
//        if(isArmed == 0/*digitalRead(Arm_Control) == LOW*/){    //Kick out of loop if disarmed
//            state = 2;
//            return;
//            break;
//        }                                //  1 minutes for auto time out
//        Serial.println("past car alarm");
//        if(val_alarm_trigger == 0){            //Kick out of loop if cleared
//            return;
//        }
        if(x_alarm >= 10){
            state = 6;
        }
//Check state against what put it into the trigger
//        Serial.print("Got here!!!\nstate: ");
//        Serial.println(state);
//        state = 0;
// return;
//    }
}

void ClearAlarm_Function(){
  Serial.println("Clear Alarm State");
  //Turn off the noise, remain tripped
  //car.clear_alarm();
//  val_alarm_trigger = 0;  //Set the Alarm Trigger off
    val_ignition_check = car.check_ignition();//digitalRead(IGNITION_CHECK);
    val_door_check = car.check_doors();//digitalRead(DOOR_CHECK);
//    state = 0;
    x_alarm = 0;
}

void DoorLock_Function(){
  Serial.println("Door Lock State");
  car.lock();
  val_SW_door_unlock_R = digitalRead(LOCK_CONTROL); 
//  state=0;  
}

void DoorUnLock_Function(){
  Serial.println("Door Unlock State");
  car.unlock();
  val_SW_door_unlock_R = digitalRead(LOCK_CONTROL);
//  state=0;
}

void Lights_On_Function(){
    Serial.println("Lights on State");
    car.lightsOn();
}

void Lights_Off_Function(){
    Serial.println("Lights on State");
    car.lightsOff();
}