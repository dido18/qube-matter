#include "Modulino.h"
#include <Matter.h>
#include <MatterSwitch.h>

MatterSwitch matter_switch;

ModulinoMovement movement;
ModulinoPixels leds;

#define DEBUG
#define ENABLE_MATTER


#ifndef BTN_BUILTIN
  #define BTN_BUILTIN PA0
#endif


float ax;
float ay;
float az;
float roll;
float pitch;
float yaw;

void setup() {
  Serial.begin(115200);
  Modulino.begin();
  movement.begin();
  leds.begin();

  pinMode(BTN_BUILTIN, INPUT_PULLUP);

#ifdef ENABLE_MATTER
  Matter.begin();
  matter_switch.begin();

  if (!Matter.isDeviceCommissioned()) {
    Serial.println("Matter device is not commissioned");
    Serial.println("Commission it to your Matter hub with the manual pairing code or QR code");
    Serial.printf("Manual pairing code: %s\n", Matter.getManualPairingCode().c_str());
    Serial.printf("QR code URL: %s\n", Matter.getOnboardingQRCodeUrl().c_str());
  }
  Serial.printf("Manual pairing code: %s\n", Matter.getManualPairingCode().c_str());
  while (!Matter.isDeviceCommissioned()) {
    delay(200);
  }

  Serial.println("Waiting for Thread network...");
  while (!Matter.isDeviceThreadConnected()) {
    delay(200);
  }
  Serial.println("Connected to Thread network");
  Serial.println("Waiting for Matter device discovery...");
  while (!matter_switch.is_online()) {
    delay(200);
  }
  Serial.println("Matter device is now online");
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

const float sensitivity = 0.5;   // Adjust for responsiveness
const float deadZone = 5.0;      // Ignore small noise
static float brightness = 50.0;  // Initial brightness (0–100%)
static unsigned long lastUpdate = 0;

float getKnobValue(float Gz, unsigned long now) {
  if (lastUpdate == 0) {
    lastUpdate = now;
    return brightness;
  }

  float deltaTime = (now - lastUpdate) / 1000.0;  // in seconds
  lastUpdate = now;

  // Ignore tiny rotations (noise)
  if (abs(Gz) >= deadZone) {
    brightness += (-Gz) * sensitivity * deltaTime;  // +Gz (right) decrease, -Gz  (left) increases
  }

  // Clamp between 0 and 100
  if (brightness > 100) brightness = 100;
  if (brightness < 0) brightness = 0;


  return brightness;
}

bool button_pressed_last = false;


void logAction(String gesture) {
  Serial.print("Triggering Matter action: ");
  Serial.println(gesture);
}


void decommission_handler()
{
  // If the button is not pressed or the device is not commissioned - return
  if (digitalRead(BTN_BUILTIN) != LOW ) {
    Serial.println("Button is not pressed");
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
bool isTappedOn = false;
bool isShakedOn = false;
float knobValaue = 0;

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
    if (isTappedOn == false){
      isTappedOn = true;
      leds.set(0, BLUE, 25);
      leds.show();
      logAction("tap z ON");
      matter_switch.set_state(true);
    }else{
      isTappedOn = false;
      leds.clear(0);
      leds.show();
      logAction("tap z OFF");
      matter_switch.set_state(false);
    }

  }

  knobValaue = getKnobValue(yaw, now);
  int i = map(knobValaue, 0, 100, 0, 7);


  if (isShaked(ax, ay, now)){
    if (isShakedOn == false){
      leds.set(7, RED, 25);
      leds.show();
      isShakedOn = true;
      logAction("shake ON");
    }else{
      isShakedOn = false;
        leds.clear(7);
       leds.show();
      logAction("shake OFF");
    }
  }

   // Handle the decommissioning process if requested
  decommission_handler();
}
