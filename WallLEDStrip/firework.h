/*
  Firework library header for Fireworks
  Written by Tyler Gordon Hill
*/
#pragma once

#include <FastLED.h>
#define NUM_LEDS 300

#define NUM_SPARKS 50

#define FLAG_COVER 0x01
#define FLAG_WRAP 0x02
#define FLAG_FADE 0x04

class Spark {
public:
  float slope; //position, velocity
  bool offScreen = false;
  Spark();
  int16_t modOrNot(int16_t x, int16_t y, bool wrap);
  void reset(uint8_t power);
  int16_t _move(int16_t start, uint8_t frame, uint8_t flags, uint8_t maxFrames, CRGB leds[]);
  void move(int16_t start, uint8_t frame, CRGB::HTMLColorCode color, uint8_t flags, uint8_t maxFrames, CRGB leds[]);
  void move(int16_t start, uint8_t frame, CRGB * color, uint8_t flags, uint8_t maxFrames, CRGB leds[]);
};

union Color {
  CRGB::HTMLColorCode codeColor;
  CRGB * constructorColor;
};

enum ColorType : bool {
  HTMLColorCode = true,
  ObjectPointer = false,
};

class Firework {
public:
  int16_t pos;
  uint8_t frame;
  uint8_t maxFrames;
  Color color;
  ;
  ColorType colorType;
  Spark sparks[NUM_SPARKS];
  uint8_t flags;
  uint8_t power;
  Firework(int16_t setPos, CRGB::HTMLColorCode setColor, uint8_t setPower, bool setCover, bool setWrap, uint8_t setMaxFrames);
  Firework(int16_t setPos, CRGB * setColor, uint8_t setPower, bool setCover, bool setWrap, bool setFade, uint8_t setMaxFrames);
  void reset(int16_t setPos, uint8_t setPower, uint8_t setMaxFrames);
  void reset(int16_t setPos, CRGB::HTMLColorCode setColor, uint8_t setPower, bool setCover, bool setWrap, uint8_t setMaxFrames);
  void reset(int16_t setPos, CRGB * setColor, uint8_t setPower, bool setCover, bool setWrap, bool setFade, uint8_t setMaxFrames);
  bool move(CRGB leds[]);
  void run(CRGB leds[]);
};
