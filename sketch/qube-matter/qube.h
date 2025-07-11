#ifndef QUBE_H
#define QUBE_H

#include "Arduino.h"
#include "Modulino.h"

enum QubeFace {
  FACE_TOP = 0,
  FACE_BOTTOM = 1,
  FACE_LEFT = 2,
  FACE_RIGHT = 3,
  FACE_FRONT = 4,
  FACE_BACK = 5,
  FACE_UNKNOWN = -1
};

class Qube; // Forward declaration for use in CallbackFunction

typedef void (*CallbackFunction)(Qube&, QubeFace);

class Qube {
public:
    Qube()
      : movement(), leds()
    { }

    void setup();
    void update();

    void setOnFaceChange(CallbackFunction callback) {
        this->onFaceChange = callback;
    }

    void SetColor( uint8_t r, uint8_t g, uint8_t b, uint8_t brightness = 5){
        for (int i = 0; i < 6; ++i) {
            this->leds.set(i, ModulinoColor(r, g, b), brightness);
        }
        this->leds.show();
    }

private:
    ModulinoMovement movement;
    ModulinoPixels leds;

    CallbackFunction onFaceChange = NULL;

    QubeFace upFace = FACE_UNKNOWN;
    QubeFace candidateFace = FACE_UNKNOWN;
    unsigned long candidateStart = 0;
    int faceState = 0;             // 0: face idle, 1: face changed

    QubeFace detectUpFace() {
        float ax = this->movement.getX();
        float ay = this->movement.getY();
        float az = this->movement.getZ();

        const float threshold = 0.8f; // g, adjust as needed
        if (az > threshold) return FACE_TOP;
        if (az < -threshold) return FACE_BOTTOM;
        if (ax > threshold) return FACE_FRONT;
        if (ax < -threshold) return FACE_BACK;
        if (ay > threshold) return FACE_RIGHT;
        if (ay < -threshold) return FACE_LEFT;
        return FACE_UNKNOWN; // Default/fallback
    }
    
};

#endif
