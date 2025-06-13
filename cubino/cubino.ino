#include "Modulino.h"

ModulinoMovement movement;

float ax;
float ay;
float az;
float roll;
float pitch;
float yaw;

const float GRAVITY = 0.9; // Should be 1g, but seeing at the value it is not precise.  Gravity in g-units (1g = 9.80665 m/s²)



void setup() {
  Serial.begin(9600);
  Modulino.begin();
  movement.begin();
}


// void processCubeGesture(float ax, float ay, float az, float gx, float gy, float gz) {
//   // Double tap detection: Check for a spike above threshold (ignore gravity ~1g)
//   checkDoubleTap(ax, ay, az);
// }

 // Internal state variables
   // Tap detection parameters
const float highThreshold = 1.5;     // g - peak value
const float lowThreshold = 0.8;      // g - fall back to near rest
const unsigned long maxInterval = 150; // ms - time between rise and fall

static enum { IDLE, PEAK_DETECTED } state = IDLE;
static unsigned long peakTime = 0;

bool checkTapZ(float Az, unsigned long now) {
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

enum TwistState { TWIST_IDLE, TWIST_RIGHT, TWIST_LEFT };

const float twistThreshold = 100.0;       // deg/s to detect twist start
const float stopThreshold = 30.0;          // deg/s to detect stop
const unsigned long minDuration = 50;      // ms
const unsigned long maxDuration = 300;     // ms

static TwistState twistState = TWIST_IDLE;
static unsigned long startTime = 0;

String checkTwistZ(float Gz, unsigned long now) {
  switch (twistState) {
    case TWIST_IDLE:
      if (Gz > twistThreshold) {
        twistState = TWIST_RIGHT;
        startTime = now;
      } else if (Gz < -twistThreshold) {
        twistState = TWIST_LEFT;
        startTime = now;
      }
      break;

    case TWIST_RIGHT:
      if (abs(Gz) < stopThreshold) {
        unsigned long duration = now - startTime;
        if (duration >= minDuration && duration <= maxDuration) {
          twistState = TWIST_IDLE;
          return "right";
        } else {
          twistState = TWIST_IDLE;
        }
      } else if (now - startTime > maxDuration) {
        // Timeout
        twistState = TWIST_IDLE;
      }
      break;

    case TWIST_LEFT:
      if (abs(Gz) < stopThreshold) {
        unsigned long duration = now - startTime;
        if (duration >= minDuration && duration <= maxDuration) {
          state = IDLE;
          return "left";
        } else {
          state = IDLE;
        }
      } else if (now - startTime > maxDuration) {
        // Timeout
        state = IDLE;
      }
      break;
  }

  return "";  // no twist detected
}

void triggerMatterAction(String gesture) {
  Serial.print("Triggering Matter action: ");
  Serial.println(gesture);
  delay(2000);
  // Replace with actual Matter event or command call
  // e.g., matterLight.toggle(), matterScene.activate("scene1"), etc.
}


String twist;

void loop() {
  movement.update();

  ax = movement.getX();
  ay = movement.getY();
  az = movement.getZ();

  roll = movement.getRoll();
  pitch = movement.getPitch();
  yaw = movement.getYaw();

  // Serial.print("x ");
  // Serial.print(ax, 3);
  // Serial.print("	y ");
  // Serial.print(ay, 3);
  // Serial.print("	z ");
  // Serial.println(az, 3);

  // // Print gyroscope values
  Serial.print(roll, 1);
  Serial.print(", ");
  Serial.print(pitch, 1);
  Serial.print(", ");
  Serial.println(yaw, 1);

  if (checkTapZ(az, millis())) {
    // triggerMatterAction("tap z");
  }
  twist = checkTwistZ(yaw, millis() );
  if (twist != ""){
    Serial.println(twist);
    triggerMatterAction("twisted");
  }
}
