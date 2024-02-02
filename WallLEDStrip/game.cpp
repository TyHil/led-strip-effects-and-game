/*
  Game library header for Wall LED Strip
  Written by Tyler Gordon Hill
*/
#include "game.h"
#include "firework.h"



/* Player */

Player::Player() {
  pos = 0;
  shotLength = 2;
  timeMove = 0;
  timeShoot = 0;
  enemyCountAtDeath = -1;
  posAtDeath = -1;
}

void Player::reset(uint8_t enemyCount) {
  pos = 0;
  adjustShotLength(enemyCount);
}

bool Player::left() {
  if (pos < NumLeds - 1 and millis() - timeMove >= 40) {
    timeMove = millis();
    pos++;
    return true;
  }
  return false;
}

bool Player::right() {
  if (pos > 0 and millis() - timeMove >= 40) {
    timeMove = millis();
    pos--;
    return true;
  }
  return false;
}

bool Player::shoot(CRGB leds[]) {
  if (millis() - timeShoot >= 40) {
    for (uint8_t i = 0; i < shotLength + 1; i++) { //shot out animation
      if (pos + i < NumLeds - 1) {
        leds[pos + i] = CRGB::Blue;
      }
      FastLED.show();
      delay(50);
    }
    delay(50);
    for (uint8_t i = shotLength; i > 0; i--) { //shot in animation
      if (pos + i < NumLeds - 1) {
        leds[pos + i] = CRGB::Black;
      }
      FastLED.show();
      delay(50);
    }
    leds[pos] = CRGB::Green;
    timeShoot = millis();
    return true;
  }
  return false;
}

void Player::adjustShotLength(uint8_t enemyCount) { //shot length gets bigger after a point
  if (enemyCount < 9) {
    shotLength = 2;
  } else {
    shotLength = 3;
  }
}

void Player::gravestone(uint8_t enemyCount, int16_t location, CRGB leds[]) {
  if (enemyCount == enemyCountAtDeath and location == posAtDeath){
    leds[location] = CRGB(20, 20, 20);
  }
}

void Player::gravestone(uint8_t enemyCount, CRGB leds[]) {
  if (enemyCount == enemyCountAtDeath){
    leds[posAtDeath] = CRGB(20, 20, 20);
  }
}

void Player::death(uint8_t enemyCount, CRGB leds[]) {
  Firework(pos, CRGB::Green, 200, true, true).run(leds);
  enemyCountAtDeath = enemyCount;
  posAtDeath = pos;
}

void Player::display(CRGB leds[]) {
  leds[pos] = CRGB::Green;
}



/* Enemy */

Enemy::Enemy() {}

void Enemy::reset() {
  pos = random(6, NumLeds - 2);
}

void Enemy::reset(int16_t setPos) {
  pos = setPos;
}

void Enemy::move(int16_t playerPos) {
  if (pos != -1) {
    uint8_t r = random(0, 4); //1/2 chance move forward except always charge player
    bool forward = r == 0 or r == 1;
    for (uint8_t i = 1; i < 6 and !forward; i++) {
      forward = pos - i == playerPos;
    }
    if (forward and pos != 0) {
      pos--;
    } else if (r == 2 and pos < NumLeds - 2) { //1/4 chance move back
      pos++;
    }
  }
}

void Enemy::display(CRGB leds[]) {
  if (pos != -1) {
    leds[pos] = CRGB::Red;
  }
}



/* Game */

Game::Game() {
  enemyCount = 4;
  timeEnemyMove = millis();
  player = Player();
  for (uint8_t i = 0; i < MaxEnemies; i++) {
    enemies[i] = Enemy();
  }
}

void Game::displayLevel(CRGB leds[]) {
  int16_t gap = 0; //adds a gap every 5 leds
  int16_t START = 20;
  for (int16_t i = START; i <= enemyCount + START - 4; i++) { //display level
    leds[i + gap] = CRGB::Red;
    FastLED.show();
    delay(100);
    if ((i - START + 1) % 5 == 0) {
      gap++;
      delay(100);
    }
    if ((i - START + 1) % 10 == 0) {
      gap++;
      delay(100);
    }
  }

  delay(2000);

  for (int16_t i = enemyCount + START - 4; i >= START; i--) { //remove display
    if ((i - START + 1) % 5 == 0) {
      gap--;
      delay(100);
    }
    if ((i - START + 1) % 10 == 0) {
      gap--;
      delay(100);
    }
    leds[i + gap] = CRGB::Green;
    FastLED.show();
    delay(100);
  }
}

void Game::start(CRGB leds[]) {
  if (enemyCount > 4) { //if not on first level
    displayLevel(leds); //display level beaten
  }

  enemyCount = 4;
  enemyReset();
  player.reset(enemyCount);
  displayLevel(leds); //display level 1, 4 enemies
  boolean isNotEnemy = true;
  leds[NumLeds - 1] = CRGB::Blue;
  FastLED.show();
  for (int16_t i = NumLeds - 2; i > 0; i--) { //display enemies
    displaySingle(leds, i);
    FastLED.show();
  }
  player.display(leds);
  FastLED.show();
}

void Game::left(CRGB leds[]) {
  if (player.left()) {
    deathCheck(leds);
  }
}

void Game::right(CRGB leds[]) {
  if (player.right()) {
    deathCheck(leds);
  }
}

void Game::shoot(CRGB leds[]) {
  if (player.shoot(leds)) {
    for (uint8_t i = player.shotLength; i > 0; i--) { //check for hits
      if (player.pos + i < NumLeds - 1) {
        enemyDeathCheck(player.pos + i);
      }
    }
  }
}

void Game::deathCheck(CRGB leds[]) { //checks if any enemies are on top of the player
  for (uint8_t i = 0; i < enemyCount; i++) {
    if (player.pos == enemies[i].pos) {
      player.death(enemyCount, leds);
      start(leds);
      break;
    }
  }
}

void Game::enemyDeathCheck(int16_t pos) { //checks if any enemies are on top of the player
  for (uint8_t i = 0; i < enemyCount; i++) {
    if (pos == enemies[i].pos) {
      enemies[i].reset(-1);
    }
  }
}

void Game::enemyReset() {
  for (uint8_t i = 0; i < enemyCount; i++) enemies[i].reset();
  for (uint8_t i = enemyCount; i < 23; i++) enemies[i].reset(-1);
}

void Game::enemyMove(CRGB leds[]) {
  if (millis() - timeEnemyMove >= (uint64_t) (250 - 10 * (enemyCount - 4))) {
    timeEnemyMove = millis();
    for (uint8_t i = 0; i < enemyCount; i++) {
      enemies[i].move(player.pos);
    }
    deathCheck(leds);
  }
}

void Game::enemyDisplay(CRGB leds[]) {
  for (uint8_t i = 0; i < enemyCount; i++) enemies[i].display(leds);
}

void Game::winCheck(CRGB leds[]) {
  if (player.pos == NumLeds - 1) { //beat level
    enemyCount++;
    player.adjustShotLength(enemyCount);
    enemyReset();
    Firework(NumLeds - 1, CRGB::Blue, 200, true, true).run(leds);
    displayLevel(leds);
    player.reset(enemyCount);
    player.display(leds);
    FastLED.show();
    for (int i = 1; i <= NumLeds - 2; i++) { //display enemies
      displaySingle(leds, i);
      FastLED.show();
    }
    leds[NumLeds - 1] = CRGB::Blue;
    FastLED.show();
  }
}

void Game::displaySingle(CRGB leds[], int16_t i) {
  bool isNotEnemy = true;
  for (uint8_t j = 0; j < enemyCount; j++) {
    if (i == enemies[j].pos) {
      enemies[j].display(leds);
      isNotEnemy = false;
    }
  }
  if (isNotEnemy) {
    leds[i] = CRGB::Black;
    player.gravestone(enemyCount, i, leds);
  }
  FastLED.show();
}

void Game::display(CRGB leds[]) {
  FastLED.clear();
  player.gravestone(enemyCount, leds);
  player.display(leds);
  for (uint8_t i = 0; i < enemyCount; i++) enemies[i].display(leds);
  leds[NumLeds - 1] = CRGB::Blue;
}

void Game::run(bool _left, bool _right, bool _shoot, CRGB leds[]) {
  if (_left) {
    left(leds);
  }
  if (_right) {
    right(leds);
  }
  if (_shoot) {
    shoot(leds);
  }
  enemyMove(leds);
  display(leds);
  winCheck(leds);
}
