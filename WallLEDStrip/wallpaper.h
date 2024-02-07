/*
  Wallpaper library header for Wall LED Strip
  Written by Tyler Gordon Hill
*/
#include <FastLED.h>
#define NUM_LEDS 300

#include "firework.h"

#define RGB_START_LED 146 //0 to (NUM_LEDS - 1)
#define RGB_END_LED 299 //0 to (NUM_LEDS - 1)

enum Mode { white, rgb, strobe, red, green, blue, chosen, fireworks, NUM_MODES };

class Wallpaper {
  public:
    Mode mode;
    uint8_t brightness;
    uint8_t blueLight;
    uint8_t color; //tracking for rgb and strobe progress
    uint32_t timeInput, timeRgb, timeStrobe, timeFirework;
    CRGB *chosenColor;
    Firework *firework;
    Wallpaper(uint8_t setBrightness, Firework *setFirework);
    ~Wallpaper();
    void up(bool inGame, CRGB leds[]);
    void down(bool inGame, CRGB leds[]);
    void left(CRGB leds[]);
    void right(CRGB leds[]);
    void setBrightness(bool inGame);
    void fillDisplay(bool show, CRGB leds[]);
    void rgbEffect(bool show, CRGB leds[]);
    void strobeEffect(bool show, CRGB leds[]);
    void fireworkEffect(bool show, CRGB leds[]);
    void effects(bool show, bool bypass, CRGB leds[]);
    void display(bool show, bool bypass, CRGB leds[]);
    void run(bool _up, bool _down, bool _left, bool _right, bool resuming, CRGB leds[]);
};
