# Qube

> Qube: Make your smart home tangible!


Qube is a Matter-enabled smart cube device that allows you to control smart home actions by physically rotating the cube.

Qube acts as a Matter device with 6 matter switches.

Qube detects which face is pointing up. Every time a new face is pointed up the Qube set the the matter switch to true.

The built-in LEDs provide visual feedback, changing color based on the active face.

It allows integration with Matter-compatible hubs and platforms (such as Alexa, Google Home, etc.).

## Getting Started

> [!IMPORTANT]  
> The Qube can communicate with Matter hubs through a Thread network, so the hubs used must be Thread border routers. See [here]( https://docs.arduino.cc/tutorials/nano-matter/user-manual/#with-amazon-alexa) for the complete list of compatible Matter hub.

1. **Hardware**
   - [1x Arduino Nano Matter](https://docs.arduino.cc/hardware/nano-matter/) ~22€
   - [1x Arduino Nano Connector Carrier](https://store.arduino.cc/products/nano-connector-carrier) ~13€
   - [1x Modulino Movement](https://store.arduino.cc/products/modulino-movement) ~13€
   - [1x Modulino Pixels](https://store.arduino.cc/products/modulino-pixels) 11€
   - 4xAA batteries
   - 1x4-AA Battery Holder ~3€
   - 1x3D printed cube

2. **Configuration**

   - Clone this repository.
   - Open the project  `qube-matter/qube-matter.ino` into the Arduino IDE
   - Before uploading decomment the line `#define ENABLE_MATTER` (and comment the in the `#define ENABLE_LOGGING`) `qube-matter.ino` file
   - Upload the code into the board
   - Follow the [Arduino matter guide](https://docs.arduino.cc/tutorials/nano-matter/user-manual/#matter) to commission the Matter device into you smart device by looking at the pairing code printed in the serial monitor of the Arduino IDE.
   - After the commission, the *6 matter switches* should appears in the smart app.
   - Resolve each matter switch to each face of the cube.  NOTE: since it is not possible to give a "name" to the matter switch (at least in the alexa app) you need to manually resolve what is the matter switch associated to a cube face. One way to do this is to associate a simple action to each matter switch (e.g., send a notifiction), move each face of the cube, rename the matter switch with the current face.

Use your smart app to associated any other action to each of the matter switch.

*Decommission*: To set up the Qube-Matter with a different smart home system (for example, switching from Alexa to Google Home), you must first decommission the device. To do this, press and hold the button for at least 10 seconds. This resets the Qube-Matter, allowing it to be paired with a new platform.
