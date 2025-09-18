# Qube

Qube is a Matter-enabled smart cube device for tangible smart home control. Rotate the cube to trigger actions—each face maps to a Matter switch. Built-in LEDs provide visual feedback, and Qube integrates with Matter-compatible hubs (Alexa, Google Home, etc.).

## Getting Started

> **Important:** Qube communicates with Matter hubs via Thread. Your hub must be a Thread border router. See [compatible hubs](https://docs.arduino.cc/tutorials/nano-matter/user-manual/#with-amazon-alexa).

### Hardware Requirements
- [Arduino Nano Matter](https://docs.arduino.cc/hardware/nano-matter/)
- [Arduino Nano Connector Carrier](https://store.arduino.cc/products/nano-connector-carrier)
- [Modulino Movement](https://store.arduino.cc/products/modulino-movement)
- 1 or 2x [Modulino Pixels](https://store.arduino.cc/products/modulino-pixels)
- 4x AA batteries & holder
- 3D printed cube



### Setup Instructions
1. Clone this repository:
   ```bash
   git clone git@github.com:dido18/qube-matter.git
   ```
2. Open `qube-matter.ino` in Arduino IDE.
3. (Optional) If using 2 Modulino Pixels, see next section.
4. In `qube-matter.ino`, uncomment `#define ENABLE_MATTER` and comment `#define ENABLE_LOGGING`.
5. Upload the code to your board.
6. Follow the [Arduino Matter guide](https://docs.arduino.cc/tutorials/nano-matter/user-manual/#matter) to commission the device. Use the pairing code from the serial monitor.
7. After commissioning, 6 Matter switches will appear in your smart app.
8. Map each switch to a cube face. (Alexa does not allow naming switches; manually test and rename each switch by triggering actions.)
9. Use your smart app to assign smart actions (e.g., turn on lamp) to each switch.


### Configuring Two Modulino Pixels
If using two Modulino Pixels modules, you must assign a unique address to the second module to avoid conflicts.

**Steps to change the address of the second Modulino Pixels:**
1. Connect one Modulino Pixels to the board (tested on UNO r4 WIFI).
2. Open Arduino IDE.
3. Go to `File > Examples > Modulino > Utilities > AddressChanger`.
4. Upload the AddressChanger sketch to your board.
5. In the serial monitor, enter: `<CURRENT_ADDRESS> 0x4D` (e.g., `0x6C 0x4D`).
6. Press Enter and wait for completion.
7. Disconnect the first Modulino Pixels and repeat for the second if needed.

> The new address (`0x4D`) must be reflected in your main sketch when initializing the second Modulino Pixels.



### Decommissioning
To pair Qube-Matter with a different smart home system (e.g., switching from Alexa to Google Home):
- Press and hold the button for at least 10 seconds to reset the device. You can now pair it with a new platform.
