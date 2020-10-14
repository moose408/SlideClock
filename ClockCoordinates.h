//==========================================================================
// Slide Clock - ClockCoordinates.h
//
// Copyright (c) 2020, Craig Colvin
//
// Licensed under Creative Commons License
// Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
//==========================================================================

#ifndef CLOCKCOORDINATES_H
#define CLOCKCOORDINATES_H

const long MAX_TOH_STEPS = 8000;
const long MAX_HOURS_STEPS = 26500;
const long MAX_TOM_STEPS = 10000;       //DEBUG DEBUG Need to confirm this
const long MAX_MINUTES_STEPS = 26500;


extern const long MINUTES_POSITIONS[];
extern const long TOM_POSITIONS[];
extern const long HOURS_POSITIONS[];
extern const long TOH_POSITIONS[];

// Number of absolute steps from home
// where a digit resides
#define POS_0 1000
#define POS_1 3700
#define POS_2 6400
#define POS_3 9100
#define POS_4 11800
#define POS_5 14500
#define POS_6 17200
#define POS_7 19900
#define POS_8 22600
#define POS_9 25350

#endif //CLOCKCOORDINATES_H
