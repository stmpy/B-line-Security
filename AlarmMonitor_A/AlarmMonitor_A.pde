/*
*Program by Clinton Mullins
*July 7, 2010
*Car Alarm Monitor Rev. 0.1
*
*Additional libraries documented with include statements
*/

//http://www.arduino.cc/playground/uploads/Code/FSM.zip
#include <FiniteStateMachine.h>

const byte NUMBER_OF_STATES = _; //how many states are we cycling through?

/* initialize states
State On = State(ledOn);
*/
State SysHealthPass = State(HealthPass);
State SysHealthFail = State(HealthFail);
State DoorsUnlocked = State(DoorUnlock);
State DoorsLocked = State(DoorLock);
State DoorsClosed = State(DoorClosed);
State DoorsOpened = State(DoorOpen);
State AlarmArmed = State(Arm);
State AlarmDisarmed = State(Disarm);
State AlarmTriggered = State(Triggered);
State IgnitionOn = State(Ignitionon);
State IgnitionOff = State(Ignitionoff);

//initialize state machine, start in state: CheckHealth
FSM AlarmStateMachine = FSM();

void setup()
{ //Declare pin I/O's
//Inputs for variables
//All inputs will require a 10K pull-up/down resistor
  int dlockPin = 22;  //Input monitor door lock status
  int ignitionPin = 24;  //Input monitor ignition status
  int syshealthPin = 26;  //Input Monitor system Health
  int triggerPin = 28;  //Input Alarm triggered
  //int setPin = 30;  //Input Alarm Set
  int docPin = 32;  //Input, Doors open/closed
  int alarmADPin = 34;  //Input monitor alarm armed/disarmed
//Outputs for variables 
  int dlockcmdPin = 36;  //Output
  int alarmADcmdPin = 38;  //Input

//Inputs for pinMode
  pinMode(dlockPin, INPUT);
  pinMode(ignitionPin, INPUT);
  pinMode(syshealthPin, INPUT);
  pinMode(triggerPin, INPUT);
  //pinMode(setPin, INPUT);
  pinMode(docPin, INPUT);
  pinMode(alarmADPin, INPUT);
//Outputs for pinMode 
  pinMode(dlockcmdPin, OUTPUT);
  pinMode(alarmADcmdPin, OUTPUT);
}

void loop()
{ //Program to run

  if (button.uniquePress()){
    //increment buttonPresses and constrain it to [ 0, 1, 2, 3 ]
    //Fix this statement to switch automatically
    buttonPresses = ++buttonPresses % NUMBER_OF_STATES;
    switch (buttonPresses){
      case 0: ledStateMachine.transitionTo(On); break;
      case 1: ledStateMachine.transitionTo(Off); break;
      case 2: ledStateMachine.transitionTo(FadeIn); break;
      case 3: ledStateMachine.transitionTo(FadeOut); break;
    }
  }
  AlarmStateMachine.update();
}


/*
//utility functions
void ledOn(){ led.on(); }
void ledOff(){ led.off(); }
void ledFadeIn(){ led.fadeIn(500); }
void ledFadeOut(){ led.fadeOut(500); }
//end utility functions
*/

