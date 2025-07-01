#include "Modulino.h"
#include <Matter.h>
#include <MatterSwitch.h>

ModulinoMovement movement;
ModulinoPixels leds;

#define DEBUG
#define ENABLE_MATTER


MatterSwitch tap;
MatterSwitch shake;
MatterSwitch faceSwitch[6]; // One for each face


enum CubeFace {
  FACE_TOP = 0,
  FACE_BOTTOM,
  FACE_LEFT,
  FACE_RIGHT,
  FACE_FRONT,
  FACE_BACK
};

// Helper to detect which face is down based on 1g force
CubeFace detectFace(float ax, float ay, float az) {
  const float threshold = 0.8; // g, adjust as needed
  if (az > threshold) return FACE_TOP;
  if (az < -threshold) return FACE_BOTTOM;
  if (ax > threshold) return FACE_RIGHT;
  if (ax < -threshold) return FACE_LEFT;
  if (ay > threshold) return FACE_FRONT;
  if (ay < -threshold) return FACE_BACK;
  return FACE_TOP; // Default/fallback
}


float ax;
float ay;
float az;
float roll;
float pitch;
float yaw;

unsigned long lastFace = 6; // Invalid face to force update on first run


void setup() {
  Serial.begin(115200);

  Modulino.begin();
  movement.begin();
  leds.begin();
  leds.clear();

  pinMode(BTN_BUILTIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_BUILTIN_INACTIVE);

  lastFace = detectFace(ax, ay, az);

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
  // TODO: show leds animantion to show it is connecting to thread
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
#endif
}


enum ShakeState {
  SHAKE_IDLE,
  SHAKE_RUNNING
};
static ShakeState shakeState = SHAKE_IDLE;
static unsigned long shakeStart = 0;

const float shakeThreshold = 0.5;        // g
const unsigned long shakeDuration = 50; // ms


bool isShaked(float ax, float ay, unsigned long now) {
  float magnitude = sqrt(ax * ax + ay * ay);

  // #ifdef DEBUG
  //   Serial.print("magnitude: ");
  //   Serial.println(magnitude);
  // #endif

  switch (shakeState) {
    case SHAKE_IDLE:
      if (magnitude > shakeThreshold) {
        shakeStart = now;
        shakeState = SHAKE_RUNNING;
        // #ifdef DEBUG
        //   Serial.println("SHAKING START");
        // #endif
      }
      break;

    case SHAKE_RUNNING:
      if (magnitude > shakeThreshold) {
        if (now - shakeStart > shakeDuration) {
          shakeState = SHAKE_IDLE;
          return true;  // Shake confirmed
        }
      } else {
        //  #ifdef DEBUG
        //   Serial.println("SHAKING INTERRUPTED");
        // #endif
        // Shake interrupted
        shakeState = SHAKE_IDLE;
      }
      break;
  }

  return false;
}

const float highThreshold = 1.5;     // g - peak value
const float lowThreshold = 0.8;      // g - fall back to near rest
const unsigned long maxInterval = 150; // ms - time between rise and fall

static enum { IDLE, PEAK_DETECTED } state = IDLE;
static unsigned long peakTime = 0;
// Tap: quick, isolated spike, short duration, usually on one axis (Z).
bool isTapped(float Az, unsigned long now) {
  Az = abs(Az);
  switch (state) {
    case IDLE:
      if (Az > highThreshold) {
        peakTime = now;
        state = PEAK_DETECTED;
      }
      break;

    case PEAK_DETECTED:
      if (Az < lowThreshold) {
        if (now - peakTime <= maxInterval) {
          state = IDLE;
          return true;  // Valid tap detected
        } else {
          state = IDLE; // Timeout
        }
      } else if (now - peakTime > maxInterval) {
        state = IDLE;  // Timeout
      }
      break;
  }

  return false; // No tap detected
}


void logAction(String gesture) {
  Serial.print("Triggering Matter action: ");
  Serial.println(gesture);
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


unsigned long now;

void loop() {
  movement.update();

  ax = movement.getX();
  ay = movement.getY();
  az = movement.getZ();

  roll = movement.getRoll();
  pitch = movement.getPitch();
  yaw = movement.getYaw();

  now = millis();

  if (isTapped(az, now)) {
    Serial.println("tapped");
      tap.set_state(true);
      leds.set(0, BLUE, 25);
      leds.show();
      delay(500);
      leds.clear();
  }


  if (isShaked(ax, ay, now)){
     Serial.println("shaked");
      shake.set_state(true);
      leds.set(7, RED, 25);
      leds.show();
      delay(500);
      leds.clear();
  }

  CubeFace currentFace = detectFace(ax, ay, az);
   if (currentFace != lastFace) {
    for (int i = 0; i < 6; ++i) {
      faceSwitch[i].set_state(i == currentFace);
    }
    Serial.printf("Face %d is touching the ground\n", currentFace);
    lastFace = currentFace;
  }

  // Handle the Matter actions for tap and shake
  // Handle the decommissioning process if requested
  decommission_handler();
}
