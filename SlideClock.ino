//==========================================================================
// Slide Clock - SlideClock.ino
//
// Copyright (c) 2020, Craig Colvin
//
// Licensed under Creative Commons License
// Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
//==========================================================================

#include "hardware.h"
#include "menu.h"
#include "stepper.h"
#include "clock.h"

#include "ClockCoordinates.h"     //DEBUG DEBUG

//=======================================
// C o n s t a n t s
//=======================================
#define UP_ARROW  0x41
#define DOWN_ARROW 0x42
#define RIGHT_ARROW 0x43
#define LEFT_ARROW 0x44
#define EQUALS_SIGN 0x3D
#define PLUS_SIGN 0x2B
#define MINUS_SIGN 0x2D
#define CTRL_C  0x03


typedef struct {
  int minutes;
  int tensOfMinutes;
  int hours;
  int tensOfHours;
}myDateTime;

//=======================================
// Global Variables
//=======================================
int iCurrentMotor;

//=======================================
// S e t T i m e T o U s e r I n p u t
//=======================================
void SetTimeToUserInput() {
  myDateTime dtUserInput;

  Serial.print("\nInput current time :");
  dtUserInput = InputTime();
 
  SetRTCTime((dtUserInput.tensOfHours * 10) + dtUserInput.hours, (dtUserInput.tensOfMinutes * 10) + dtUserInput.minutes);
}
//=======================================
// I n p u t T i m e 
//=======================================
myDateTime InputTime() {
  myDateTime dtInput;
  char chInputbuffer[20];
  int iHour,iMins;

 
  for(int i=0; i<4; i++) {
    while (Serial.available() == 0) {}; // (Serial.available()) {
      char inChar = Serial.read();
      if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      chInputbuffer[0] = inChar;
      chInputbuffer[1] = 0x00;
      Serial.print(chInputbuffer);

      switch (i) {
        case 0:
            dtInput.tensOfHours = atoi(chInputbuffer);
            break;
        case 1:
            dtInput.hours = atoi(chInputbuffer);
            Serial.print(":");
            break;
        case 2:
            dtInput.tensOfMinutes = atoi(chInputbuffer);
            break;
        case 3:
            dtInput.minutes = atoi(chInputbuffer);
            break;
        }
      }
    }
//  }
  return (dtInput);
}
//========================================
// K e y p a d C o n t r o l
//========================================
void KeypadControl() {
  bool bLoop;
  bool bArrowSequence;
  int in;
  long lSteps;


  // initialize variables
  bLoop = true;
  bArrowSequence = false;
  lSteps = 5000;
  iCurrentMotor = MINUTES;

  while (bLoop) {  
 
    if(Serial.available()){
      in = Serial.read();



      switch (in) {
        case CTRL_C:      //ctrl-c
            bLoop=false;
            return;
            break;
        case 0x1B:
            bArrowSequence=true;
            break;
        case RIGHT_ARROW:
            if (bArrowSequence) {
              Serial.print(F("RIGHT\n"));
              
            }
            bArrowSequence = false;
            break;
         case LEFT_ARROW:
            if (bArrowSequence) {
              Serial.print(F("LEFT\n"));
              
            }
            bArrowSequence = false;
            break;
        case DOWN_ARROW:
            if (bArrowSequence) {
              Serial.print(F("DOWN\n"));
              MoveRelative(iCurrentMotor,lSteps);
              
            }
            bArrowSequence = false;
            break;
        case UP_ARROW:
            if (bArrowSequence) {
              Serial.print(F("UP\n"));
              MoveRelative(iCurrentMotor, -lSteps);
            }
            bArrowSequence = false;
            break;
        case PLUS_SIGN:
        case EQUALS_SIGN:   //plus (or equals)
            Serial.print(F("Increase Steps\n"));
             lSteps = lSteps+100;
            bArrowSequence = false;
            break;
        case MINUS_SIGN:        // minus
            Serial.print(F("Decrease Steps\n"));
             lSteps = lSteps-100;
             bArrowSequence = false;
            break;
        case 0x5D:        // ]
            Serial.print(F("xxxxx\n"));
            
            bArrowSequence = false;
            break;
        case 0x5B:      // [
            //if not proceeded by a 1B then this is legit
            if (bArrowSequence == false) {
            Serial.print(F("Decrease Speed\n"));
            
            }
            break;
       case 0x65:      // e
            //if not proceeded by a 1B then this is legit
            if (bArrowSequence == false) {
            
            }
            break;
       case 0x7A:      // z
            //if not proceeded by a 1B then this is legit
            if (bArrowSequence == false) {
                ZeroPosition(iCurrentMotor);
            }
            break;
      case '0':
          iCurrentMotor = MINUTES;
          break;
      case '1':
          iCurrentMotor = TOM;
          break;
      case '2':
          iCurrentMotor = HOURS;
          break;
      case '3':
          iCurrentMotor = TOH;
          break;
      case '4':
          lSteps = 100;
          break;
      case '5':
          lSteps = 500;
          break;
      case '6':
          lSteps = 1000;
          break;
       case '7':
          lSteps = 2000;
          break;
       case '8':
          lSteps = 8000;
          break;
       case '9':
          lSteps = 10000;
          break;
       default:
          bArrowSequence = false;
          Serial.print(F("Input detected: '"));
          Serial.print(in);
          Serial.print(F("' 0x"));
          Serial.print(in, HEX);
          Serial.print("\n");
          break;
     }  //switch  

    //show current values
    ClearScreen();
    Serial.print("Current Position: ");
    Serial.println(GetCurrentPosition(iCurrentMotor));

    Serial.print("Steps: ");
    Serial.print(lSteps);
    Serial.print("\n");
     
    } // if
  } // while
}
//=======================================
// R u n C l o c k
//=======================================
void RunClock() {

  while(1) {
    UpdateDisplayTime();

    //Check if button has been pressed to change the time
    if(IsButtonPressed(BTN_MODE)) {
      SetTimeWithButtons();
    }
    
    delay(1000);
  }
}
//-------------------------------------
// s e t u p
//-------------------------------------
void setup()
{
    Serial.begin(9600);
    Serial.print("Slide Clock version 1.0\n");

    pinMode(BTN_MODE, INPUT_PULLUP);
    pinMode(BTN_ADVANCE, INPUT_PULLUP);
    pinMode(NOT_ENABLE_PIN, OUTPUT);


  iCurrentMotor = MINUTES;

  RTCInit();
  InitSteppers();

  //Wait for a keypress on startup. If no keypress then run the clock
  delay(5000);
  if (Serial.available() == 0) {
    FindHomePosition();
    RunClock();
  }


}
//-------------------------------------
// l o o p
//
// If we make it to loop() then we are
// processing user input from terminal
// window.
//-------------------------------------
void loop()
{
  char chMenuSelection;
  bool bEnable;

    ClearScreen();
    Serial.print("Current time : ");
    PrintCurrentTime();
    Serial.print("  Min motor position : ");
    Serial.println(GetCurrentPosition(MINUTES));
    Serial.print("  TOM motor position : ");
    Serial.println(GetCurrentPosition(TOM));
    Serial.print("Hours motor position : ");
    Serial.println(GetCurrentPosition(HOURS));
    Serial.print("  TOH motor position : ");
    Serial.println(GetCurrentPosition(TOH));

  //Display menu and get user's choice
  chMenuSelection = ShowMenu();
  switch(chMenuSelection)
    {
    case 'h' :
        FindHomePosition();
        break;
    case 'k' :
        KeypadControl();
        break;
    case 'r' :
        RunClock();
        break;
    case 't' :
        SetTimeToUserInput();
        break;
   default :
        Serial.print("selected unknown\n");
        break;
    }

  Serial.print("\n\nPress a key to continue...");
  recvOneChar();

}
