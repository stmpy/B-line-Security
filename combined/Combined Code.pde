/*
*Program by Clinton Mullins
    *Alarm code functions by June Cho
    *Wireless Module code to be included by Travis Jeppson
*Revision Dates:
*Aug 18, 2010; Oct 22, 2010    Pin Assignments and moved Alarm Functions to End of File
*Dec 21, 2010;                 State definitions moved from block diagram to code
*                              Additional utility functions to process state functions added
*
*Dec 23, 2010                  Revised
*Car Alarm Monitor Rev. 0.3
*Additional libraries shall be documented with include statements
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
//  State CLEAR = State(ClearAlarm_Function);            //Clear Alarm, 6
 
 FSM AlarmStateMachine = FSM(INITIAL);    //Initialize the state machine to the start state

//Initialize variables & Pin Assignments
int door_unlock_R=22;
int SW_door_unlock_R=2;              //From Clinton, Unlock Switch when LOW  Duplicate instatiation
int door_lock_R=23;
int SW_door_lock_R=3;                //From Clinton, lock Switch when LOW
int door_check=5;
int light=28;
int horn=30;
int ignition_check=4;
int to_door_check=24;                //To Clinton ????
int to_ignition_check=25;            //To Clinton ????
int alarm_trigger=26;                //From Clinton
int val_SW_door_lock_R=0;
int val_door_check=0;
int val_ignition_check=0;
int val_SW_door_unlock_R=0;
int val_alarm_trigger=0;           //Initially not triggered
int x=0;
byte IsArmed = 0;                    //initially disarmed
byte state = 0;                      //Set start state to case 0
byte lastState = 0;

/***********************************************************************************************
                            Program setup and program to run.
************************************************************************************************/
//Program setup
void setup()
{
  pinMode(SW_door_unlock_R,INPUT);          //2    (5V-->5V)   ###From Clinton
  pinMode(ignition_check,INPUT);            //4    (12V-->5V)  ####Check with resister 
  pinMode(door_check,INPUT);                //5    (12V-->5V)  ####Check with resister
  pinMode(SW_door_lock_R,INPUT);            //3    (5V-->5V)   ###From CLinton
  pinMode(alarm_trigger,INPUT);             //26    (5V-->5V)   ###From Clinton
  
  pinMode(door_unlock_R,OUTPUT);            //22   (5V-->12V)
  pinMode(door_lock_R,OUTPUT);              //23   (5V-->12V)
  pinMode(light,OUTPUT);                    //28   (5V-->12V)
  pinMode(horn, OUTPUT);                    //30   (5V-->12V)
  pinMode(to_ignition_check, OUTPUT);       //25    (5V-->5V)   ###To Clinton ????
  pinMode(to_door_check, OUTPUT);           //24    (5V-->5V)   ###To Clinton
}

//Program to run
void loop(){
  if (state != lastState) {
    switch (state){
      case 0: AlarmStateMachine.transitionTo(INITIAL); break;       //Start State, initialization
      case 1: AlarmStateMachine.transitionTo(ARM); break;           //ARM the Alarm
      case 2: AlarmStateMachine.transitionTo(DISARM); break;        //Disarm the Alarm
      case 3: AlarmStateMachine.transitionTo(Trigger_Alarm); break; //Alarm Triggered
      case 4: AlarmStateMachine.transitionTo(Lock_Doors); break;    //Lock Doors
      case 5: AlarmStateMachine.transitionTo(Unlock_Doors); break;  //Unlock Doors
//    case 6: AlarmStateMachine.transitionTo(ClearAlarm_Function); break;  //Clear Alarm-remove noise  
    }
  }
       AlarmStateMachine.update();
}

/***********************************************************************************************
The following functions were created by Clinton Mullins to facilitate state change functionality.
Default to the monitor state
************************************************************************************************/

void Monitor(){
  val_SW_door_unlock_R = digitalRead(SW_door_unlock_R);   //2    (5V-->5V)   ###From Clinton
  val_ignition_check = digitalRead(ignition_check);       //4    (12V-->5V)  ###Check with resister 
  val_door_check = digitalRead(door_check);               //5    (12V-->5V)  ###Check with resister
  val_SW_door_lock_R = digitalRead(SW_door_lock_R);       //3    (5V-->5V)   ###From CLinton
  val_alarm_trigger = digitalRead(alarm_trigger);         //26   (5V-->5V)   ###From Clinton
//  Check_Arm_Code = digitalRead(Arm Code);
//  Check_Disarm_Code = digitalRead(Disarm Code);
//  Check_Lock_Code = digitalRead(Lock Code);
//  Check_Unlock_Code = digitalRead(Unlock Code);
//  Check_ClearAlarm = digitalRead(ClearAlarm Code);
  
  //Re-read sensor values and check for changes
  if (val_SW_door_unlock_R != digitalRead(SW_door_unlock_R)){
    state = 3;
  }
  if (val_ignition_check != digitalRead(ignition_check)){
    state = 3;
  }
  if (val_door_check != digitalRead(door_check)){
    state = 3;
  }
  if (val_SW_door_lock_R != digitalRead(SW_door_lock_R)){
    state = 3;
  }
  if (val_alarm_trigger != digitalRead(alarm_trigger)){
    state = 3;
  }
//  if (Check_Arm_Code != digitalRead(Arm Code)){
//    state = 1;
//  }
//  if (Check_Disarm_Code != digitalRead(Disarm Code)){
//    state = 2;
//  }
//  if (Check_Lock_Code != digitalRead(Lock Code)){
//    state = 4;
//  }
//  if (Check_Unlock_Code != digitalRead(Unlock Code)){
//    state = 5;
//  }  
//  if (Check_ClearAlarm != digitalRead(ClearAlarm Code)){
//    state = 6;
//  }  
  else {
    state = 0;                                          //If no changes keep monitoring inputs
  }
}

void Arm(){
  CheckIgnition_Function();              //Get ingition and door status
  DoorCheck_Function();  
  while(val_ignition_check==HIGH){
  break;
  }                                      //assumes HIGH is car on
  while(val_door_check==HIGH){
  DoorCheck_Function();
  }                                      //if not closed wait until closed to proceed
  IsArmed = 1;                           //Sets armed status to on
  DoorLock_Function();
  state = 3;                             //After arming the alarm, immediately go to monitoring the trigger
}

void Disarm(){
  ClearAlarm_Function();                //Set values to clear alarm
  DoorUnLock_Function();                //Unlock the doors
  IsArmed = 0;
  state = 0;
}

void AlarmTrigger(){
  CheckIgnition_Function();              //Get ingition and door status
  DoorCheck_Function();  
  if (val_door_check==HIGH and IsArmed == 1 or val_ignition_check==HIGH and IsArmed == 1){
//  if doors are unlocked and alarm is armed, trigger alarm
//  if ignition is on, trigger alarm
  val_alarm_trigger = HIGH;               //  Turn on the Alarm Trigger
  AlarmIsTriggered_Function();            //  Trigger alarm
  //Send GPS data function();             //  Fetch GPS Data
  while(x < 300000){
    if(state){                            //If commanded to quiet then break
      lastState = state;                 //Sets state to the new state value
      break;                              //  5 minutes fo auto time out
    }
    x++;
   }
  }
}

void ClearAlarm_Function(){
  //Turn off the noise, remain tripped
  digitalWrite(horn, LOW);     // set the horn off
  digitalWrite(light, LOW);    // set the lights off
  val_alarm_trigger = LOW;  //Set the Alarm Trigger off
  state = 0;
}