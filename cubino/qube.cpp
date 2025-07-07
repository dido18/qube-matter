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
  const float TAP_THRESHOLD = 0.5;  // Adjust as needed (in g's)
  const unsigned long TAP_COOLDOWN = 300; // in ms

  unsigned long now = millis();
  float deltaAz = fabs(this->az - 1.0);  
  if (deltaAz > TAP_THRESHOLD && (now - this->peakTime > TAP_COOLDOWN)) {
      Serial.println("Tap detected (software)!");
      this->peakTime = now;
      return true;
  }
  return false;
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
            return currentFace;
        }
    } else {
        candidateFace = currentFace;
        candidateStart = now;
    }
    return FACE_UNKNOWN; // Default/fallback if no change detected
}
