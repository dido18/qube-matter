#include "qube.h"

#include <Matter.h>
#include <MatterSwitch.h>

Qube qube;

MatterSwitch tap;
MatterSwitch shake;
MatterSwitch faceSwitch[6]; // One for each face


void setup() {
  Serial.begin(115200);
  qube.setup();
}


unsigned long now;

void loop() {
  qube.update();

  if(qube.isTapped()) {
    Serial.println("tapped");
  }
  if(qube.isShaked()){
    Serial.println("shaked");
  }

  int changedFace = qube.isFaceChanged();
  if (changedFace != -1) {
    Serial.printf("Face %d (%s) is touching the ground\n", changedFace, faceToString(changedFace));
  } 

    // Handle the decommissioning process if requested
  decommission_handler();

}


const char* faceToString(int face) {
  switch (face) {
    case FACE_TOP: return "TOP";
    case FACE_BOTTOM: return "BOTTOM";
    case FACE_LEFT: return "LEFT";
    case FACE_RIGHT: return "RIGHT";
    case FACE_FRONT: return "FRONT";
    case FACE_BACK: return "BACK";
    default: return "UNKNOWN";
  }
}


void decommission_handler()
{
  // If the button is not pressed or the device is not commissioned - return
  if (digitalRead(BTN_BUILTIN) != LOW ) {
    return;
  }
  if (!Matter.isDeviceCommissioned()){
    return;
  }

  // Store the time when the button was first pressed
  uint32_t start_time = millis();
  // While the button is being pressed
  while (digitalRead(BTN_BUILTIN) == LOW) {
    // Calculate the elapsed time
    uint32_t elapsed_time = millis() - start_time;
    // If the button has been pressed for less than 10 seconds, continue
    if (elapsed_time < 10000u) {
      yield();
      continue;
    }

    // Blink the LED to indicate the start of the decommissioning process
    for (uint8_t i = 0u; i < 10u; i++) {
      digitalWrite(LED_BUILTIN, !(digitalRead(LED_BUILTIN)));
      delay(100);
    }

    Serial.println("Starting decommissioning process, device will reboot...");
    Serial.println();
    digitalWrite(LED_BUILTIN, LED_BUILTIN_INACTIVE);
    // This function will not return
    // The device will restart once decommissioning has finished
    Matter.decommission();
  }
}
