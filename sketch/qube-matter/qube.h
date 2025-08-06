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

// Helper function to convert QubeFace to string
inline const char* FaceToString(QubeFace face) {
    switch (face) {
        case FACE_TOP:    return "TOP";
        case FACE_BOTTOM: return "BOTTOM";
        case FACE_LEFT:   return "LEFT";
        case FACE_RIGHT:  return "RIGHT";
        case FACE_FRONT:  return "FRONT";
        case FACE_BACK:   return "BACK";
        default:          return "UNKNOWN";
    }
}

class Qube; // Forward declaration for use in CallbackFunction

// This function will be called when the face of the Qube changes.
// It provides the Qube instance, the previous face, and the new face.
// This allows for custom actions to be performed when the face changes.
typedef void (*CallbackFunction)(Qube&, QubeFace, QubeFace);

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
        for (int i = 0; i < 8; ++i) {
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
