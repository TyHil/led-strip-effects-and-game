/*
  Wall mounted WS2812B LED strip control for Arduino Nano with white light (with blue
  light adjustment), RGB rainbow effect, strobe effect, and specific color choice that
  syncs with the desk light code through a laser sensor. There is also a joystick
  controlled game in which the player, a green pixel that can shoot a couple pixels
  ahead of itself, moves from the beginning of the strip to the end through an ever
  growing field of enemies as the game progresses.
  Written by Tyler Hill
  Version 3.0
  Note: Neopixels doesn't have dynamic brightness
*/
#include "firework.h"
#include "game.h"
#define DataPin 2
#define YPin A3
#define XPin A2
#define SWPin 4
#define RgbStartLed 146 //0 to (NumLeds - 1)
#define RgbEndLed 299 //0 to (NumLeds - 1)
CRGB leds[NumLeds];

/*Wallpaper Vars*/

int64_t timeWallpaper = -30000; //inactive time before wallpaper is resumed
uint8_t brightness = 51, mode = 11, color; //brightness, effect mode: blue light amount/rainbow/strobe mode/chosen color, rainbow and strobe color
bool resuming = 1; //wallpaper being resumed from game
uint64_t flashTime; //strobe timing
CRGB *chosenColor = new CRGB(136, 136, 136);
Firework firework = Firework(150, chosenColor, 100, false, false, true);
uint64_t timerFirework = 0;
uint64_t generalTimer = 0; //time between frames in wallpaper

/*Game Vars*/

bool startingGame = true; //new game
Game game = Game();

/*Serial Vars*/

uint64_t timerSerial = 0;

/*Wallpaper Functions*/

void update() { //set all to correct brightness and blue light value
  if (mode < 13 or mode > 15) FastLED.setBrightness(brightness);
  else FastLED.setBrightness(255);
  CRGB setColor;
  if (mode <= 10) setColor = CRGB(255, 255, mode * 25.5);
  else if (mode > 12 and mode < 16) setColor = CRGB(chosenColor->r * (mode == 13), chosenColor->g * (mode == 14), chosenColor->b * (mode == 15));
  else if (mode == 16) setColor = *chosenColor;
  if (mode != 11 and mode != 12 and mode != 17) for (uint16_t i = 0; i < NumLeds; i++) leds[i] = setColor;
}
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

/*Game Functions*/

void bright(bool wait) { //check for brightness change and update
  if (analogRead(XPin) > 923) {
    if (mode == 13 and millis() - timeWallpaper >= 30000) chosenColor->r = max(chosenColor->r - 17, 0);
    else if (mode == 14 and millis() - timeWallpaper >= 30000) chosenColor->g = max(chosenColor->g - 17, 0);
    else if (mode == 15 and millis() - timeWallpaper >= 30000) chosenColor->b = max(chosenColor->b - 17, 0);
    else brightness = max(brightness - 17, 17);
  }
  if (analogRead(XPin) < 100) {
    if (mode == 13 and millis() - timeWallpaper >= 30000) chosenColor->r = min(chosenColor->r + 17, 255);
    else if (mode == 14 and millis() - timeWallpaper >= 30000) chosenColor->g = min(chosenColor->g + 17, 255);
    else if (mode == 15 and millis() - timeWallpaper >= 30000) chosenColor->b = min(chosenColor->b + 17, 255);
    else brightness = min(brightness + 17, 255);
  }
  if ((analogRead(XPin) > 923 or analogRead(XPin) < 100) and millis() - timeWallpaper < 30000) {
    FastLED.setBrightness(brightness);
    if (wait) delay(250);
  }
}
int16_t mod(int16_t x, int16_t y) {
  return x < 0 ? ((x + 1) % y) + y - 1 : x % y;
}

void setup() {
  FastLED.addLeds<WS2812B, DataPin, GRB>(leds, NumLeds);
  FastLED.clear();
  FastLED.show();
  //Needed for RPI
  Serial.begin(9600); //Serial.println("");
  byte *c;
  bool increasingContinue = true;
  bool decreasingContinue = true;
  for (uint16_t i = 0; i <= max(abs(RgbStartLed - RgbEndLed), NumLeds - abs(RgbStartLed - RgbEndLed)); i++) {
    c = Wheel((((uint16_t)i * 256 / NumLeds) + color) & 255); //& works as a form of mod
    if (increasingContinue) {
      uint16_t increasing = mod(RgbStartLed + i, NumLeds);
      leds[increasing] = CRGB(*c, *(c + 1), *(c + 2));
      if (increasing == RgbEndLed) {
        increasingContinue = false;
      }
    }
    if (decreasingContinue) {
      uint16_t decreasing = mod(RgbStartLed - i, NumLeds);
      leds[mod(RgbStartLed - i, NumLeds)] = CRGB(*c, *(c + 1), *(c + 2));
      if (decreasing == RgbEndLed) {
        decreasingContinue = false;
      }
    }
  }
  pinMode(SWPin, INPUT);
  digitalWrite(SWPin, HIGH);
  randomSeed(analogRead(12)); //better random
  pinMode(6, INPUT); //laser sensor input for syncing strobe effect
}
void loop() {

  /*Game*/

  if (millis() - timeWallpaper < 30000) {
    if (!resuming and !startingGame) FastLED.clear(); //normal frame
    if (!resuming) { //for going back to wallpaper
      resuming = 1;
    }
    if (startingGame) { //new game
      Firework(0, CRGB::Green, 200, true, true).run(leds);
      game.start(leds);
      startingGame = false;
    }
    if (analogRead(YPin) > 682 or analogRead(YPin) < 341 or !digitalRead(SWPin)) {
      timeWallpaper = millis();
    }
    game.run(analogRead(YPin) > 682, analogRead(YPin) < 341, !digitalRead(SWPin), leds);
    bright(1); //brightness change
    FastLED.show();
  }

  /*Wallpaper*/

  else {
    if (mode == 12 and digitalRead(6)) { //if laser input recieved then sync with other strip
      color = 0;
      generalTimer = millis() - 151;
      flashTime = millis() - 301;
      delay(90);
    }
    if (millis() - generalTimer > 150) {
      generalTimer = millis();
      if (resuming) { //set up to resume animation
        update();
        FastLED.setBrightness(0);
      }
      if (analogRead(YPin) > 923) { //left, decrease mode
        mode = max(mode - 1, 0);
        if (mode < 13) FastLED.setBrightness(brightness);
      }
      if (analogRead(YPin) < 100) { //right, increase mode
        mode = min(mode + 1, 17);
      }
      if ((analogRead(YPin) > 923 or analogRead(YPin) < 100) and mode <= 10) { //update LEDs if any
        for (uint16_t i = 0; i < NumLeds; i++) {
          leds[i] = CRGB(255, 255, mode * 25.5);
        }
      }
      bright(0);
      if (analogRead(XPin) > 923 or analogRead(XPin) < 100 or analogRead(YPin) > 923 or analogRead(YPin) < 100) { //any
        update();
        if (mode <= 10 or (mode > 12 and mode != 17)) FastLED.show();
      }
      if (mode == 11) { //rainbow effect
        byte *c;
        color = mod(color - 1, 256);
        bool increasingContinue = true;
        bool decreasingContinue = true;
        for (uint16_t i = 0; i <= max(abs(RgbStartLed - RgbEndLed), NumLeds - abs(RgbStartLed - RgbEndLed)); i++) {
          c = Wheel((((uint16_t)i * 256 / NumLeds) + color) & 255); //& works as a form of mod
          if (increasingContinue) {
            uint16_t increasing = mod(RgbStartLed + i, NumLeds);
            leds[increasing] = CRGB(*c, *(c + 1), *(c + 2));
            if (increasing == RgbEndLed) {
              increasingContinue = false;
            }
          }
          if (decreasingContinue) {
            uint16_t decreasing = mod(RgbStartLed - i, NumLeds);
            leds[mod(RgbStartLed - i, NumLeds)] = CRGB(*c, *(c + 1), *(c + 2));
            if (decreasing == RgbEndLed) {
              decreasingContinue = false;
            }
          }
        }
        FastLED.show();
      }
      if (mode == 12 and millis() - flashTime > 300) { //strobe effect
        flashTime = millis();
        color = (color + 1) % 3;
        for (uint16_t i = 0; i < NumLeds; i++) leds[i] = CRGB(255 * (color == 0), 255 * (color == 1), 255 * (color == 2));
        FastLED.show();
      }
      if (digitalRead(SWPin) == 0) timeWallpaper = millis();
      if (resuming) { //fade in on resume
        resuming = 0;
        for (int16_t i = (brightness / 17) * (mode < 13 or mode == 16) + 17 * (mode > 12 and mode < 16); i <= brightness * (mode < 13 or mode == 16) + 255 * (mode > 12 and mode < 16); i += (brightness / 17) * (mode < 13 or mode == 16) + 17 * (mode > 12 and mode < 16)) {
          FastLED.setBrightness(i); //won't work with show(i)
          FastLED.show();
          delay(40);
        }
      }
    }
    if (mode == 17 and millis() - timerFirework >= 20) {
      if (millis() - timerFirework >= 250) {
        for (uint16_t i = 0; i < NumLeds; i++) leds[i] = CRGB::Black;
        firework.reset(random(0, NumLeds), random(10, 41));
      }
      timerFirework = millis();
      if (firework.move(leds)) {
        firework.reset(random(0, NumLeds), random(50, 151));
      }
      FastLED.show();
    }
  }
  if (millis() - timerSerial > 250) {
    timerSerial = millis();
    if (Serial.available() >= 3) {
      uint8_t fade = Serial.read();
      if (fade and mode != 17) {
        for (int16_t i = brightness * (mode < 13 or mode == 16) + 255 * (mode > 12 and mode < 16); i >= 0; i -= (brightness / 17) * (mode < 13 or mode == 16) + 17 * (mode > 12 and mode < 16)) {
          FastLED.setBrightness(i); //won't work with show(i)
          FastLED.show();
          delay(40);
        }
      }
      uint8_t buf[] = {brightness, mode, chosenColor->r, chosenColor->g, chosenColor->b};
      Serial.write(buf, 5);
      brightness = Serial.read();
      mode = Serial.read();
      if (Serial.available() >= 3) {
        chosenColor->r = Serial.read();
        chosenColor->g = Serial.read();
        chosenColor->b = Serial.read();
      }
      /*Serial.print(brightness);
      Serial.print(",");
      Serial.print(mode);
      Serial.print(",");
      Serial.print(chosenColor->r);
      Serial.print(",");
      Serial.print(chosenColor->g);
      Serial.print(",");
      Serial.print(chosenColor->b);
      Serial.println();*/
      if (fade) {
        timeWallpaper = millis() - 30000;
        resuming = 1;
      } else {
        update();
        FastLED.show();
      }
    }
  }
}
