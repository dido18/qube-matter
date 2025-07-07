#include "qube.h"
#include "Modulino.h"

void Qube::setup() {
    Modulino.begin();
    this->movement.begin();
    this->leds.begin();
    this->leds.clear();
}

void Qube::update() {
    this->movement.update();

    this->ax = this->movement.getX();
    this->ay = this->movement.getY();
    this->az = this->movement.getZ();
}

bool Qube::isShaked() {
    float magnitude = sqrt(this->ax * this->ax + this->ay * this->ay);
    unsigned long now = millis();

    switch (this->shakeState) {
        case 0:
            if (magnitude > this->shakeThreshold) {
                this->shakeStart = now;
                this->shakeState = 1; // SHAKE_RUNNING
            }
            break;
        case 1: // SHAKE_RUNNING
            if (magnitude > this->shakeThreshold) {
                if (now - this->shakeStart > this->shakeDuration) {
                    this->shakeState = 0; // SHAKE_IDLE
                    this->leds.clear();
                    this->leds.show();
                    return true;  // Shake confirmed
                }
            } else {
                this->shakeState = 0; // SHAKE_IDLE
            }
            break;
    }
    return false;
}

bool Qube::isTapped() {
  float Az = abs(this->az);
  unsigned long now = millis();
  switch (this->tapState) {
    case 0: // IDLE
      if (Az > this->highThreshold) {
        this->peakTime = now;
        this->tapState = 1; // PEAK_DETECTED
      }
      break;

    case 1: // PEAK_DETECTED
      if (Az < this->lowThreshold) {
        if (now - this->peakTime <= this->maxInterval) {
          this->tapState = 0; // IDLE
          this->leds.clear();
          this->leds.show();
          return true;  // Valid tap detected
        } else {
          this->tapState = 0; // Timeout
        }
      } else if (now - this->peakTime > this->maxInterval) {
        this->tapState = 0;  // Timeout
      }
      break;
  }

  return false; // No tap detected
}

QubeFace Qube::detectUpFace() {
    const float threshold = 0.8f; // g, adjust as needed
    if (this->az > threshold) return FACE_TOP;
    if (this->az < -threshold) return FACE_BOTTOM;
    if (this->ax > threshold) return FACE_FRONT;
    if (this->ax < -threshold) return FACE_BACK;
    if (this->ay > threshold) return FACE_RIGHT;
    if (this->ay < -threshold) return FACE_LEFT;
    return FACE_TOP; // Default/fallback
}

QubeFace Qube::isUpFaceChanged() {
    static QubeFace candidateFace = FACE_TOP;
    static unsigned long candidateStart = 0;


    QubeFace currentFace = detectUpFace();
    unsigned long now = millis();

    if (currentFace != this->lastFace) {
        if (currentFace != candidateFace) {
            candidateFace = currentFace;
            candidateStart = now;
        } else if (now - candidateStart >= this->faceStableThreshold) {
            this->lastFace = currentFace;
            this->setFaceColor(currentFace);
            return currentFace;
        }
    } else {
        candidateFace = currentFace;
        candidateStart = now;
    }
    return FACE_UNKNOWN; // Default/fallback if no change detected
}

// Map QubeFace to RGB color
void Qube::setFaceColor(QubeFace face) {
    uint8_t r = 0, g = 0, b = 0;
    uint8_t brightness = 64; // Example brightness, adjust as needed

    switch (face) {
        case FACE_TOP:
        case FACE_BOTTOM:
            r = 255; g = 255; b = 0;   // Yellow
            break;
        case FACE_LEFT:
            r = 0;   g = 255; b = 0;   // Green
            break;
        case FACE_RIGHT:
            r = 0;   g = 0;   b = 255; // Blue
            break;
        case FACE_FRONT:
            r = 255; g = 0;   b = 0;   // Red
            break;
        case FACE_BACK:
            r = 255; g = 128; b = 0;   // Orange
            break;
        default:
            r = 0;   g = 0;   b = 0;   // Off/Unknown
            break;
    }

    this->leds.clear();
    for (int i = 0; i < 8; i++) {
        this->leds.set(i, r, g, b, brightness);
    }
    this->leds.show();
}
