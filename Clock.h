//==========================================================================
// Slide Clock -  Clock.h
//
// Copyright (c) 2020, Craig Colvin
//
// Licensed under Creative Commons License
// Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
//==========================================================================

#include "ClockCoordinates.h"

#ifndef CLOCK_H
#define CLOCK_H


void UpdateDisplayTime();
void RTCInit();
void SetRTCTime (int iHours, int iMinutes);

void PrintCurrentTime();

void SetTimeWithButtons();
bool IsButtonPressed(int iButtonID);

#endif //CLOCK_H
