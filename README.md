# ESP Joker*
(*working title)

ESP32 Meshtastic interface for Club Joker

## Goal

Access and control RV* tech via Meshtastic

(* Westfalia Club Joker camper van)

## Devices

Supported and planned devices

* Victron BLE devices
 * Victron BMV-712 battery monitor (/)
* Indoor climate sensor (x)
* Westfalia/Paragon control panel (x)
 * Control diesel heater

## Setup

### Build

Install Pioarduino (or PlatformIO).

create a file `src/config.h` with your config:

```
#define VICTRON_BMV_BLE_MAC "xx:xx:xx:xx:xx:xx"
#define VICTRON_BMV_INSTANT_READOUT_KEY "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

#define COMMAND_WORD "camp"

// list of devices allowed to interact, finish with 0x0
#define OWN_DEVICES { 0x123445678, 0xabcdef01, 0x0 }
```

### Flash

Connect, and run:
`pio run -t upload`

### Install

RX pin is Pin 16, TX pin is Pin 17. Connect them (**crossed**) to your designated RX/TX pins on your
Meshtastic device. Also connect common ground.

Usually both the ESP32 board running this and the Meshtastic device can be fed from the same 5V source (USB,...),
so connecting ground and 5V pins does the trick.

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

 Send a text message from a configured trusted device.

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