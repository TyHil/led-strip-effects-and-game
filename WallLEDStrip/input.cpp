/*
  Input library implementation for Wall LED Strip
  Written by Tyler Gordon Hill
*/
#include <Arduino.h>
#include "input.h"



/* Input */

Input::Input() {
  pinMode(SWPin, INPUT);
  digitalWrite(SWPin, HIGH);
}

bool Input::up(bool sensitive) {
  return analogRead(XPin) < (sensitive ? 341 : 100);
}

bool Input::down(bool sensitive) {
  return analogRead(XPin) > (sensitive ? 682 : 932);
}

bool Input::left(bool sensitive) {
  return analogRead(YPin) > (sensitive ? 682 : 932);
}

bool Input::right(bool sensitive) {
  return analogRead(YPin) < (sensitive ? 341 : 100);
}

bool Input::click() {
  return digitalRead(SWPin) == 0;
}
