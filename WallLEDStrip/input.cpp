/*
  Input library implementation for Wall LED Strip
  Written by Tyler Gordon Hill
*/
#include "input.h"
#include <Arduino.h>



/* Input */

Input::Input() {
  pinMode(SW_PIN, INPUT);
  digitalWrite(SW_PIN, HIGH);
}

bool Input::up(bool sensitive) {
  return analogRead(X_PIN) < (sensitive ? 341 : 100);
}

bool Input::down(bool sensitive) {
  return analogRead(X_PIN) > (sensitive ? 682 : 932);
}

bool Input::left(bool sensitive) {
  return analogRead(Y_PIN) > (sensitive ? 682 : 932);
}

bool Input::right(bool sensitive) {
  return analogRead(Y_PIN) < (sensitive ? 341 : 100);
}

bool Input::click() {
  return digitalRead(SW_PIN) == 0;
}
