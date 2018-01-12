// * Name: MultiTouchMultiResponse.ino
// * Project: InkLab
// * Author: David Haylock
// * Creation Date: 11/01/2018
// * Copyright: (c) 2018 by Watershed Arts Trust Ltd.

#include <MPR121.h>
#include <Wire.h>
#include <Servo.h>

#define NUMBER_OF_ELECTRODES 12

// These are the variables you can change!
#define TOUCH_THRESHOLD 40
#define RELEASE_THRESHOLD 20

#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

// Objects
Servo servo;
bool servoLatch = false;

//----------------------------------------------------
// *
// * Setup Section 
// * 
//----------------------------------------------------
void setup() {

    Serial.println("=============================");
    Serial.println("MultiTouchMultiResponse");
    Serial.println("=============================");

    // Open a serial connection so we can see debug messages
    Serial.begin(9600);

    // Not needed unless on an Arduino Leonardo
    while(!Serial);

    // As it Sounds 
    setupTouchBoard();

    servo.attach(9);
}

//----------------------------------------------------
// *
// * Setup the Touch Board
// * 
//----------------------------------------------------
void setupTouchBoard() {
    // Setup Wire
    Wire.begin();

    // Check the Connection to the Bare Connductive Board
    if(!MPR121.begin(0x5C)) {
        Serial.println("Error: Setting Up MPR121");
        switch(MPR121.getError()){
            case NO_ERROR:
                Serial.println("no error");
                break;  
            case ADDRESS_UNKNOWN:
                Serial.println("incorrect address");
                break;
            case READBACK_FAIL:
                Serial.println("readback failure");
                break;
            case OVERCURRENT_FLAG:
                Serial.println("overcurrent on REXT pin");
                break;      
            case OUT_OF_RANGE:
                Serial.println("electrode out of range");
                break;
            case NOT_INITED:
                Serial.println("not initialised");
                break;
            default:
                Serial.println("unknown error");
                break;      
        }
        while(1);
    }

    // Pin 4 is the MPR121 interrupt on the Bare Touch Board
    MPR121.setInterruptPin(4);

    // this is the touch threshold - setting it low makes it more like a proximity trigger
    MPR121.setTouchThreshold(TOUCH_THRESHOLD);

    // this is the release threshold - must ALWAYS be smaller than the touch threshold
    MPR121.setReleaseThreshold(RELEASE_THRESHOLD);  

    // initial data update
    MPR121.updateTouchData();
}

//----------------------------------------------------
// *
// * Main Loop
// * 
//----------------------------------------------------
void loop() {

    // Check for change on the sensors
    if(MPR121.touchStatusChanged()) {
        MPR121.updateTouchData();

        // We've grouped the pins into three
        // Each pin has three functions
        // 1: SWITCH
        // 2: TOGGLE
        // 3: LOOP
        // Group One: SERVO : SWITCH
        
        if(MPR121.isNewTouch(0)) {
            servo.write(SERVO_MAX_ANGLE);
            Serial.println("SERVO SWITCH ON");
        }
        else if(MPR121.isNewRelease(0)) {
            servo.write(SERVO_MIN_ANGLE);
            Serial.println("SERVO SWITCH OFF");
        }

        // Group One: SERVO : TOGGLE
        if(MPR121.isNewTouch(1)) {
            if(!servoLatch) {
                servo.write(SERVO_MAX_ANGLE);
                Serial.println("SERVO TOGGLE ON");
                servoLatch = true;
            }
            else if(servoLatch) {
                servo.write(SERVO_MIN_ANGLE);
                Serial.println("SERVO TOGGLE OFF");
                servoLatch = false;
            }
        }

        // Group One: SERVO : LOOP
        if(MPR121.isNewTouch(2)) {
            Serial.println("SERVO LOOP STARTED");
            for(int angle = SERVO_MIN_ANGLE; angle < SERVO_MAX_ANGLE; angle++) {
                servo.write(angle);
                delay(5);
            }
            delay(100);

            for(int angle = SERVO_MAX_ANGLE; angle > SERVO_MIN_ANGLE; angle--) {
                servo.write(angle);
                delay(5);
            }
            Serial.println("SERVO LOOP FINISHED");
        }
    }
}