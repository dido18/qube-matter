#pragma once

#include "Modulino.h"

enum QubeFace {
  FACE_TOP = 0,
  FACE_BOTTOM,
  FACE_LEFT,
  FACE_RIGHT,
  FACE_FRONT,
  FACE_BACK
};

class Qube {
public:
    Qube()
      : movement(), leds() // Explicitly construct members here
    {}

    void setup();
    void update();

    bool isShaked();
    bool isTapped();
    int isFaceChanged(); // Returns face id if changed, -1 otherwise

    QubeFace detectFace();

private:
    ModulinoMovement movement;
    ModulinoPixels leds;

    unsigned long lastFace = -1; // Invalid face to force update on first run

    float ax = 0.0f;
    float ay = 0.0f;
    float az = 0.0f;

    int shakeState = 0; // 0: idle, 1: running
    unsigned long shakeStart = 0;
    static constexpr float shakeThreshold = 1.5f;
    static constexpr unsigned long shakeDuration = 100;

    int tapState = 0; // 0: idle, 1: peak detected
    unsigned long peakTime = 0;
    static constexpr float highThreshold = 1.0f;
    static constexpr float lowThreshold = 0.5f;
    static constexpr unsigned long maxInterval = 200; // Max time between high and low threshold
};
