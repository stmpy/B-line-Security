/* Car interface class for B-Line Security
 * dot H file that is required and needs to be included in order to use the car
 * interface class.
 * 
 * #include <car.h>
 *
 * All variables and functions used in this class are defined below.
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

#ifndef CAR_H
#define CAR_H

#include "WProgram.h"

#define BLINK_LENGTH 2

class Car {
    public:
        Car(int *i,int *o);
        
        void unlock();
        void lock();
        void alarm();
        void clear_alarm();
        // need to return a value (HIGH, LOW)
        byte check_ignition();
        byte check_doors();
        byte check_unlock();
        byte check_lock();
        
    private:
        void morse_code(byte codes[],int duration[],int length,bool horn);
        // inputs
        int unlock_control;
        int lock_control;
        int light_control;
        int horn_control;
        int door_control;
        int ignition_control;
        // outputs
        int unlock_check;
        int lock_check;
        int door_check;
        int ignition_check;
        // random
        byte blink[BLINK_LENGTH];
        int blink_dur[BLINK_LENGTH];
};

#endif