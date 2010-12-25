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

// OUTPUTS -- these will never change within the program
#define NUM_OF_OUTPUTS 6
#define UNLOCK_CONTROL 22  // door_unlock_R
#define LOCK_CONTROL 23    // door_lock_R
#define LIGHT_CONTROL 11   // light
#define HORN_CONTROL 10    // horn
#define DOOR_CONTROL 24    // to_door_check
#define IGNITION_CONTROL 25 // to_ignition_check

// INPUTS -- these will never change within the program
#define NUM_OF_INPUTS 4
#define UNLOCK_CHECK 2     // SW_door_unlock_R
#define LOCK_CHECK 3       // SW_door_lock_R
#define DOOR_CHECK 5       // door_check
#define IGNITION_CHECK 4   // ignition_check

// Global Variables
int outputs[NUM_OF_OUTPUTS] = { // need to be in this order
                UNLOCK_CONTROL,
                LOCK_CONTROL,
                LIGHT_CONTROL,
                HORN_CONTROL,
                DOOR_CONTROL,
                IGNITION_CONTROL};
int inputs[NUM_OF_INPUTS] = { // need to be in this order
                UNLOCK_CHECK,
                LOCK_CHECK,
                DOOR_CHECK,
                IGNITION_CHECK};
// declaration
Car car = Car(inputs,outputs);
void setup() {
 
}

void loop() {

// example call to lock doors
car.lock();
 
}