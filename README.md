# Arduino RFID Door Lock System
A smart door control and alert demo for Arduino Uno R3: using servo lock, buttons for entry/exit, buzzer and LEDs for alerts, and RFID authentication for alarm clearance.

## Features
* Servo-based door lock/unlock simulation

* Control buttons for exit (with alert) and entry (no alert)

* Red LED + buzzer alarm when exiting

* Green LED indicates RFID authentication success

* Only authorized RFID cards can deactivate the alarm

* Button debouncing for reliable switching

* Schematic drawing included for hardware setup

## Hardware List
* Arduino Uno R3

* Servo motor (SG90 or similar)

* Red LED & 220Ω resistor

* Green LED & 220Ω resistor

* Passive buzzer (parallel to red LED)

* Two push buttons

* RFID RC522 module

* Breadboard & jumper wires

## How It Works
* Press "ButtonIn" button: door opens, alert (red LED + buzzer) starts

* Press "ButtonOut" button: door opens, no alert

* With alert on, scan authorized RFID card to stop alert; green LED blinks

* Door closes by pressing the respective button after entry/exit

