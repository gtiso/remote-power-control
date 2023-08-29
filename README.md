# Arduino Home Power Control via SMS

Welcome to the Arduino Home Power Control project repository! This project demonstrates how to control the electrical power of a house using an Arduino and SMS messages sent from a phone. With this setup, you can remotely manage the power state of your appliances and devices, adding a layer of convenience and energy efficiency to your home.

## Introduction

Managing household electrical devices remotely can be extremely useful, especially for scenarios like turning off forgotten appliances or ensuring energy savings. This project uses an Arduino board and a GSM module to control power states via SMS commands.

## Features

- Turn on/off power remotely using SMS commands.
- Receive status updates via SMS about the current state.
- User-friendly SMS command structure for easy interaction.

## Requirements

To replicate this project, you will need the following components:

- Arduino board (e.g., Arduino Uno)
- GSM module (e.g., SIM800L)
- Relay module for controlling power
- Mobile phone capable of sending SMS
- Jumper wires
- Power supply for the Arduino, Sim and Relay module

## Hardware Setup

1. Connect the GSM module to the Arduino board using UART communication.
2. Connect the relay module(s) to the Arduino pins. Make sure to follow the relay module's specifications.
3. Connect the necessary power supply to the Arduino and relay module(s).
4. Ensure proper grounding and wiring to prevent any electrical hazards.

Schematics for the wiring instructions : 

| Arduino Pins | Destination |
| ------ | ------ |
| D2 | Led Pin |
| D3 | Button Pin |
| D4 | Relay Pin |
| D5 | SIM800L RxD(4) |
| D6 | SIM800L TxD(5) |
| A4 | LCD SDA |
| A5 | LCD SDL |

## Software Setup

1. Install the Arduino IDE on your computer if not already installed.
2. Connect your Arduino board to your computer using a USB cable.
3. Open the **'PowerControl.ino'** sketch in the Arduino IDE.
4. Configure the GSM module and relay pins according to your hardware setup.
5. Upload the sketch to the Arduino board.


## Usage

1. Power on your Arduino board and wait for the initialization to complete.
2. Send an SMS from your mobile phone with the appropriate commands to the GSM module's phone number.
    * To turn on the power: Send POWER ON.
    * To turn off the power: Send POWER OFF.
    * To get the current status : Send STATUS or STATE.
3. You will receive SMS replies confirming the action or providing the status.

## Future Improvements

This project can be extended in various ways:

- Implement a more secure authentication mechanism for SMS commands.
- Develop a mobile app for easier and more intuitive control.
- Integrate with home automation platforms for centralized management.
- Add support for scheduling tasks and automation.

## Contributing
Contributions to this project are welcome! If you have any ideas, bug fixes, or improvements, feel free to open an issue or submit a pull request.

*Feel free to reach out to the project maintainers if you have any questions or need assistance. Happy hacking!*
