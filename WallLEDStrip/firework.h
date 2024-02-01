/*
  Firework library header for Fireworks
  Written by Tyler Gordon Hill
*/
#include <FastLED.h>
#define NumLeds 300

#define NumSparks 50
#define MaxFrames 150
#define CoverFade 80

class Spark {
   public:
    float slope; //position, velocity
    bool randomColor;
    bool offScreen = 0;
    CRGB *color;
    Spark();
    int16_t modOrNot(int16_t x, int16_t y, bool wrap);
    void reset();
    void move(int16_t start, int16_t frame, CRGB *color, bool cover, bool fade, bool wrap, CRGB leds[]);
};

class Firework {
  public:
    int16_t pos;
    int16_t frame;
    bool randomColors;
    CRGB *color;
    Spark sparks[NumSparks];
    bool cover, fade, wrap;
    Firework(int16_t setPos, bool setCover, bool setFade, bool setWrap, CRGB *setColor);
    void reset(int16_t setPos);
    bool move(CRGB leds[]);
    void run(CRGB leds[]);
};
