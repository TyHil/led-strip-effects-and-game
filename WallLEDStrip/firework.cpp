/*
  Firework library implementation for Fireworks
  Written by Tyler Gordon Hill
*/
#include "firework.h"



/* Spark */

Spark::Spark() {}

void Spark::reset() {
  offScreen = 0;
  slope = (float)random(-2000, 2001) / 100;
}

int16_t Spark::modOrNot(int16_t x, int16_t y, bool wrap) {
  if (wrap) {
    return x < 0 ? ((x + 1) % y) + y - 1 : x % y;
  }
  return x;
}

void Spark::move(int16_t start, int16_t frame, CRGB::HTMLColorCode color, bool cover, bool fade, bool wrap, CRGB leds[]) {
  if (offScreen) {
    return;
  }
  if (!cover or frame <= CoverFade) {
    leds[modOrNot((int16_t)(start + slope*pow(frame - 1, 0.6)), NumLeds, wrap)] = CRGB::Black;
  }
  int16_t pos = modOrNot((int16_t)(start + slope*pow(frame, 0.6)), NumLeds, wrap);
  if (pos >= NumLeds or pos < 0) {
    offScreen = 1;
    return;
  }
  leds[pos] = color;
}

void Spark::move(int16_t start, int16_t frame, CRGB *color, bool cover, bool fade, bool wrap, CRGB leds[]) {
  if (offScreen) {
    return;
  }
  if (!cover or frame <= CoverFade) {
    leds[modOrNot((int16_t)(start + slope*pow(frame - 1, 0.6)), NumLeds, wrap)] = CRGB::Black;
  }
  int16_t pos = modOrNot((int16_t)(start + slope*pow(frame, 0.6)), NumLeds, wrap);
  if (pos >= NumLeds or pos < 0) {
    offScreen = 1;
    return;
  }
  if (frame > CoverFade and fade) {
    //float fadeAmount = (frame - CoverFade) / (MaxFrames - CoverFade);
    //leds[pos] = CRGB((uint8_t) (color->r * fadeAmount), (uint8_t) (color->g * fadeAmount), (uint8_t) (color->b * fadeAmount));
    leds[pos] = *color;
  } else {
    leds[pos] = *color;
  }
}



/* Firework */

Firework::Firework(int16_t setPos, CRGB::HTMLColorCode setColor, bool setCover, bool setWrap) {
  codeColor = setColor;
  cover = setCover;
  wrap = setWrap;
  fade = false;
  color = 0;
  for (uint8_t i = 0; i < NumSparks; i++) {
    sparks[i] = Spark();
  }
  reset(setPos);
}

Firework::Firework(int16_t setPos, CRGB *setColor, bool setCover, bool setWrap, bool setFade) {
  constructorColor = setColor;
  cover = setCover;
  wrap = setWrap;
  fade = setFade;
  color = 1;
  for (uint8_t i = 0; i < NumSparks; i++) {
    sparks[i] = Spark();
  }
  reset(setPos);
}

void Firework::reset(int16_t setPos) {
  pos = setPos;
  for (uint8_t i = 0; i < NumSparks; i++) {
    sparks[i].reset();
  }
}

bool Firework::move(CRGB leds[]) {
  uint8_t sparksOffScreen = 0;
  for (uint8_t i = 0; i < NumSparks; i++) {
    if (color) {
      sparks[i].move(pos, frame, constructorColor, cover, fade, wrap, leds);
    } else {
      sparks[i].move(pos, frame, codeColor, cover, fade, wrap, leds);
    }
  }
  frame++;
  return frame >= MaxFrames;
}

void Firework::run(CRGB leds[]) {
  for (frame = 0; frame < MaxFrames; frame++) {
    for (int i = 0; i < NumSparks; i++) {
      if (color) {
        sparks[i].move(pos, frame, constructorColor, cover, fade, wrap, leds);
      } else {
        sparks[i].move(pos, frame, codeColor, cover, fade, wrap, leds);
      }
    }
    FastLED.show();
    delay(20);
  }
}
