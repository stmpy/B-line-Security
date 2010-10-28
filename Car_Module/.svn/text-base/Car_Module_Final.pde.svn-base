

int door_unlock_R=13;
int SW_door_unlock_R=2;              //From Clinton, Unlock Switch when LOW
int val_SW_door_unlock_R=0;

int door_lock_R=12;
int SW_door_lock_R=3;                //From Clinton, lock Switch when LOW
int val_SW_door_lock_R=0;

int light=11;

int horn=10;

int ignition_check=4;
int val_ignition_check=0;
int door_check=5;
int val_door_check=0;



int to_ignition_check=7;            //To Clinton
int to_door_check=8;                //To Clinton

int alarm_trigger=6;                //From Clinton
int val_alarm_trigger=0;

void setup()
{
  pinMode(door_unlock_R,OUTPUT);            //13    (5V-->12V)
  pinMode(SW_door_unlock_R,INPUT);          //2     (5V-->5V)   ###From Clinton
  pinMode(door_lock_R,OUTPUT);              //12    (5V-->12V)
  pinMode(SW_door_lock_R,INPUT);            //3     (5V-->5V)   ###From CLinton

  pinMode(light,OUTPUT);                    //11    (5V-->12V)
  pinMode(horn, OUTPUT);                    //10    (5V-->12V)

  pinMode(ignition_check,INPUT);            //4    (12V-->5V)        ####Check with resister 
  pinMode(door_check,INPUT);                //5    (12V-->5V)        ####Check with resister

  pinMode(alarm_trigger,INPUT);             //6    (5V-->5V)   ###From Clinton

  pinMode(to_ignition_check, OUTPUT);       //7    (5V-->5V)   ###To Clinton
  pinMode(to_door_check, OUTPUT);           //8    (5V-->5V)   ###To Clinton

//Remain #1, #9


}



void loop()
{

///////////////// [LOCK/UNLOCK] ////////////////

  val_SW_door_unlock_R=digitalRead(SW_door_unlock_R);
  val_SW_door_lock_R=digitalRead(SW_door_lock_R);
  
  if(val_SW_door_unlock_R==HIGH)
  {
    digitalWrite(door_unlock_R,LOW);    //unlock is not working 
  }
  if(val_SW_door_unlock_R==LOW)      //door unlock 
  {
    digitalWrite(door_unlock_R,HIGH);
    delay(500);
    digitalWrite(light, HIGH);   //light on for 0.5 sec
    delay(500);
    digitalWrite(light, LOW);    // set the LED off
    delay(500);     
    digitalWrite(light, HIGH);  //light on for 0.5 sec   
    delay(500);                   // so light blink twice when door is open

    digitalWrite(horn, HIGH);  //Horn on for 0.5 sec   
    delay(500);                  
    digitalWrite(horn, LOW);  //Horn on for 0.5 sec   
    delay(500);                  
    digitalWrite(horn, HIGH);  //Horn on for 0.5 sec   
    delay(500);                  
    }
      //Unlock!!!!

  if(val_SW_door_lock_R==HIGH)
  {
    digitalWrite(door_lock_R,LOW);
  }
  if(val_SW_door_lock_R==LOW)      //LOCK!!!
  {
    digitalWrite(door_lock_R,HIGH); //Left door open
    delay(500);
    digitalWrite(door_lock_R,LOW);
    delay(500);
    digitalWrite(door_lock_R,HIGH); //Right door open
    delay(500);
    digitalWrite(light, HIGH);   //light on for 2 sec
    delay(2000);
    
    
    digitalWrite(horn, HIGH);  //Horn on for 0.5 sec   
    delay(500);                  
    
    
  }
      //Lock
///////////////// [LOCK/UNLOCK] ////////////////





///////////////// [TRIGGER-->HORN/LIGHT] ////////////////
  val_alarm_trigger=digitalRead(alarm_trigger);

  if(val_alarm_trigger==HIGH)
{                            //SOS!!!!!!! MORSE CODE!!!!!!!
  digitalWrite(horn, HIGH);   // set the horn on
  digitalWrite(light, HIGH);   
  delay(1000);                  // wait for a second
  digitalWrite(horn, HIGH);   // set the horn on
  digitalWrite(light, HIGH);
  delay(300);                  // wait for a second
  digitalWrite(horn, LOW);    // set the horn off
  digitalWrite(light, LOW);
  delay(1000);                 // wait for a second
  digitalWrite(horn, HIGH);   // set the horn on
  digitalWrite(light, HIGH);
  delay(300);                  // wait for a second
  digitalWrite(horn, LOW);    // set the horn off
  digitalWrite(light, LOW);
  delay(2000);                  // wait for a second
//S 
 
  digitalWrite(horn, HIGH);   // set the horn on
  digitalWrite(light, HIGH);
  delay(1000);                  // wait for a second
  digitalWrite(horn, LOW);    // set the horn off
  digitalWrite(light, LOW);
  delay(1000);                  // wait for a second
  digitalWrite(horn, HIGH);   // set the horn on
  digitalWrite(light, HIGH);
  delay(1000);                  // wait for a second
  digitalWrite(horn, LOW);    // set the horn off
  digitalWrite(light, LOW);
  delay(1000);                  // wait for a second
  digitalWrite(horn, HIGH);   // set the horn on
  digitalWrite(light, HIGH);
  delay(1000);                  // wait for a second
  digitalWrite(horn, LOW);    // set the horn off
  digitalWrite(light, LOW);
  delay(2000);                  // wait for a second
//O
  digitalWrite(horn, HIGH);   // set the horn on
  digitalWrite(light, HIGH);
  delay(300);                  // wait for a second
  digitalWrite(horn, LOW);    // set the horn off
  digitalWrite(light, LOW);
  delay(1000);                  // wait for a second
  digitalWrite(horn, HIGH);   // set the horn on
  digitalWrite(light, HIGH);
  delay(300);                  // wait for a second
  digitalWrite(horn, LOW);    // set the horn off
  digitalWrite(light, LOW);
  delay(1000);                  // wait for a second
  digitalWrite(horn, HIGH);   // set the horn on
  digitalWrite(light, HIGH);
  delay(300);                  // wait for a second
  digitalWrite(horn, LOW);    // set the horn off
  digitalWrite(light, LOW);
  delay(2000);                  // wait for a second
//S 
}
///////////////// [TRIGGER-->HORN/LIGHT] ////////////////
///////////////Is this part Okay? isn't it too loud?////////////



///////////////// [IGNITION CHECK] ////////////////
  val_ignition_check=digitalRead(ignition_check);
      
  if(val_ignition_check==HIGH)
{     
  digitalWrite(to_ignition_check,HIGH);
}
  if(val_ignition_check==LOW)
{     
  digitalWrite(to_ignition_check,LOW);
}
///////////////// [IGNITION CHECK] ////////////////




///////////////// [DOOR CHECK] ////////////////
  val_door_check=digitalRead(door_check);

  if(val_door_check==HIGH)
{     
  digitalWrite(to_door_check,HIGH);
}
  if(val_door_check==LOW)
{     
  digitalWrite(to_door_check,LOW);
}
///////////////// [DOOR CHECK] ////////////////
      
      
      
}

///////////////////////////////////////////////////////////////////////////
