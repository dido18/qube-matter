#include "qube.h"

void Qube::setup() {
    Modulino.begin();
    this->movement.begin();
    this->leds.begin();
    this->leds.clear();

    this->upFace = this->detectUpFace();
    this->candidateFace = FACE_UNKNOWN;
    this->candidateStart = 0;

}

void Qube::update() {
    this->movement.update();

    QubeFace newFace = this->detectUpFace();
    unsigned long now = millis();

    switch (this->faceState)
    {
    case 0:
        if (newFace != this->upFace) {
            this->faceState = 1;
            this->candidateStart = now;
            this->candidateFace = newFace;
        }
        break;
    case 1:
        if (newFace != this->candidateFace) {
            this->candidateStart = now;
            this->candidateFace = newFace;
            break;
        }

        if (newFace == this->candidateFace){
            if (now - this->candidateStart > 500) {;
                QubeFace previousFace = this->upFace;
                this->upFace = newFace;
                this->candidateFace = FACE_UNKNOWN; // Reset candidate face
                this->faceState = 0;
                if (this->onFaceChange) {
                    this->onFaceChange(*this, previousFace,  this->upFace);
                }
                break;
            }
        }
        break;
    default:
        break;
    }
}
