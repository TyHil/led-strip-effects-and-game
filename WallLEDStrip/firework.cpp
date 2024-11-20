/*
  Firework library implementation for Fireworks
  Written by Tyler Gordon Hill
*/
#include "firework.h"



/* Spark */

Spark::Spark() {}

void Spark::reset(uint8_t power) {
  offScreen = 0;
  slope = (float)random(-10 * power, 10 * power + 1) / 100.0;
}

int16_t Spark::modOrNot(int16_t x, int16_t y, bool wrap) {
  if (wrap) return x < 0 ? ((x + 1) % y) + y - 1 : x % y;
  return x;
}

void Spark::move(int16_t start, int16_t frame, CRGB::HTMLColorCode color, bool cover, bool fade, bool wrap, CRGB leds[]) {
  if (offScreen) return;
  if (!cover or frame < COVER_FADE)
    leds[modOrNot((int16_t)(start + slope * pow(frame - 1, 0.6)), NUM_LEDS, wrap)] = CRGB::Black;
  int16_t pos = modOrNot((int16_t)(start + slope * pow(frame, 0.6)), NUM_LEDS, wrap);
  if (pos >= NUM_LEDS or pos < 0) {
    offScreen = 1;
    return;
  }
  leds[pos] = color;
}

void Spark::move(int16_t start, int16_t frame, CRGB * color, bool cover, bool fade, bool wrap, CRGB leds[]) {
  if (offScreen) return;
  if (!cover or frame < COVER_FADE)
    leds[modOrNot((int16_t)(start + slope * pow(frame - 1, 0.6)), NUM_LEDS, wrap)] = CRGB::Black;
  int16_t pos = modOrNot((int16_t)(start + slope * pow(frame, 0.6)), NUM_LEDS, wrap);
  if (pos >= NUM_LEDS or pos < 0) {
    offScreen = 1;
    return;
  }
  if (fade and frame >= COVER_FADE) {
    float fadeAmount = ((float)(frame - COVER_FADE + 1) / (float)(MAX_FRAMES - COVER_FADE)) * -1.0 + 1;
    leds[pos]
      = CRGB((uint8_t)(color->r * fadeAmount), (uint8_t)(color->g * fadeAmount), (uint8_t)(color->b * fadeAmount));
  } else {
    leds[pos] = *color;
  }
}



/* Firework */

Firework::Firework(int16_t setPos, CRGB::HTMLColorCode setColor, uint8_t setPower, bool setCover, bool setWrap) {
  codeColor = setColor;
  colorType = HTMLColorCode;
  cover = setCover;
  wrap = setWrap;
  fade = false;
  for (uint8_t i = 0; i < NUM_SPARKS; i++) sparks[i] = Spark();
  reset(setPos, setPower);
}

Firework::Firework(int16_t setPos, CRGB * setColor, uint8_t setPower, bool setCover, bool setWrap, bool setFade) {
  constructorColor = setColor;
  colorType = ObjectPointer;
  cover = setCover;
  wrap = setWrap;
  fade = setFade;
  for (uint8_t i = 0; i < NUM_SPARKS; i++) sparks[i] = Spark();
  reset(setPos, setPower);
}

void Firework::reset(int16_t setPos, uint8_t setPower) {
  pos = setPos;
  power = setPower;
  frame = 0;
  for (uint8_t i = 0; i < NUM_SPARKS; i++) sparks[i].reset(power);
}

void Firework::reset(int16_t setPos, CRGB::HTMLColorCode setColor, uint8_t setPower, bool setCover, bool setWrap) {
  codeColor = setColor;
  colorType = HTMLColorCode;
  cover = setCover;
  wrap = setWrap;
  fade = false;
  reset(setPos, setPower);
}

void Firework::reset(int16_t setPos, CRGB * setColor, uint8_t setPower, bool setCover, bool setWrap, bool setFade) {
  constructorColor = setColor;
  colorType = ObjectPointer;
  cover = setCover;
  wrap = setWrap;
  fade = setFade;
  reset(setPos, setPower);
}

bool Firework::move(CRGB leds[]) {
  uint8_t sparksOffScreen = 0;
  for (uint8_t i = 0; i < NUM_SPARKS; i++)
    if (colorType == ObjectPointer)
      sparks[i].move(pos, frame, constructorColor, cover, fade, wrap, leds);
    else sparks[i].move(pos, frame, codeColor, cover, fade, wrap, leds);
  frame++;
  return frame >= MAX_FRAMES;
}

void Firework::run(CRGB leds[]) {
  for (frame = 0; frame < MAX_FRAMES; frame++) {
    for (int i = 0; i < NUM_SPARKS; i++)
      if (colorType == ObjectPointer)
        sparks[i].move(pos, frame, constructorColor, cover, fade, wrap, leds);
      else sparks[i].move(pos, frame, codeColor, cover, fade, wrap, leds);
    FastLED.show();
    delay(20);
  }
}
