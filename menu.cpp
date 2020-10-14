//==========================================================================
// Slide Clock - menu.cpp
//
// Copyright (c) 2020, Craig Colvin
//
// Licensed under Creative Commons License
// Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
//==========================================================================

#include <arduino.h>


//================================
// r e c v O n e C h a r
//================================
char recvOneChar() {
    char receivedChar;

    do {
        //delayMilliseconds(10);
    } while (Serial.available() == 0);

    if (Serial.available() > 0) {
        receivedChar = Serial.read();
        Serial.flush();
    }
    return receivedChar;
}

//================================
// C l e a r S c r e e n
//================================
void ClearScreen() {
  Serial.write(27);       // ESC command
  Serial.print("[2J");    // clear screen command
  Serial.write(27);
  Serial.print("[H");     // cursor to home command

}
//===================================
// S h o w M e n u
//
// Assumes VT100 terminal emulation
//===================================
char ShowMenu() {
    //ClearScreen();

    Serial.write(27);       // ESC command
    Serial.print("[30;47m");    // Black on white


    Serial.print("\n");
    Serial.print(F("---------Choose Command-----------\n"));
    Serial.write(27);       // ESC command
    Serial.print("[39;49m");    // normal color

    Serial.print(F("h - set home position\n"));
    Serial.print(F("k - keypad control\n"));
    Serial.print(F("r - Run Clock\n"));
    Serial.print(F("t - Set Time\n"));

    return recvOneChar();

}
