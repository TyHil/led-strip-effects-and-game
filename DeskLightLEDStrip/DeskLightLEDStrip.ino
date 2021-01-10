/*
  Desk light individually addressable RGB strip control with white light (with blue
  light adjustment), RGB rainbow effect, and strobe effect that syncs with the wall code
  by sending laser pulses.
  Written by Tyler Hill
  Version 2.0
  Remote Codes:
  E0E040BF
      E0E048B7
  E0E0D02F E0E0E01F
      E0E008F7
*/
#include <IRremote.h>
#include <FastLED.h>
#define NumLeds 131
#define DataPin 2
bool power, fade;//power starts off, fade in from off
CRGB leds[NumLeds];
IRrecv irrecv(3);
decode_results results;
uint8_t brightness = 10, blue = 10, color;//blue light amount/rainbow/strobe mode, effect color
unsigned long flashTime;//how often strobe changes
byte * Wheel(byte WheelPos) {//Rainbow fuction from NeoPixel library strandtest example code
  static byte c[3];
  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }
  return c;
}
void update() {//set all to correct brightness and blue light value
  FastLED.setBrightness(brightness);
  if (blue <= 10) for (uint8_t i = 0; i < NumLeds; i++) leds[i] = CRGB(255, 255, blue * 25.5);
}
void setup() {
  //Serial.begin(9600);//Serial.println("");
  irrecv.enableIRIn();
  FastLED.addLeds<WS2812B, DataPin, GRB>(leds, NumLeds);
  pinMode(6, OUTPUT);//attached to laser for communication with another LED strip
}
void loop() {
  if (irrecv.decode(&results)) {
    if (power) {
      if (results.value == 0xE0E048B7) brightness = min(brightness + 10, 100);//up, increase brightness
      else if (results.value == 0xE0E008F7) brightness = max(brightness - 10, 10);//down, decrease brightness
      else if (results.value == 0xE0E0D02F) blue = max(blue - 1, 0);//left, decrease blue light
      else if (results.value == 0xE0E0E01F) blue = min(blue + 1, 12);//right, increase blue light, other lighting effects
      if (results.value == 0xE0E008F7 or results.value == 0xE0E048B7 or results.value == 0xE0E0D02F or results.value == 0xE0E0E01F) {//any
        update();
        if (blue <= 10) FastLED.show();
      }
    }
    if (results.value == 0xE0E040BF) {//on/off
      power = !power;
      if (power) {
        update();
        fade = true;
      } else {
        for (int8_t i = brightness - 1; i > 0; i -= brightness / 10) {
          FastLED.show(i);
          delay(40);
        }
        FastLED.clear();
        FastLED.show();
      }
    }
    irrecv.resume();
  }
  if (blue == 11 and power) {//rainbow effect
    byte *c;
    color = (color + 5) % 256;
    for (uint8_t i = 0; i <= 66; i++) {
      c = Wheel((((uint16_t)i * 256 / NumLeds) + color) & 255);//& works as a form of mod
      leds[i] = CRGB(*c, *(c + 1), *(c + 2));
      if (2 * 66 - i < NumLeds) leds [2 * 66 - i] = CRGB(*c, *(c + 1), *(c + 2));
    }
    if (!fade) FastLED.show();
    }
  if (blue == 12 and power and millis() - flashTime > 300) {//strobe effect
    flashTime = millis();
    if (!fade) color = (color + 1) % 3;
    if (color == 1) {
      digitalWrite(6, HIGH);//send laser pulse when green
      delay(10);
      digitalWrite(6, LOW);// turn off when blue
    }
    for (uint8_t i = 0; i < NumLeds; i++) leds[i] = CRGB(255 * (color == 0), 255 * (color == 1), 255 * (color == 2));
    if (!fade) FastLED.show();
    }
  if (fade) {
    fade = false;
    for (int8_t i = 0; i <= brightness; i += brightness / 10) {
      FastLED.show(i);
      delay(40);
    }
  }
  delay(150);
}
