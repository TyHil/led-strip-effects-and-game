/*
  Firework library implementation for Fireworks
  Written by Tyler Gordon Hill
*/
#include "firework.h"



/* Spark */

Spark::Spark() {}

void Spark::reset(uint8_t power) {
  offScreen = false;
  slope = (float)random(-10 * power, 10 * power + 1) / 100.0;
}

int16_t Spark::modOrNot(int16_t x, int16_t y, bool wrap) {
  if (wrap) return x < 0 ? ((x + 1) % y) + y - 1 : x % y;
  return x;
}

int16_t Spark::_move(int16_t start, uint8_t frame, uint8_t flags, CRGB leds[]) {
  if (!(flags & FLAG_COVER) or frame < COVER_FADE)
    leds[modOrNot((int16_t)(start + slope * pow(frame - 1, 0.6)), NUM_LEDS, flags & FLAG_WRAP)] = CRGB::Black;
  int16_t pos = modOrNot((int16_t)(start + slope * pow(frame, 0.6)), NUM_LEDS, flags & FLAG_WRAP);
  if (pos >= NUM_LEDS or pos < 0) {
    offScreen = true;
  }
  return pos;
}

void Spark::move(int16_t start, uint8_t frame, CRGB::HTMLColorCode color, uint8_t flags, CRGB leds[]) {
  if (offScreen) return;
  int16_t pos = _move(start, frame, flags, leds);
  if (offScreen) return;
  leds[pos] = color;
}

void Spark::move(int16_t start, uint8_t frame, CRGB * color, uint8_t flags, uint8_t maxFrames, CRGB leds[]) {
  if (offScreen) return;
  int16_t pos = _move(start, frame, flags, leds);
  if (offScreen) return;
  if ((flags & FLAG_FADE) and frame >= COVER_FADE) {
    float fadeAmount = ((float)(frame - COVER_FADE + 1) / (float)(maxFrames - COVER_FADE)) * -1.0 + 1;
    leds[pos]
      = CRGB((uint8_t)(color->r * fadeAmount), (uint8_t)(color->g * fadeAmount), (uint8_t)(color->b * fadeAmount));
  } else {
    leds[pos] = *color;
  }
}



/* Firework */

Firework::Firework(int16_t setPos, CRGB::HTMLColorCode setColor, uint8_t setPower, bool setCover, bool setWrap, uint8_t setMaxFrames) {
  for (uint8_t i = 0; i < NUM_SPARKS; i++) sparks[i] = Spark();
  reset(setPos, setColor, setPower, setCover, setWrap, setMaxFrames);
}

Firework::Firework(int16_t setPos, CRGB * setColor, uint8_t setPower, bool setCover, bool setWrap, bool setFade, uint8_t setMaxFrames) {
  for (uint8_t i = 0; i < NUM_SPARKS; i++) sparks[i] = Spark();
  reset(setPos, setColor, setPower, setCover, setWrap, setFade, setMaxFrames);
}

void Firework::reset(int16_t setPos, uint8_t setPower, uint8_t setMaxFrames) {
  pos = setPos;
  power = setPower;
  frame = 0;
  maxFrames = setMaxFrames;
  for (uint8_t i = 0; i < NUM_SPARKS; i++) sparks[i].reset(power);
}

void Firework::reset(int16_t setPos, CRGB::HTMLColorCode setColor, uint8_t setPower, bool setCover, bool setWrap, uint8_t setMaxFrames) {
  color.codeColor = setColor;
  colorType = HTMLColorCode;
  flags = 0;
  if (setCover) flags |= FLAG_COVER;
  if (setWrap) flags |= FLAG_WRAP;
  reset(setPos, setPower, setMaxFrames);
}

void Firework::reset(int16_t setPos, CRGB * setColor, uint8_t setPower, bool setCover, bool setWrap, bool setFade, uint8_t setMaxFrames) {
  color.constructorColor = setColor;
  colorType = ObjectPointer;
  flags = 0;
  if (setCover) flags |= FLAG_COVER;
  if (setWrap) flags |= FLAG_WRAP;
  if (setFade) flags |= FLAG_FADE;
  reset(setPos, setPower, setMaxFrames);
}

bool Firework::move(CRGB leds[]) {
  uint8_t sparksOffScreen = 0;
  for (uint8_t i = 0; i < NUM_SPARKS; i++)
    if (colorType == ObjectPointer)
      sparks[i].move(pos, frame, color.constructorColor, flags, maxFrames, leds);
    else sparks[i].move(pos, frame, color.codeColor, flags, leds);
  frame++;
  return frame >= maxFrames;
}

void Firework::run(CRGB leds[]) {
  for (frame = 0; frame < maxFrames;) {
    move(leds);
    FastLED.show();
    delay(20);
  }
}
