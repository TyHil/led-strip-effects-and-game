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
    Spark();
    int16_t modOrNot(int16_t x, int16_t y, bool wrap);
    void reset(uint8_t power);
    void move(int16_t start, int16_t frame, CRGB::HTMLColorCode color, bool cover, bool fade, bool wrap, CRGB leds[]);
    void move(int16_t start, int16_t frame, CRGB *color, bool cover, bool fade, bool wrap, CRGB leds[]);
};

enum ColorType : bool {
  HTMLColorCode = true,
  ObjectPointer = false,
};

class Firework {
  public:
    int16_t pos;
    int16_t frame;
    bool randomColors;
    CRGB::HTMLColorCode codeColor;
    CRGB *constructorColor;
    ColorType colorType;
    Spark sparks[NumSparks];
    bool cover, wrap, fade;
    uint8_t power;
    Firework(int16_t setPos, CRGB::HTMLColorCode setColor, uint8_t setPower, bool setCover, bool setWrap);
    Firework(int16_t setPos, CRGB *setColor, uint8_t setPower, bool setCover, bool setWrap, bool setFade);
    void reset(int16_t setPos, uint8_t setPower);
    bool move(CRGB leds[]);
    void run(CRGB leds[]);
};
