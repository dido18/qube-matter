#include "Modulino.h"
#include "cube.h"

#define DEBUG

Cube cube;


void setup() {
  Serial.begin(9600);
  // Initialize the cube
  cube.setup();
}

void loop() {
  cube.update();
}



// float ax;
// float ay;
// float az;
// float roll;
// float pitch;
// float yaw;


// enum ShakeState {
//   SHAKE_IDLE,
//   SHAKE_RUNNING
// };
// static ShakeState shakeState = SHAKE_IDLE;
// static unsigned long shakeStart = 0;

// const float shakeThreshold = 0.5;        // g
// const unsigned long shakeDuration = 50; // ms


// bool isShaked(float ax, float ay, unsigned long now) {
//   float magnitude = sqrt(ax * ax + ay * ay);

//   // #ifdef DEBUG
//   //   Serial.print("magnitude: ");
//   //   Serial.println(magnitude);
//   // #endif

//   switch (shakeState) {
//     case SHAKE_IDLE:
//       if (magnitude > shakeThreshold) {
//         shakeStart = now;
//         shakeState = SHAKE_RUNNING;
//         #ifdef DEBUG
//           Serial.println("SHAKING START");
//         #endif
//       }
//       break;

//     case SHAKE_RUNNING:
//       if (magnitude > shakeThreshold) {
//         if (now - shakeStart > shakeDuration) {
//           shakeState = SHAKE_IDLE;
//           return true;  // Shake confirmed
//         }
//       } else {
//          #ifdef DEBUG
//           Serial.println("SHAKING INTERRUPTED");
//         #endif
//         // Shake interrupted
//         shakeState = SHAKE_IDLE;
//       }
//       break;
//   }

//   return false;
// }

// const float highThreshold = 1.5;     // g - peak value
// const float lowThreshold = 0.8;      // g - fall back to near rest
// const unsigned long maxInterval = 150; // ms - time between rise and fall

// static enum { IDLE, PEAK_DETECTED } state = IDLE;
// static unsigned long peakTime = 0;
// // Tap: quick, isolated spike, short duration, usually on one axis (Z).
// bool isTapped(float Az, unsigned long now) {
//   Az = abs(Az);
//   switch (state) {
//     case IDLE:
//       if (Az > highThreshold) {
//         peakTime = now;
//         state = PEAK_DETECTED;
//       }
//       break;

//     case PEAK_DETECTED:
//       if (Az < lowThreshold) {
//         if (now - peakTime <= maxInterval) {
//           state = IDLE;
//           return true;  // Valid tap detected
//         } else {
//           state = IDLE; // Timeout
//         }
//       } else if (now - peakTime > maxInterval) {
//         state = IDLE;  // Timeout
//       }
//       break;
//   }

//   return false; // No tap detected
// }

// const float sensitivity = 0.5;   // Adjust for responsiveness
// const float deadZone = 5.0;      // Ignore small noise
// static float brightness = 50.0;  // Initial brightness (0–100%)
// static unsigned long lastUpdate = 0;

// float getKnobValue(float Gz, unsigned long now) {
//   if (lastUpdate == 0) {
//     lastUpdate = now;
//     return brightness;
//   }

//   float deltaTime = (now - lastUpdate) / 1000.0;  // in seconds
//   lastUpdate = now;

//   // Ignore tiny rotations (noise)
//   if (abs(Gz) >= deadZone) {
//     brightness += (-Gz) * sensitivity * deltaTime;  // +Gz (right) decrease, -Gz  (left) increases
//   }

//   // Clamp between 0 and 100
//   if (brightness > 100) brightness = 100;
//   if (brightness < 0) brightness = 0;


//   return brightness;
// }

// void triggerMatterAction(String gesture) {
//   Serial.print("Triggering Matter action: ");
//   Serial.println(gesture);
// }


// unsigned long now;
// bool isTappedOn = false;
// bool isShakedOn = false;
// float knobValaue = 0;
