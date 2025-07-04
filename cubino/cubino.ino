#include "qube.h"
#include <Matter.h>
#include <MatterSwitch.h>

Qube qube;

// #define ENABLE_MATTER

#ifdef ENABLE_MATTER
MatterSwitch tap;
MatterSwitch shake;
MatterSwitch faceSwitch[6]; // One for each face
#endif

void matterSetup();
void decommission_handler();

void setup() {
  Serial.begin(115200);
  
  qube.setup();
  matterSetup();
}

void loop() {
  qube.update();

  if(qube.isTapped()) {
    Serial.println("tapped");
  }
  if(qube.isShaked()){
    Serial.println("shaked");
  }

  QubeFace changedFace = qube.isUpFaceChanged();
  if (changedFace != FACE_UNKNOWN) {
    Serial.printf("Face %d (%s) is pointing to sky\n", changedFace, faceToString(changedFace));
  }

  // Handle the decommissioning process if requested
  decommission_handler();
}

void matterSetup() {
#ifdef ENABLE_MATTER
  Matter.begin();
  tap.begin();
  shake.begin();

  for (int i = 0; i < 6; ++i) {
    faceSwitch[i].begin();
  }

  if (!Matter.isDeviceCommissioned()) {
    Serial.println("Matter device is not commissioned");
    Serial.println("Commission it to your Matter hub with the manual pairing code or QR code");
    Serial.printf("Manual pairing code: %s\n", Matter.getManualPairingCode().c_str());
    Serial.printf("QR code URL: %s\n", Matter.getOnboardingQRCodeUrl().c_str());
  }
  while (!Matter.isDeviceCommissioned()) {
    delay(200);
  }
  Serial.println("Waiting for Thread network...");
  while (!Matter.isDeviceThreadConnected()) {
    decommission_handler();
    delay(200);
  }
  Serial.println("Connected to Thread network");
  Serial.println("Waiting for Matter device discovery...");
  while (!tap.is_online()) {
    decommission_handler();
    delay(200);
  }
  Serial.println("Tap is online...");
  while (!shake.is_online()) {
    decommission_handler();
    delay(200);
  }
  Serial.println("Shake is online...");
   for (int i = 0; i < 6; ++i) {
    while (!faceSwitch[i].is_online()) {
      decommission_handler();
      delay(200);
    }
    Serial.printf("FaceSwitch %d is online...\n", i);
  }
  return ;
#endif
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
