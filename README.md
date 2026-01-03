# Meshcamper

Meshtastic interface for camper van/RV.

## Goal

Access and control campervan tech via Meshtastic, like battery state, climate, heater or fridge.

* Turn on diesel heater when almost back from your hike
* Check if the ice cream in the fridge still survived
* Watch charging/discharging the leisure battery

## Devices

Currently supported and planned devices:

* Victron BLE devices
 * Victron BMV-712 battery monitor (/)
* Indoor climate sensor (x)
* Small OLED display module (WIP)
* Westfalia/Paragon control panel (x)
 * Control diesel heater (x)

## Setup

### Build (Hardware)

Parts:
* ESP32 Dev board v1 (clone)
* SH1106 I2C OLED display module (128x64, 1.3")

Pinout:
* D16: Serial RX (to serial TX of Meshtastic device)
* D17: Serial TX (to serial RX of Meshtastic device)
* D21: I2C SDA (Display)
* D22: I2C SCL (Display)
* 3V3: Connect to display and sensor VCC
* GND: connect to GND of all peripherals
* VIN: (5V) Connect to 5V power source and 5V of Meshtastic device

### Build (software)

Install Pioarduino (or PlatformIO).

create a file `src/config.h` with your config:

```
#define VICTRON_BMV_BLE_MAC "xx:xx:xx:xx:xx:xx"
#define VICTRON_BMV_INSTANT_READOUT_KEY "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

#define COMMAND_WORD "camp"

// optional: list of devices allowed to interact, finish with 0x0
#define OWN_DEVICES { 0x123445678, 0xabcdef01, 0x0 }
```

### Flash

Connect, and run:
`pio run -t upload`

### Install

RX pin is Pin 16, TX pin is Pin 17. Connect them (**crossed**) to your designated RX/TX pins on your
Meshtastic device. Also connect common ground.

Usually both the ESP32 board running this and the Meshtastic device can be fed from the same 5V source (USB,...),
so connecting ground and 5V pins does the trick. Provide quality wire gauge, as Meshtastic devices draw over 1A during TX.

### Configure Meshtastic

* Configure a private primary channel. **otherwise strangers can interact with the device!**
* enable "Serial" module
 * RX/TX pins as soldered.
 * Baud rate: 9600
 * "Protobufs" mode
 * Save. Device does reboot.

Done.

## Usage

Don't use the phone via BLE simultaneously, it conflicts with the serial connection.

Send a text message from a favorited or configured trusted device.

### Commands

Format is always `<command word> <module> <parameters>`. In the examples below, let's use `bus`.
Commands are case-insensitive.

#### Battery (`b`)
Get Information from the Victron battery monitor:

Send: `bus b`
Response:
```
B:13.28V:95.8%,S:12.64V,P:-2.6W(t-0.8s)
```
It contains (Main) battery voltage, SoC, Aux battery voltage/value, current power flow, age of last info from BMV.

#### Report

Enable or disable a regular device report (e.g. if you are charging the battery, have a power hungry consumer running, want to monitor the state over longer periods, or are just curious).

* `bus r` Toggles report on and off. Default interval is 120s.
* `bus r 1` Enable report.
* `bus r 1 <time>` Enable report, and set custom interval in seconds.
* `bus r 0` Disable report.

Each report looks like
```
B93%S12.6V
```
Contains main battery SoC and starter battery voltage.
