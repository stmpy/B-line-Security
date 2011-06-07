/* Car interface class for B-Line Security
 * Test bed and demonstration file, this file is not actually needed to implement
 * the car interface, it is soely a demonstration on how to use the car
 * interface class
 *
 * Written by June Cho
 * Modified by Travis Jeppson
 *
 * B-Line Security Senior Project 2010
 * Members
 * Travis Jeppson
 * June Cho
 * Clinton Mullins
 *
 */
 
#include "car.h"

Car::Car(int *i,int *o){
    byte blinkTemp[] = {HIGH, LOW};
    int blinkTemp_dur[] = {200,200};
    
    memcpy(this->blink,blinkTemp,2);
    memcpy(this->blink_dur,blinkTemp_dur,4);
    
    this->unlock_control = o[0];
    this->lock_control = o[1];
    this->light_control = o[2];
    this->horn_control = o[3];
    this->door_control = o[4];
    this->ignition_control = o[5];
    
    this->unlock_check = i[0];
    this->lock_check = i[1];
    this->door_check = i[2];
    this->ignition_check = i[3];
}

/***********************************************************************************************
The following functions were created by June Cho specifically for his car module.
These will need to be moved to a separate file to be called as function calls within
the case statements.
***********************************************************************************************/

void Car::unlock(){
    ///////////////// [UNLOCK] ////////////////
    // Set Headlight and horn to blink and honk twice when the door is unlocked
    digitalWrite(this->unlock_control, HIGH);     delay(100);         //door unlock
    digitalWrite(this->unlock_control, LOW);
    morse_code(this->blink,this->blink_dur,BLINK_LENGTH,true);        // blink lights honk horn
    morse_code(this->blink,this->blink_dur,BLINK_LENGTH,false);        // blink lights
    // turn signal off
}

void Car::lock(){
    ///////////////// [LOCK] //////////////
    //Set both left and right doors to lock
    digitalWrite(this->lock_control,HIGH);     delay(100); //LOCK!!!
    digitalWrite(this->lock_control,LOW);      delay(100);
    digitalWrite(this->lock_control,HIGH);     delay(100);
    digitalWrite(this->lock_control,LOW);
    // lights and horn
    morse_code(this->blink,this->blink_dur,BLINK_LENGTH,true);
}

void Car::alarm(){
 ///////////////// [TRIGGER-->HORN/LIGHT] ////////////////
  int length = 6;
  byte code[] = {HIGH,LOW,HIGH,LOW,HIGH,LOW};
  int s_dur[] = {100,200,100,200,100,200};
  int o_dur[] = {200,200,200,200,200,200};
  //SOS    !!!!!!! MORSE CODE !!!!!!!
  morse_code(code,s_dur,length,true);
  morse_code(code,o_dur,length,true);
  morse_code(code,s_dur,length,true);
}

byte Car::check_ignition(){
 ///////////////// [IGNITION CHECK] ////////////////
  return digitalRead(this->ignition_check);
}

byte Car::check_doors(){
  ///////////////// [DOOR CHECK] ////////////////
  return digitalRead(this->door_check);
}

byte Car::check_unlock(){
    return digitalRead(this->unlock_check);
}

byte Car::check_lock(){
    return digitalRead(this->lock_check);
}

void Car::clear_alarm(){
    
}

void Car::morse_code(byte *code,int *duration,int length,bool horn){
    
    for(int i=0;i<length;i++){
/*        if(code[i] == HIGH){
            Serial.print("HIGH");
        } else if(code[i] == LOW){
            Serial.print("LOW");
        } else {
            Serial.print("Unknown state");
        }
        Serial.println();
        Serial.print("dur: ");
        Serial.println(duration[i]);
*/        if(horn){digitalWrite(this->horn_control, code[i]);}
        digitalWrite(this->light_control, code[i]);
        delay(duration[i]*2);
    }
}
