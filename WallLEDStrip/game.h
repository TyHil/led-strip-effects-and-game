/*
  Game library header for Wall LED Strip
  Written by Tyler Gordon Hill
*/
#include <FastLED.h>
#define NumLeds 300

#include "firework.h"

#define MaxEnemies 23

class Player {
  public:
    int16_t pos;
    uint8_t shotLength; //distance of shooting
    uint32_t timeMove; //how often player moves
    uint32_t timeShoot; //how often player shoots
    int8_t enemyCountAtDeath; //number of eneies (level) of last death
    int16_t posAtDeath; //position of last death
    Player();
    void reset(uint8_t enemyCount);
    bool left();
    bool right();
    bool shoot(CRGB leds[]);
    void adjustShotLength(uint8_t enemyCount);
    void gravestone(uint8_t enemyCount, int16_t location, CRGB leds[]);
    void gravestone(uint8_t enemyCount, CRGB leds[]);
    void death(uint8_t enemyCount, CRGB leds[]);
    void display(CRGB leds[]);
};

class Enemy {
  public:
    int16_t pos;
    Enemy();
    void reset();
    void reset(int16_t setPos);
    void move(int16_t playerPos);
    void display(CRGB leds[]);
};

class Game {
  public:
    Player player;
    Enemy enemies[23];
    uint8_t enemyCount; //number of enemies
    uint32_t timeEnemyMove; //how often enemies move
    Firework *firework;
    Game(Firework *setFirework);
    void displayLevel(CRGB leds[]);
    void start(CRGB leds[]);
    void left(CRGB leds[]);
    void right(CRGB leds[]);
    void shoot(CRGB leds[]);
    void deathCheck(CRGB leds[]);
    void enemyDeathCheck(int16_t pos);
    void enemyReset();
    void enemyMove(CRGB leds[]);
    void enemyDisplay(CRGB leds[]);
    void run(bool _left, bool _right, bool _shoot, CRGB leds[]);
    void displaySingle(CRGB leds[], int16_t i);
    void display(CRGB leds[]);
    void winCheck(CRGB leds[]);
};
