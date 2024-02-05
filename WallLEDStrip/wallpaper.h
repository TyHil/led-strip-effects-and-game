/*
  Wallpaper library header for Wall LED Strip
  Written by Tyler Gordon Hill
*/
#include <FastLED.h>
#define NumLeds 300

#include "firework.h"

#define RgbStartLed 146 //0 to (NumLeds - 1)
#define RgbEndLed 299 //0 to (NumLeds - 1)

enum Mode { white, rgb, strobe, red, green, blue, color, fireworks, NUM_MODES };

class Wallpaper {
  public:
    Mode mode;
    uint8_t brightness;
    uint8_t blueLight;
    uint8_t color; //tracking for rgb and strobe progress
    uint32_t timeInput, timeRgb, timeStrobe, timeFirework;
    CRGB *chosenColor;
    Firework firework;
    Wallpaper(uint8_t setBrightness);
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
