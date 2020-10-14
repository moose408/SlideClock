//==========================================================================
// Slide Clock - stepper.h
//
// Copyright (c) 2020, Craig Colvin
//
// Licensed under Creative Commons License
// Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
//==========================================================================

#ifndef STEPPER_H
#define STEPPER_H

const int MINUTES = 0;
const int TOM = 1;
const int HOURS = 2;
const int TOH = 3;

// Motor parameters
const int FULL_STEPS_PER_REVOLUTION = 64;
const int MAXSPEED = 1600; 
const int ACCELERATION = 1600;

void InitSteppers();
void MoveRelative(int iMotor, long lSteps);
void MoveAbsolute(int iMotor, long lSteps);
long GetCurrentPosition(int iMotor);
void ZeroPosition(int iMotor);
void MoveAllSteppers(long lTOHSteps, long lHoursSteps, long lTOMSteps, long lMinutesSteps);
void FindHomePosition();

#endif //STEPPER_H
