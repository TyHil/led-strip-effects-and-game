/*
  Wall mounted WS2812B LED strip control for Arduino Nano with white light (with blue
  light adjustment), RGB rainbow effect, strobe effect, and specific color choice that
  syncs with the desk light code through a laser sensor. There is also a joystick
  controlled game in which the player, a green pixel that can shoot a couple pixels
  ahead of itself, moves from the beginning of the strip to the end through an ever
  growing field of enemies as the game progresses.
  Written by Tyler Hill
  Version 6.0
  Note: Neopixels doesn't have dynamic brightness
*/

#include "game.h"
#include "wallpaper.h"
#define DataPin 2
#define YPin A3
#define XPin A2
#define SWPin 4
CRGB leds[NumLeds];



/* Wallpaper Vars */

int32_t timeWallpaper = -30000; //inactive time before wallpaper is resumed
bool resuming = 1; //wallpaper being resumed from game
Wallpaper wallpaper = Wallpaper(51);



/* Game Vars */

bool startingGame = true; //new game
Game game = Game();



/* Serial Vars */

uint32_t timerSerial = 0;



/* Inputs */

bool up() {
  return analogRead(YPin) > 682;
}

bool down() {
  return analogRead(YPin) < 341;
}

bool left() {
  return analogRead(YPin) > 923;
}

bool right() {
  return analogRead(YPin) < 100;
}

bool click() {
  return digitalRead(SWPin) == 0;
}



/* Setup */

void setup() {
  FastLED.addLeds<WS2812B, DataPin, GRB>(leds, NumLeds);
  FastLED.clear();
  FastLED.show();
  //Needed for RPI
  Serial.begin(9600); //Serial.println("");
  wallpaper.rgbEffect(false, leds); ///?
  ///wallpaper.rgbEffect(true, leds); ///???
  pinMode(SWPin, INPUT);
  digitalWrite(SWPin, HIGH);
  randomSeed(analogRead(12)); //better random
}



void loop() {

  /* Game */

  if (millis() - timeWallpaper < 30000) {
    if (left() or right() < 341 or click()) {
      timeWallpaper = millis();
    }

    if (!resuming and !startingGame) FastLED.clear(); //normal frame
    if (!resuming) { //for going back to wallpaper
      resuming = 1;
    }

    if (startingGame) { //new game
      Firework(0, CRGB::Green, 200, true, true).run(leds);
      game.start(leds);
      startingGame = false;
    }

    game.run(left(), right(), click(), leds);

    if (up()) {
      wallpaper.up(true, leds);
    } else if (down()) {
      wallpaper.down(true, leds);
    }
    if (up() or down()) {
      delay(250);
    }

    FastLED.show();
  }



  /* Wallpaper */

  else {
    wallpaper.run(up(), down(), left(), right(), resuming, leds);
    if (resuming) {
      resuming = 0;
    }
    if (click()) timeWallpaper = millis();
  }



  /* Serial */

  if (millis() - timerSerial > 250) {
    timerSerial = millis();
    if (Serial.available() >= 3) {
      uint8_t fade = Serial.read();
      if (fade and wallpaper.mode != fireworks) {
        bool redGreenBlue = wallpaper.mode == red or wallpaper.mode == green or wallpaper.mode == blue;
        for (
          int16_t i = wallpaper.brightness * !redGreenBlue + 255 * redGreenBlue;
          i >= 0;
          i -= (wallpaper.brightness / 17) * !redGreenBlue + 17 * redGreenBlue
        ) {
          FastLED.setBrightness(i); //won't work with show(i)
          FastLED.show();
          delay(40);
        }
      }
      uint8_t buf[] = {wallpaper.brightness, wallpaper.mode, wallpaper.chosenColor->r, wallpaper.chosenColor->g, wallpaper.chosenColor->b};
      Serial.write(buf, 5);
      wallpaper.brightness = Serial.read();
      wallpaper.mode = static_cast<Mode>(Serial.read());
      if (Serial.available() >= 3) {
        wallpaper.chosenColor->r = Serial.read();
        wallpaper.chosenColor->g = Serial.read();
        wallpaper.chosenColor->b = Serial.read();
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
        wallpaper.display(true, true, leds);
      }
    }
  }
}
