/*
  Wall mounted WS2812B LED strip control with white light (with blue light adjustment),
  RGB rainbow effect, and strobe effect that syncs with the desk light code through a
  laser sensor. There is also a joystick controlled game in which the player, a green
  pixel that can shoot a couple pixels ahead of itself, moves from the beginning of the
  strip to the end through an ever growing field of enemies as the game progresses.
  Written by Tyler Hill
  Version 2.0
  Note: Neopixels doesn't have dynamic brightness
*/
#include <FastLED.h>
#define NumLeds 300
#define DataPin 2
#define YPin A3
#define XPin A2
#define SWPin 4
#define Mid 161
boolean startingGame = true;//new game
uint8_t shotLength = 2;//distance of shooting in game
uint8_t brightness = 40;
uint8_t blue = 11;//bluelight
uint8_t color;//effect color
boolean resuming = 1;//wallpaper being resumed from game
unsigned long flashTime;//strobe timing
int16_t playerPos;//position of player in game
int16_t enemiesPos[23];//positions of each enemy
uint8_t enemies = 4;//number of enemies
unsigned long timeEnemyMove;//how often enemies move
unsigned long timeGeneral;//time between shots in game and time between frames in wallpper
long timeWallpaper = -30000;//inactive time before wallpaper is resumed
int8_t diedEnemies = -1;//number of eneies (level) of last death
int16_t diedPos = -1;//position of last death
CRGB leds[NumLeds];

/*Functions*/

void bright(bool wait) {//check for brightness change and update
  if (analogRead(XPin) > 923) brightness = max(brightness - 10, 10);
  if (analogRead(XPin) < 100) brightness = min(brightness + 10, 100);
  if (analogRead(XPin) > 923 or analogRead(XPin) < 100) {
    FastLED.setBrightness(brightness);
    if (wait) delay(250);
  }
}
void died(int toTest) {//if a grvaestone shsould be placed at a position of last death
  if (enemies == diedEnemies and toTest == diedPos) leds[diedPos] = CRGB(20, 20, 20);
}
void adjustShotLength() {//shot lenght get bigger after a point
  if (enemies < 9) {
    shotLength = 2;
  } else {
    shotLength = 3;
  }
}
void explosion(int start, CRGB color) {//generates an explosion at a point with a certain color
  /*CRGB colorList[51];//code for a random color explsion
    for (int j = 0; j < 50; j++) colorList[j] = CRGB(random(0,255), random(0,255), random(0,255));
    colorList[50] = color;*/
  float slopes[51];//creates linear equations with random slopes
  for (int j = 0; j < 50; j++) slopes[j] = (float)random(-2000, 2001) / 100;//higher slope higher speed
  slopes[50] = 0;
  for (int frame = 0; frame < 150; frame++) {//each frame puts a new x (time) value into the linear equations and gets a different y value (position)
    if (frame < 80) for (int j = 0; j < 51; j++) leds[abs(-abs(-abs((int16_t)(start + slopes[j]*pow(frame, 0.6))) + NumLeds - 1) + NumLeds - 1)] = CRGB::Black;//remove last position, starts leaving a trail after 80 frames, the abs() and + NumLeds - 1 allow the particles to bounce
    for (int j = 0; j < 51; j++) leds[abs(-abs(-abs((int16_t)(start + slopes[j]*pow(frame + 1, 0.6))) + NumLeds - 1) + NumLeds - 1)] = color;//set new position, use colorList[j] instead of color is for a random color explsion
    FastLED.show();
    delay(20);
  }
  for (uint16_t i = 0; i < 20; i++) {//fill empty spots between dots to create full green
    for (int j = 0; j < 51; j++) {
      int16_t lightPos = abs(-abs(-abs((int16_t)(start + slopes[j] * pow(150, 0.6))) + NumLeds - 1) + NumLeds - 1);
      leds[min(max(lightPos + i, 0), NumLeds - 1)] = color;
      leds[min(max(lightPos - i, 0), NumLeds - 1)] = color;
    }
    FastLED.show();
    delay(20);
  }
}
void test() {//checks if any enemies are on top of the player
  for (uint8_t i = 0; i < enemies; i++) {
    if (playerPos == enemiesPos[i]) {
      explosion(playerPos, CRGB::Green);
      diedEnemies = enemies;
      diedPos = playerPos;
      start();
      break;
    }
  }
}
void load() {//set new enemy positions
  for (uint8_t i = 0; i <= 23; i++) enemiesPos[i] = -1;
  for (uint8_t i = 0; i < enemies; i++) enemiesPos[i] = random(6, NumLeds - 2);
}
void DeadEnemy(int j) {//check for enemies in players shot
  for (uint8_t i = 0; i < enemies; i++) if (enemiesPos[i] == playerPos + j) enemiesPos[i] = -1;
}
void start() {//reset game
  if (enemies > 4) {//if not on first level
    int j = 0;//adds a gap every 5 leds
    for (int i = 20; i <= enemies + 18; i++) {//display level beaten
      delay(100);
      leds[i + j] = CRGB::Red;
      FastLED.show();
      if (i % 5 == 4) {
        j++;
      }
    }
    delay(1000);
    j = 0;
    for (int i = 20; i <= enemies + 19; i++) {//remove display
      leds[i + j] = CRGB::Green;
      if (i % 5 == 4) {
        j++;
      }
    }
    FastLED.show();
  }
  delay(100);
  load();
  enemies = 4;
  adjustShotLength();
  for (int i = 20; i <= 23; i++) leds[i] = CRGB::Red;//dsiplay level 1, 4 enemies
  boolean isNotEnemy = true;
  leds[NumLeds - 1] = CRGB::Blue;
  FastLED.show();
  for (int i = NumLeds - 2; i > 0; i--) {//display enemies
    isNotEnemy = true;
    for (int j = 0; j < enemies; j++) {
      if (i == enemiesPos[j]) {
        leds[i] = CRGB::Red;
        isNotEnemy = false;
      }
    }
    if (isNotEnemy) {
      if (enemies == diedEnemies and i == diedPos) leds[i] = CRGB(20, 20, 20);//gravestone
      else leds[i] = CRGB::Black;
    }
    FastLED.show();
  }
  playerPos = 0;
  leds[0] = CRGB::Green;
  FastLED.show();
}
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

void setup() {
  //Serial.begin(9600);//Serial.println("");
  FastLED.addLeds<WS2812B, DataPin, GRB>(leds, NumLeds);
  byte *c;///does this need to be a pointer
  for (uint8_t i = 0; i <= Mid; i++) {//set first rainbow frame
    c = Wheel((((uint16_t)i * 256 / NumLeds)) & 255);
    leds [i] = CRGB(*c, *(c + 1), *(c + 2));
    if (2 * Mid - i < NumLeds) leds [2 * Mid - i] = CRGB(*c, *(c + 1), *(c + 2));
  }
  pinMode(SWPin, INPUT);
  digitalWrite(SWPin, HIGH);///why
  randomSeed(analogRead(12));//better random
  pinMode(6, INPUT);//laser sensor input for syncing strobe effect
}
void loop() {

  /*Game*/

  if (millis() - timeWallpaper < 30000) {
    if (!resuming and !startingGame) FastLED.clear();//normal frame
    if (!resuming) {//coming from wallpaper
      resuming = 1;
      leds[playerPos] = CRGB::Green;
    }
    if (startingGame) {//new game
      explosion(0, CRGB::Green);
      start();
      startingGame = false;
    }
    if (analogRead(YPin) > 682) {//left
      leds[playerPos] = CRGB::Black;
      died(playerPos);
      playerPos++;
      leds[playerPos] = CRGB::Green;
      test();
      timeWallpaper = millis();
      delay(40);
    }
    if (analogRead(YPin) < 341 and playerPos != 0) {//rigth
      leds[playerPos] = CRGB::Black;
      died(playerPos);
      playerPos--;
      leds[playerPos] = CRGB::Green;
      test();
      timeWallpaper = millis();
      delay(40);
    }
    bright(1);//brightness change
    if (millis() - timeEnemyMove >= 250 - 10 * (enemies - 4)) {
      for (uint8_t i = 0; i < enemies; i++) {
        if (enemiesPos[i] != -1) {
          uint8_t r = random(0, 4);//1/2 chance move forward or charge player
          if ((r == 0 or r == 1 or enemiesPos[i] - 1 == playerPos or enemiesPos[i] - 2 == playerPos or enemiesPos[i] - 3 == playerPos or enemiesPos[i] - 4 == playerPos or enemiesPos[i] - 5 == playerPos) and enemiesPos[i] != 0) {
            leds[enemiesPos[i]] = CRGB::Black;
            died(enemiesPos[i]);
            enemiesPos[i]--;
            leds[enemiesPos[i]] = CRGB::Red;
          } else if ((r == 2) and enemiesPos[i] < NumLeds - 2) {//1/4 chance move back
            leds[enemiesPos[i]] = CRGB::Black;
            died(enemiesPos[i]);
            enemiesPos[i]++;
            leds[enemiesPos[i]] = CRGB::Red;
          }
        }
      }
      timeEnemyMove = millis();
      test();
    }
    if (playerPos == NumLeds - 1) {//beat level
      enemies++;
      adjustShotLength();
      load();
      explosion(NumLeds - 1, CRGB::Blue);
      int j = 0;
      for (int i = 20; i <= enemies + 19; i++) {//display level
        delay(100);
        leds[i + j] = CRGB::Red;
        FastLED.show();
        if (i % 5 == 4) {
          j++;
        }
      }
      delay(500);
      playerPos = 0;
      boolean isNotEnemy;
      leds[playerPos] = CRGB::Green;
      FastLED.show();
      for (int i = 1; i <= NumLeds - 2; i++) {//display enemies
        isNotEnemy = true;
        for (uint8_t j = 0; j < enemies; j++) {
          if (i == enemiesPos[j]) {
            leds[i] = CRGB::Red;
            isNotEnemy = false;
          }
        }
        if (isNotEnemy) {
          if (enemies == diedEnemies and i == diedPos) leds[i] = CRGB(20, 20, 20);//gravestone
          else leds[i] = CRGB::Black;
        }
        FastLED.show();
      }
      leds[NumLeds - 1] = CRGB::Blue;
      FastLED.show();
    }
    if (!digitalRead(SWPin)) {
      timeWallpaper = millis();
    }
    if (millis() - timeGeneral >= 750 and !digitalRead(SWPin)) {//shot
      leds[playerPos] = CRGB::Blue;
      FastLED.show();
      delay(50);
      for (uint8_t i = 1; i < shotLength + 1; i++) {//shot out animation
        if (playerPos < NumLeds - i) leds[playerPos + i] = CRGB::Blue;
        FastLED.show();
        delay(50);
      }
      delay(50);
      for (uint8_t i = shotLength; i > 0; i--) {//shot in animation and check for hits
        if (playerPos < NumLeds - i) {
          leds[playerPos + i] = CRGB::Black;
          DeadEnemy(i);
        }
        FastLED.show();
        delay(50);
      }
      leds[playerPos] = CRGB::Green;
      leds[NumLeds - 1] = CRGB::Blue;
      timeGeneral = millis();
      delay(50);
    }
    FastLED.show();
  }

  /*Wallpaper*/

  else {
    if (blue == 12 and digitalRead(6)) {//if laser input recieved then sync with other strip
      color = 0;
      timeGeneral = millis() - 151;
      flashTime = millis() - 301;
      delay(90);
    }
    if (millis() - timeGeneral > 150) {
      timeGeneral = millis();
      if (resuming) {//set up to resume animation
        if (blue <= 10) for (uint16_t i = 0; i < NumLeds; i++) leds[i] = CRGB(255, 255, blue * 25.5);
        if (blue <= 11) FastLED.setBrightness(0);
      }
      if (analogRead(YPin) > 923) {//left, decrease blue light
        blue = max(blue - 1, 0);
      }
      if (analogRead(YPin) < 100) blue = min(blue + 1, 12);//right, increase blue light and enter other lighting modes after max blue light
      if ((analogRead(YPin) > 923 or analogRead(YPin) < 100) and blue <= 10) for (uint16_t i = 0; i < NumLeds; i++) leds[i] = CRGB(255, 255, blue * 25.5);//update LEDs if any
      bright(0);
      if (analogRead(XPin) > 923 or analogRead(XPin) < 100 or analogRead(YPin) > 923 or analogRead(YPin) < 100) FastLED.show();//show if any
      if (blue == 11) {//rainbow effect
        byte *c;
        color = (color + 1) % 256;
        for (uint8_t i = 0; i <= Mid; i++) {
          c = Wheel((((uint16_t)i * 256 / NumLeds) + color) & 255);//& works as a form of mod
          leds [i] = CRGB(*c, *(c + 1), *(c + 2));
          if (2 * Mid - i < NumLeds) leds [2 * Mid - i] = CRGB(*c, *(c + 1), *(c + 2));
        }
        FastLED.show();
      }
      if (blue == 12 and millis() - flashTime > 300) {//strobe effect
        flashTime = millis();
        color = (color + 1) % 3;
        for (uint16_t i = 0; i < NumLeds; i++) leds[i] = CRGB(255 * (color == 0), 255 * (color == 1), 255 * (color == 2));
        FastLED.show();
      }
      if (digitalRead(SWPin) == 0) timeWallpaper = millis();
      if (resuming) {//fade in on resume
        resuming = 0;
        if (blue <= 11) {
          for (uint8_t i = 0; i <= brightness; i++) {
            FastLED.setBrightness(i);
            FastLED.show();
            delay(40);
          }
        }
      }
    }
  }
}
