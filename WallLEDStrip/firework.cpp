/*
  Firework library implementation for Fireworks
  Written by Tyler Gordon Hill
*/
#include "firework.h"



/* Helper */

int16_t mod(int16_t x, int16_t y) {
  return x < 0 ? ((x + 1) % y) + y - 1 : x % y;
}



/* Spark */

Spark::Spark() {
  randomColor = true;
}

Spark::Spark(CRGB color) {
  randomColor = false;
  this->color = color;
}

void Spark::reset() {
  offScreen = 0;
  if (randomColor) {
    color = CRGB(random(0,255), random(0,255), random(0,255));
  }
  slope = (float)random(-2000, 2001) / 100;
}

int16_t Spark::modOrNot(int16_t x, int16_t y, bool wrap) {
  if (wrap) {
    return mod(x, y);
  }
  return x;
}

void Spark::move(int16_t start, int16_t frame, bool cover, bool fade, bool wrap, CRGB leds[]) {
  if (offScreen) {
    return;
  }
  if (cover and frame > CoverFade) {
    leds[modOrNot((int16_t)(start + slope*pow(frame - 1, 0.6)), NumLeds, wrap)] = CRGB::Black;
  }
  int16_t pos = modOrNot((int16_t)(start + slope*pow(frame, 0.6)), NumLeds, wrap);
  if (pos >= NumLeds or pos < 0) {
    offScreen = 1;
    return;
  }
  if (frame > CoverFade and fade) {
    float fadeAmount = (frame - CoverFade) / (MaxFrames - CoverFade);
    leds[pos] = CRGB((uint8_t) (color.r * fadeAmount), (uint8_t) (color.g * fadeAmount), (uint8_t) (color.b * fadeAmount));
  } else {
    leds[pos] = color;
  }
}



/* Firework */

Firework::Firework(int16_t pos, bool cover, bool fade, bool wrap, bool randomColors = true) {
  this->randomColors = randomColors;
  this->cover = cover;
  this->fade = fade;
  this->wrap = wrap;
  for (uint8_t i = 0; i < NumSparks; i++) {
    sparks[i] = Spark();
  }
  reset(pos);
}

Firework::Firework(int16_t pos, bool cover, bool fade, bool wrap, CRGB color) {
  this->color = color;
  Firework(pos, cover, fade, wrap, false);
}

void Firework::reset(int16_t pos) {
  this->pos = pos;
  for (uint8_t i = 0; i < NumSparks; i++) {
    sparks[i].reset();
  }
}

void Firework::move(CRGB leds[]) {
  uint8_t sparksOffScreen = 0;
  for (uint8_t i = 0; i < NumSparks; i++) {
    sparks[i].move(pos, frame, cover, fade, wrap, leds);
  }
}

void Firework::run(CRGB leds[]) {
  for (frame = 0; frame < MaxFrames; frame++) {
    for (int i = 0; i < NumSparks; i++) {
      sparks[i].move(pos, frame, cover, fade, wrap, leds);
    }
    FastLED.show();
    delay(20);
  }
}
