#pragma once

#include "Modulino.h"

enum QubeFace {
  FACE_TOP = 0,
  FACE_BOTTOM,
  FACE_LEFT,
  FACE_RIGHT,
  FACE_FRONT,
  FACE_BACK,
  FACE_UNKNOWN = -1 // Added to handle unknown face state
};

class Qube {
public:
    Qube()
      : movement(), leds()
    {}

    void setup();
    void update();

    bool isShaked();
    bool isTapped();
    QubeFace isUpFaceChanged();

private:
    ModulinoMovement movement;
    ModulinoPixels leds;

    QubeFace detectUpFace();

    unsigned long lastFace = FACE_UNKNOWN; // Invalid face to force update on first run

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

    static constexpr unsigned long faceStableThreshold = 300; // ms, time to consider face stable
};
