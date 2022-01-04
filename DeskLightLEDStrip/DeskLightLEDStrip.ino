/*
  Desk light WS2812B LED strip control for Arduino Nano with white light (with blue
  light adjustment), RGB rainbow effect, strobe effect, and specific color choice
  that syncs with the wall code by sending laser pulses.
  Written by Tyler Hill
  Version 3.0
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
bool power = 0, fade = 0; //power starts off, fade in from off
CRGB leds[NumLeds], chosenColor = CRGB(136, 136, 136);
IRrecv irrecv(3);
decode_results results;
uint8_t brightness = 17, mode = 10, color; //brightness, effect mode: blue light amount/rainbow/strobe mode/chosen color, rainbow and strobe color
unsigned long flashTime = 0; //strobe timing
byte * Wheel(byte WheelPos) { //Rainbow fuction from NeoPixel library strandtest example code
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
void update() { //set all to correct brightness and blue light value
  if (mode < 13 or mode > 15) FastLED.setBrightness(brightness);
  else FastLED.setBrightness(255);
  CRGB setColor;
  if (mode <= 10) setColor = CRGB(255, 255, mode * 25.5);
  else if (mode > 12 and  mode < 16) setColor = CRGB(chosenColor.r * (mode == 13), chosenColor.g * (mode == 14), chosenColor.b * (mode == 15));
  else if (mode == 16) setColor = chosenColor;
  if (mode != 11 and mode != 12) for (uint8_t i = 0; i < NumLeds; i++) leds[i] = setColor;
}
void setup() {
  //Serial.begin(9600); //Serial.println("");
  irrecv.enableIRIn();
  FastLED.addLeds<WS2812B, DataPin, GRB>(leds, NumLeds);
  pinMode(6, OUTPUT); //attached to laser for communication with another LED strip
}
void loop() {
  if (irrecv.decode(&results)) {
    if (power) {
      if (results.value == 0xE0E048B7) { //up, increase brightness
        if (mode == 13) chosenColor.r = min(chosenColor.r + 17, 255);
        else if (mode == 14) chosenColor.g = min(chosenColor.g + 17, 255);
        else if (mode == 15) chosenColor.b = min(chosenColor.b + 17, 255);
        else brightness = min(brightness + 17, 255);
      }
      else if (results.value == 0xE0E008F7) { //down, decrease brightness
        if (mode == 13) chosenColor.r = max(chosenColor.r - 17, 0);
        else if (mode == 14) chosenColor.g = max(chosenColor.g - 17, 0);
        else if (mode == 15) chosenColor.b = max(chosenColor.b - 17, 0);
        else brightness = max(brightness - 17, 17);
      }
      else if (results.value == 0xE0E0D02F) { //left, decrease mode
        mode = max(mode - 1, 0);
        if (mode < 13) FastLED.setBrightness(brightness);
      }
      else if (results.value == 0xE0E0E01F) mode = min(mode + 1, 16); //right, increase mode
      if (results.value == 0xE0E008F7 or results.value == 0xE0E048B7 or results.value == 0xE0E0D02F or results.value == 0xE0E0E01F) { //any
        update();
        if (mode <= 10 or mode > 12) FastLED.show();
      }
    }
    if (results.value == 0xE0E040BF) { //on/off
      power = !power;
      if (power) {
        update();
        fade = true;
      } else { //fade out
        for (int16_t i = (brightness - brightness / 17) * (mode < 13 or mode > 15) + 238 * (mode > 12 and mode < 16); i > 0; i -= (brightness / 17) * (mode < 13 or mode > 15) + 17 * (mode > 12 and mode < 16)) {
          FastLED.show(i);
          delay(40);
        }
        FastLED.clear();
        FastLED.show();
      }
    }
    irrecv.resume();
  }
  if (mode == 11 and power) { //rainbow effect
    byte *c;
    color = (color + 5) % 256;
    for (uint8_t i = 0; i <= 66; i++) {
      c = Wheel((((uint16_t)i * 256 / NumLeds) + color) & 255); //& works as a form of mod
      leds[i] = CRGB(*c, *(c + 1), *(c + 2));
      if (2 * 66 - i < NumLeds) leds[2 * 66 - i] = CRGB(*c, *(c + 1), *(c + 2));
    }
    if (!fade) FastLED.show();
  }
  if (mode == 12 and power and millis() - flashTime > 250) { //strobe effect
    flashTime = millis();
    if (!fade) color = (color + 1) % 3;
    if (color == 1) {
      digitalWrite(6, HIGH); //send laser pulse when green
      delay(10);
      digitalWrite(6, LOW); // turn off when blue
    }
    for (uint8_t i = 0; i < NumLeds; i++) leds[i] = CRGB(255 * (color == 0), 255 * (color == 1), 255 * (color == 2));
    if (!fade) FastLED.show();
  }
  if (fade) { //fade in
    fade = false;
    for (int16_t i = (brightness / 17) * (mode < 13 or mode > 15) + 17 * (mode > 12 and mode < 16); i <= brightness * (mode < 13 or mode > 15) + 255 * (mode > 12 and mode < 16); i += (brightness / 17) * (mode < 13 or mode > 15) + 17 * (mode > 12 and mode < 16)) {
      FastLED.show(i);
      delay(40);
    }
  }
  delay(150);
}
