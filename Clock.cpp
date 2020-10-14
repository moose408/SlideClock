//==========================================================================
// Slide Clock -  Clock.cpp
//
// Copyright (c) 2020, Craig Colvin
//
// Licensed under Creative Commons License
// Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
//==========================================================================

#include <Arduino.h>

#include "ClockCoordinates.h"
#include "Clock.h"
#include "RTClib.h"
#include "hardware.h"
#include "stepper.h"

RTC_PCF8523 rtcClock;
DateTime dtCurrentDisplayedTime;

const int MINS_GETDIGIT           = 0;
const int TENS_OF_MINS_GETDIGIT   = 1;
const int HOURS_GETDIGIT          = 2;
const int TENS_OF_HOURS_GETDIGIT  = 3;

const int MODE_CHANGE_MINS = 0;
const int MODE_CHANGE_TOM = 1;
const int MODE_CHANGE_HOURS = 2;
const int MODE_CHANGE_TOH = 3;
const int MODE_STANDBY = 4;

const int WIGGLE_DISTANCE = 400;    //Number of steps to wiggle up and down

//=========================================
// W i g g l e S l i d e
//
// This will move the slide up and then
// down a short distance. This is used when 
// setting the time to indicate which
// digit is selected.
//=========================================
void WiggleSlide(int iSlideID) {

    MoveRelative(iSlideID, -WIGGLE_DISTANCE);
    //delay(100);
    MoveRelative(iSlideID, WIGGLE_DISTANCE);
}
//=========================================
// I s B u t t o n P r e s s e d
//=========================================
bool IsButtonPressed(int iButtonID) {
  return !(digitalRead(iButtonID));
}
//=========================================
// I n c r e m e n t T i m e 
//=========================================
int IncrementTime(int iTimeValue, int iMaxValue) {
  int iNewValue;

  iNewValue = iTimeValue + 1;
  if (iNewValue > iMaxValue) {
    iNewValue = 0;
  }

  return (iNewValue);
}
//=========================================
// H a s A M i n u t e P a s s e d
//
// This expects both times to be in 24h
// notation
//=========================================
bool HasAMinutePassed() {
  DateTime dtNow = rtcClock.now();

  if ((dtNow.unixtime() - dtCurrentDisplayedTime.unixtime()) >= 60) 
    {
    return true;
    }
  else
    {
    return false; 
    }

}
//=========================================
// G e t D i g i t
//
// Returns a single digit out of the DateTime
//=========================================
int GetDigit(DateTime dtTime,int iDigitPos)
{
  switch(iDigitPos) 
  {
    case MINS_GETDIGIT:
      return dtTime.minute() % 10;
      break;
    case TENS_OF_MINS_GETDIGIT:
     return dtTime.minute() / 10;
     break;
    case HOURS_GETDIGIT:
      return dtTime.hour() % 10;
      break;
    case TENS_OF_HOURS_GETDIGIT:
      return dtTime.hour() / 10;
      break;  
  }
}

//=========================================
// R T C I n i t 
//=========================================
void RTCInit() {
    //------------
    // Setup RTC
    //------------
    rtcClock.begin();

 if (! rtcClock.initialized()) {
        Serial.println("RTC is NOT running!");
      }

  //Uncommenting this will initialize the clock to the
  //compile time of this code.
  //rtcClock.adjust(DateTime(__DATE__, __TIME__));

}
//=========================================
// S e t T i m e W i t h B u t t o n s
//=========================================
void SetTimeWithButtons() {
  int iMode;
  int iNewMinute, iNewTOM, iNewHour, iNewTOH;

  Serial.println("Set Time");
  
  //Initialize
  iNewMinute = GetDigit(dtCurrentDisplayedTime, MINS_GETDIGIT);
  iNewTOM = GetDigit(dtCurrentDisplayedTime, TENS_OF_MINS_GETDIGIT);
  iNewHour = GetDigit(dtCurrentDisplayedTime, HOURS_GETDIGIT);
  iNewTOH = GetDigit(dtCurrentDisplayedTime, TENS_OF_HOURS_GETDIGIT);

  iMode = MODE_CHANGE_TOH;

  //Show user which slide is active
  WiggleSlide(TOH);

  while(iMode != MODE_STANDBY) {

      //Process Mode Button
      if (IsButtonPressed(BTN_MODE)) {
        //select next state
        switch (iMode) {
          case MODE_CHANGE_MINS:
              iMode = MODE_STANDBY;
              break;
          case MODE_CHANGE_TOM:
              iMode = MODE_CHANGE_MINS;
              WiggleSlide(MINUTES);
              break;
          case MODE_CHANGE_HOURS:
              iMode = MODE_CHANGE_TOM;
              WiggleSlide(TOM);
              break;
          case MODE_CHANGE_TOH:
               iMode = MODE_CHANGE_HOURS;
               WiggleSlide(HOURS);
            break;
        }
      } // if BTN_MODE

      //Process Select Button
      if (IsButtonPressed(BTN_ADVANCE)) {
        switch (iMode) {
          case MODE_CHANGE_MINS:
              Serial.println("Changing mins");
              iNewMinute = IncrementTime(iNewMinute, 9);
              MoveAbsolute(MINUTES, MINUTES_POSITIONS[iNewMinute]);
              break;
          case MODE_CHANGE_TOM:
              Serial.println("Changing TOM");
              iNewTOM = IncrementTime(iNewTOM, 5);
              MoveAbsolute(TOM, TOM_POSITIONS[iNewTOM]);
               break;
          case MODE_CHANGE_HOURS:
              Serial.println("Changing Hours");
              iNewHour = IncrementTime(iNewHour, 9);
              MoveAbsolute(HOURS, HOURS_POSITIONS[iNewHour]);
              break;
          case MODE_CHANGE_TOH:
              Serial.println("Changing TOH");
              iNewTOH = IncrementTime(iNewTOH, 2);
              MoveAbsolute(TOH, TOH_POSITIONS[iNewTOH]);
            break;
        }


        
      } // if BTN_ADVANCE

  } // while !MODE_STANDBY

  SetRTCTime((iNewTOH * 10) + iNewHour, (iNewTOM * 10) + iNewMinute);
}
//===============================================
// S e t R T C T i m e
//===============================================
void SetRTCTime (int iHours, int iMinutes) {
  DateTime dtNewTime;

  dtNewTime = DateTime(2020,07,01,iHours,iMinutes,0);
  rtcClock.adjust(dtNewTime);

}
//===============================================
// C o n v e r t 2 4 h T i m e T o 1 2 h T i m e
//===============================================
DateTime Convert24hTimeTo12hTime(DateTime dtTime) {

      //Display time as 12h time as opposed to 24h
    if (dtTime.hour() >= 13 ) {
      return(DateTime(dtTime.year(),dtTime.month(),dtTime.day(),dtTime.hour() - 12,dtTime.minute(),dtTime.second()));
    }

    //No conversion necessary so return original time
    return dtTime;
}
//=========================================
// P r i n t T i m e
//=========================================
void PrintCurrentTime() {
    DateTime dtTime;

    dtTime = rtcClock.now(); 
    
    Serial.print(GetDigit(dtTime,TENS_OF_HOURS_GETDIGIT));
    Serial.print(GetDigit(dtTime,HOURS_GETDIGIT));
    Serial.print(":");
    Serial.print(GetDigit(dtTime,TENS_OF_MINS_GETDIGIT));
    Serial.print(GetDigit(dtTime,MINS_GETDIGIT));
    Serial.print("\n");
}//=========================================
// P r i n t T i m e
//=========================================
void PrintTime(DateTime dtTime) {

    Serial.print(GetDigit(dtTime,TENS_OF_HOURS_GETDIGIT));
    Serial.print(GetDigit(dtTime,HOURS_GETDIGIT));
    Serial.print(":");
    Serial.print(GetDigit(dtTime,TENS_OF_MINS_GETDIGIT));
    Serial.print(GetDigit(dtTime,MINS_GETDIGIT));
    Serial.print("\n");
}
//=========================================
// U p d a t e D i s p l a y T i m e
//
// This will get current time and move 
// any digits that have changed 
//=========================================
void UpdateDisplayTime() {
    DateTime dtNow, dtNewDisplayedTime;
    long lMinPos, lTOMPos, lHoursPos, lTOHPos;

    dtNow = rtcClock.now(); 

    // Check if minute has passed 
    if (! HasAMinutePassed()) {
      return;
    }

    //Display time as 12h time as opposed to 24h
    dtNewDisplayedTime = Convert24hTimeTo12hTime(dtNow);
    dtCurrentDisplayedTime = Convert24hTimeTo12hTime(dtCurrentDisplayedTime);

    //Just to clarify, at this point we have 3 DateTime variables
    //                 dtNow = current time read from RTC in 24h format
    //    dtNewDisplayedTime = current time read from RTC in 12h format
    //dtCurrentDisplayedTime = time that is currently being displayed and is going to be replaced

    Serial.print("\n\nCurrent displayed time: ");
    PrintTime(dtCurrentDisplayedTime);
    Serial.print("New time: ");
    PrintTime(dtNewDisplayedTime);

    lMinPos = MINUTES_POSITIONS[GetDigit(dtNewDisplayedTime,MINS_GETDIGIT)];
    lTOMPos = TOM_POSITIONS[GetDigit(dtNewDisplayedTime,TENS_OF_MINS_GETDIGIT)];
    lHoursPos = HOURS_POSITIONS[GetDigit(dtNewDisplayedTime,HOURS_GETDIGIT)];
    lTOHPos = TOH_POSITIONS[GetDigit(dtNewDisplayedTime,TENS_OF_HOURS_GETDIGIT)];
   
    MoveAllSteppers(lTOHPos, lHoursPos, lTOMPos, lMinPos);

    //Update current display time
    dtCurrentDisplayedTime = dtNow;

}
