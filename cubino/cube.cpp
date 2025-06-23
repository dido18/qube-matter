#include "cube.h"

void Cube::setup() {
    this->movement->setup();
    this->leds->setup();

    // Set the initial state of the LEDs
    this->leds->clear();
    this->leds->show();
}

void Cube::update() {
     this->movement.update();

    ax = this->movement.getX();
    ay = this->movement.getY();
    az = this->movement.getZ();

    roll = this->movement.getRoll();
    pitch = this->movement.getPitch();
    yaw = this->movement.getYaw();

    now = millis();

    if (this->isTapped(az, now)) {
        // if (isTappedOn == false){
        // isTappedOn = true;
        this->leds.set(0, BLUE, 25);
        // leds.show();
        // }else{
        // isTappedOn = false;
        // leds.clear(0);
        // leds.show();
        // }
        // triggerMatterAction("tap z");
    }

    int knobValaue = this->getKnobValue(yaw, now);
    int i = map(knobValaue, 0, 100, 0, 7);


    if (this->isShaked(ax, ay, now)){
        // if (isShakedOn == false){
        this->leds.set(7, RED, 25);
        // leds.show();
        // isShakedOn = true;
        // }else{
        // isShakedOn = false;
        // leds.clear(7);
        // leds.show();
        // }
        // triggerMatterAction("shaked");
    }
}

bool Cube::isShaked(float ax, float ay, unsigned long now) {
    // Implement shake detection logic here
    return false;
}

bool Cube::isTapped(float ax, float ay, unsigned long now) {
    // Implement shake detection logic here
    return false;
}

float Cube::getKnobValue(float gz, unsigned long now) {
    // Implement knob value calculation logic here
    return 0.0;
}
