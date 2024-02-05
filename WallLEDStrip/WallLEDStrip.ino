/*
  Wall mounted WS2812B LED strip control for Arduino Nano with white light (with blue
  light adjustment), RGB rainbow effect, strobe effect, and specific color choice.
  There is also a joystick controlled game in which the player, a green pixel that
  can shoot a couple pixels ahead of itself, moves from the beginning of the strip to
  the end through an ever growing field of enemies as the game progresses.
  Written by Tyler Hill
  Version 6.0
  Note: Neopixels doesn't have dynamic brightness
*/

#include "input.h"
#include "game.h"
#include "wallpaper.h"
#define DataPin 2
CRGB leds[NumLeds];



/* Input */

Input input;



/* Shared Firework for memory */

Firework *firework = new Firework(0, CRGB::Green, 200, true, true);



/* Wallpaper Vars */

int32_t timeWallpaper = -30000; //inactive time before wallpaper is resumed
bool resuming = true; //wallpaper being resumed from game
Wallpaper wallpaper = Wallpaper(51, firework);



/* Game Vars */

bool startingGame = true; //new game
Game game = Game(firework);



/* Serial Vars */

uint32_t timerSerial = 0;



/* Setup */

void setup() {
  FastLED.addLeds<WS2812B, DataPin, GRB>(leds, NumLeds);
  FastLED.clear();
  FastLED.show();
  //Needed for RPI
  Serial.begin(9600); //Serial.println("");
  input = Input();
  randomSeed(analogRead(12)); //better random
}



void loop() {

  /* Game */

  if (millis() - timeWallpaper < 30000) {
    if (input.left(true) or input.right(true) or input.click()) {
      timeWallpaper = millis();
    }

    if (!resuming and !startingGame) FastLED.clear(); //normal frame
    if (!resuming) { //for going back to wallpaper
      resuming = true;
    }

    if (startingGame) { //new game
      firework->reset(0, CRGB::Green, 200, true, true);
      firework->run(leds);
      game.start(leds);
      startingGame = false;
    }

    game.run(input.left(true), input.right(true), input.click(), leds);

    if (input.up(false)) {
      wallpaper.up(true, leds);
    } else if (input.down(false)) {
      wallpaper.down(true, leds);
    }
    if (input.up(false) or input.down(false)) {
      delay(250);
    }

    FastLED.show();
  }



  /* Wallpaper */

  else {
    wallpaper.run(input.up(false), input.down(false), input.left(false), input.right(false), resuming, leds);
    if (resuming) {
      resuming = false;
    }
    if (input.click()) timeWallpaper = millis();
  }



  /* Serial */

  if (millis() - timerSerial > 250) {
    timerSerial = millis();
    if (Serial.available() >= 3) {
      uint8_t fade = Serial.read();
      if (fade) {
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
      /*Serial.print(wallpaper.brightness);
      Serial.print(",");
      Serial.print(wallpaper.mode);
      Serial.print(",");
      Serial.print(wallpaper.chosenColor->r);
      Serial.print(",");
      Serial.print(wallpaper.chosenColor->g);
      Serial.print(",");
      Serial.print(wallpaper.chosenColor->b);
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
