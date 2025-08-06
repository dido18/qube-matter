#include "qube.h"

Qube qube;

#define ENABLE_MATTER
#define ENABLE_LOGGING

#ifdef ENABLE_MATTER
  #include <Matter.h>
  #include <MatterSwitch.h>
  MatterSwitch faceSwitch[6];
#endif

void onFaceChanged(Qube& cube, QubeFace face){
#ifdef ENABLE_LOGGING
  Serial.print("Face changed to: ");
  Serial.println(FaceToString(face));
#endif

#ifdef ENABLE_MATTER
  faceSwitch[face].set_state(true);
#endif

  switch (face)
  {
  case FACE_TOP:
    cube.SetColor(255,0,0);
    break;
  case FACE_BOTTOM:
    cube.SetColor(0,255,0);
    break;
  case FACE_LEFT:
    cube.SetColor(0,0,255);
    break;
  case FACE_RIGHT:
    cube.SetColor(255,255,0);
    break;
  case FACE_FRONT:
    cube.SetColor(255,0,255);
    break;
  case FACE_BACK:
    cube.SetColor(0,255,255);
    break;
  default:
    break;
  }
  return;
}

void setup() {
  Serial.begin(115200);
  qube.setOnFaceChange(onFaceChanged);
  qube.setup();

#ifdef ENABLE_MATTER
  matterSetup();
#endif
}

void loop() {
  qube.update();

#ifdef ENABLE_MATTER
  decommission_handler();
#endif
}

#ifdef ENABLE_MATTER
void matterSetup() {
  Matter.begin();

  pinMode(BTN_BUILTIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_BUILTIN_INACTIVE);

  for (int i = 0; i < 6; ++i) {
    faceSwitch[i].begin();
    // TODO: set_device_name does not work on alexa ??
    // faceSwitch[i].set_device_name(faceToString(i));
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
  for (int i = 0; i < 6; ++i) {
    while (!faceSwitch[i].is_online()) {
      decommission_handler();
      delay(200);
    }
    Serial.printf("FaceSwitch %d is online...\n", i);
  }
  return ;
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
#endif
