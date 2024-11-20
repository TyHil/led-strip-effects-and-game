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

#include "game.h"
#include "input.h"
#include "wallpaper.h"
#define DATA_PIN 2
CRGB leds[NUM_LEDS];



/* Input */

Input input;



/* Shared Firework for memory */

Firework * firework = new Firework(0, CRGB::Green, 200, true, true);



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
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
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
    if (input.left(true) or input.right(true) or input.click()) timeWallpaper = millis();

    if (!resuming and !startingGame) FastLED.clear(); //normal frame
    if (!resuming) //for going back to wallpaper
      resuming = true;

    if (startingGame) { //new game
      firework->reset(0, CRGB::Green, 200, true, true);
      firework->run(leds);
      game.start(leds);
      startingGame = false;
    }

    game.run(input.left(true), input.right(true), input.click(), leds);

    if (input.up(false)) wallpaper.up(true, leds);
    else if (input.down(false)) wallpaper.down(true, leds);
    if (input.up(false) or input.down(false)) delay(250);

    FastLED.show();
  }



  /* Wallpaper */

  else {
    wallpaper.run(input.up(false), input.down(false), input.left(false), input.right(false), resuming, leds);
    if (resuming) resuming = false;
    if (input.click()) timeWallpaper = millis();
  }



  /* Serial */

  if (millis() - timerSerial > 100) {
    timerSerial = millis();
    if (Serial.available() >= 4) {
      //fade, brightness, mode[, R, G, B], checkSum
      uint8_t read[7];
      uint8_t amount = Serial.readBytes(read, 7);
      uint16_t sum = 0;
      for (uint8_t i = 0; i < amount - 1; i++) sum += read[i];
      if (amount < 4 || sum % 256 != read[amount - 1]) {
        Serial.write(0);
      } else {
        uint8_t current[] = {
          255,
          wallpaper.brightness,
          wallpaper.mode,
          wallpaper.chosenColor->r,
          wallpaper.chosenColor->g,
          wallpaper.chosenColor->b};
        Serial.write(current, 6);
        if (read[0]) { //fade
          bool redGreenBlue = wallpaper.mode == red or wallpaper.mode == green or wallpaper.mode == blue;
          for (int16_t i = wallpaper.brightness * !redGreenBlue + 255 * redGreenBlue; i >= 0;
               i -= (wallpaper.brightness / 17) * !redGreenBlue + 17 * redGreenBlue) {
            FastLED.setBrightness(i); //won't work with show(i)
            FastLED.show();
            delay(40);
          }
        }
        wallpaper.brightness = read[1];
        wallpaper.mode = static_cast<Mode>(read[2]);
        if (wallpaper.mode == white) wallpaper.blueLight = 5;
        if (amount == 7) {
          wallpaper.chosenColor->r = read[3];
          wallpaper.chosenColor->g = read[4];
          wallpaper.chosenColor->b = read[5];
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
        if (read[0]) { //fade
          timeWallpaper = millis() - 30000;
          resuming = 1;
        } else {
          wallpaper.setBrightness(false);
          wallpaper.display(true, true, leds);
        }
      }
    }
  }
}
