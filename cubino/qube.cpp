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

QubeFace Qube::detectFace() {
    const float threshold = 0.8f; // g, adjust as needed
    if (this->az > threshold) return FACE_TOP;
    if (this->az < -threshold) return FACE_BOTTOM;
    if (this->ax > threshold) return FACE_RIGHT;
    if (this->ax < -threshold) return FACE_LEFT;
    if (this->ay > threshold) return FACE_FRONT;
    if (this->ay < -threshold) return FACE_BACK;
    return FACE_TOP; // Default/fallback
}

int Qube::isFaceChanged() {
    QubeFace currentFace = detectFace();
    if (currentFace != this->lastFace) {
        this->lastFace = currentFace;
        return static_cast<int>(currentFace);
    }
    return -1;
}
