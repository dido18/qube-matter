# Qube

> Qube: Make your smart home tangible!


Qube is a Matter-enabled smart cube device that allows you to control smart home actions by physically rotating the cube.

Each face of the Qube can be assigned a unique color, label, and action, making it a versatile and interactive controller for your Matter-compatible devices and automations.

## Features

- **Matter Integration:**
  Qube acts as a Matter device, allowing seamless integration with Matter-compatible hubs and platforms (such as Alexa, Google Home, Apple HomeKit, etc.).

- **Face Detection:**
  Qube detects which face is pointing up and can trigger different actions based on the current face.

- **Customizable Actions:**
  Each face can be assigned a unique action (callback), color, and label. Actions can include toggling lights, activating scenes, or any custom logic.

- **LED Feedback:**
  The built-in LEDs provide visual feedback, changing color based on the active face or gesture.

## Example Actions

- **Switch on/off smart lights or devices by rotating the cube to a specific face.**
- **Trigger different scenes or automations for each face.**
- **Visual feedback with LEDs to indicate the current face or action.**

## Getting Started

1. **Hardware:**
   - [1x Arduino Nano Matter](https://docs.arduino.cc/hardware/nano-matter/) ~22€
   - [1x Arduino Nano Connector Carrier](https://store.arduino.cc/products/nano-connector-carrier) ~13€
   - [1x Modulino Movement](https://store.arduino.cc/products/modulino-movement) ~13€
   - [1x Modulino Pixels](https://store.arduino.cc/products/modulino-pixels) 11€
   - 4x AA batteries
   - 1x 4-AA Battery Holder ~3€
   - 1x 3D printed cube

2. **Setup:**
   - Clone this repository.
   - Open the project in Arduino IDE
   - Ensure the folder name matches the `.ino` file name (e.g., `qube-matter/qube-matter.ino`).

3. **Commissioning:**
   - Use your Matter hub to commission the Qube device.
   - Follow the serial output for pairing codes and status.

## Limitations

- **Alexa Integration:**
  - Only MatterSwitch triggers are available for automations.
  - "When" clauses are limited to "single touch" events.
  - Other sensor types (e.g., Illuminance) may not be available for triggers in Alexa routines.


