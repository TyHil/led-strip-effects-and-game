/*
  Wallpaper library implementation for Wall LED Strip
  Written by Tyler Gordon Hill
*/
#include "wallpaper.h"



/* Helper */

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

int16_t mod(int16_t x, int16_t y) {
  return x < 0 ? ((x + 1) % y) + y - 1 : x % y;
}



/* Mode */

Mode operator++(Mode& mode) {
  mode = static_cast<Mode>(min(mode + 1, NUM_MODES - 1));
  return mode;
}
Mode operator++(Mode& mode, int) { //postfix operator
  Mode result = mode;
  ++mode;
  return result;
}

Mode operator--(Mode& mode) {
  mode = static_cast<Mode>(max(mode - 1, 0));
  return mode;
}
Mode operator--(Mode& mode, int) { //postfix operator
  Mode result = mode;
  --mode;
  return result;
}



/* Wallpaper */

Wallpaper::Wallpaper(uint8_t setBrightness, Firework *setFirework) {
  firework = setFirework;
  mode = rgb;
  brightness = setBrightness;
  blueLight = 10;
  color = 0;
  timeInput = 0;
  timeRgb = 0;
  timeStrobe = 0;
  timeFirework = 0;
  chosenColor = new CRGB(136, 136, 136);
}

Wallpaper::~Wallpaper() {
  delete chosenColor;
}

void Wallpaper::up(bool inGame, CRGB leds[]) {
  if (millis() - timeInput >= 150) {
    timeInput = millis();
    if (mode == red and !inGame) {
      chosenColor->r = min(chosenColor->r + 17, 255);
    } else if (mode == green and !inGame) {
      chosenColor->g = min(chosenColor->g + 17, 255);
    } else if (mode == blue and !inGame) {
      chosenColor->b = min(chosenColor->b + 17, 255);
    } else {
      brightness = min(brightness + 17, 255);
      setBrightness(inGame);
    }
    if (!inGame) {
      display(true, true, leds);
    }
  }
}

void Wallpaper::down(bool inGame, CRGB leds[]) {
  if (millis() - timeInput >= 150) {
    timeInput = millis();
    if (mode == red and !inGame) {
      chosenColor->r = max(chosenColor->r - 17, 0);
    } else if (mode == green and !inGame) {
      chosenColor->g = max(chosenColor->g - 17, 0);
    } else if (mode == blue and !inGame) {
      chosenColor->b = max(chosenColor->b - 17, 0);
    } else {
      brightness = max(brightness - 17, 17);
      setBrightness(inGame);
    }
    if (!inGame) {
      display(true, true, leds);
    }
  }
}

void Wallpaper::left(CRGB leds[]) {
  if (millis() - timeInput >= 150) {
    timeInput = millis();
    if (mode == white) {
      blueLight = max(blueLight - 1, 0);
    } else {
      mode--;
    }
    setBrightness(false);
    display(true, true, leds);
  }
}

void Wallpaper::right(CRGB leds[]) {
  if (millis() - timeInput >= 150) {
    timeInput = millis();
    if (blueLight < 10) {
      blueLight = min(blueLight + 1, 10);
    } else {
      mode++;
    }
    setBrightness(false);
    display(true, true, leds);
  }
}

void Wallpaper::setBrightness(bool inGame) {
  if (inGame or (mode != red and mode != green and mode != blue)) {
    FastLED.setBrightness(brightness);
  } else {
    FastLED.setBrightness(255);
  }
}

void Wallpaper::fillDisplay(bool show, CRGB leds[]) {
  CRGB setColor;
  if (mode == white) {
    setColor = CRGB(255, 255, blueLight * 25.5);
  } else if (mode == red or mode == green or mode == blue) {
    setColor = CRGB(chosenColor->r * (mode == red), chosenColor->g * (mode == green), chosenColor->b * (mode == blue));
  }
  if (mode == white or mode == red or mode == green or mode == blue or mode == chosen) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
      leds[i] = mode == chosen ? *chosenColor : setColor;
    }
    if (show) {
      FastLED.show();
    }
  }
}

void Wallpaper::rgbEffect(bool show, CRGB leds[]) {
  byte *c;
  color = mod(color - 1, 256);
  bool increasingContinue = true;
  bool decreasingContinue = true;
  for (uint16_t i = 0; i <= max(abs(RGB_START_LED - RGB_END_LED), NUM_LEDS - abs(RGB_START_LED - RGB_END_LED)); i++) {
    c = Wheel((((uint16_t)i * 256 / NUM_LEDS) + color) & 255); //& works as a form of mod
    if (increasingContinue) {
      uint16_t increasing = mod(RGB_START_LED + i, NUM_LEDS);
      leds[increasing] = CRGB(*c, *(c + 1), *(c + 2));
      if (increasing == RGB_END_LED) {
        increasingContinue = false;
      }
    }
    if (decreasingContinue) {
      uint16_t decreasing = mod(RGB_START_LED - i, NUM_LEDS);
      leds[mod(RGB_START_LED - i, NUM_LEDS)] = CRGB(*c, *(c + 1), *(c + 2));
      if (decreasing == RGB_END_LED) {
        decreasingContinue = false;
      }
    }
  }
  if (show) {
    FastLED.show();
  }
}

void Wallpaper::strobeEffect(bool show, CRGB leds[]) {
  color = (color + 1) % 3;
  for (uint16_t i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(255 * (color == 0), 255 * (color == 1), 255 * (color == 2));
  if (show) {
    FastLED.show();
  }
}

void Wallpaper::fireworkEffect(bool show, CRGB leds[]) {
  if (millis() - timeFirework >= 250) { //reset
    for (uint16_t i = 0; i < NUM_LEDS; i++) leds[i] = CRGB::Black;
    firework->reset(random(0, NUM_LEDS), chosenColor, random(10, 41), false, false, true);
  }
  timeFirework = millis();
  if (firework->move(leds)) {
    firework->reset(random(0, NUM_LEDS), random(10, 41));
  }
  if (show) {
    FastLED.show();
  }
}

void Wallpaper::effects(bool show, bool bypass, CRGB leds[]) {
  if (mode == rgb) {
    if (bypass or millis() - timeRgb >= 150) {
      timeRgb = millis();
      rgbEffect(show, leds);
    }
  } else if (mode == strobe) {
    if (bypass or millis() - timeStrobe >= 300) {
      timeStrobe = millis();
      strobeEffect(show, leds);
    }
  } else if (mode == fireworks) {
    if (bypass or millis() - timeFirework >= 20) {
      fireworkEffect(show, leds);
    }
  }
}

void Wallpaper::display(bool show, bool bypass, CRGB leds[]) {
  fillDisplay(show, leds);
  effects(show, bypass, leds);
}

void Wallpaper::run(bool _up, bool _down, bool _left, bool _right, bool resuming, CRGB leds[]) {
  if (_up) {
    up(false, leds);
  }
  if (_down) {
    down(false, leds);
  }
  if (_left) {
    left(leds);
  }
  if (_right) {
    right(leds);
  }
  if (resuming) {
    fillDisplay(false, leds);
  }
  effects(!resuming, resuming, leds);
  if (resuming) {
    bool redGreenBlue = mode == red or mode == green or mode == blue;
    for (
      int16_t i = (brightness / 17) * !redGreenBlue + 17 * redGreenBlue;
      i <= brightness * !redGreenBlue + 255 * redGreenBlue;
      i += (brightness / 17) * !redGreenBlue + 17 * redGreenBlue
    ) {
      FastLED.setBrightness(i); //won't work with show(i)
      FastLED.show();
      delay(40);
    }
  }
}
