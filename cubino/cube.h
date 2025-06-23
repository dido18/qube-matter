#pragma once

#include <Arduino.h>
#include "Modulino.h"

class Cube {

public:
    ModulinoMovement movement;
    ModulinoPixels leds;

Cube() {
    this->movement = ModulinoMovement();
    this->leds = ModulinoPixels();
    Modulino.begin();
    movement.begin();
    leds.begin();
  }
  void setup();
  void update();

private:
    bool isShaked(float ax, float ay, unsigned long now);
    bool isTapped(float az, unsigned long now);
    float getKnobValue(float gz, unsigned long now);
};
