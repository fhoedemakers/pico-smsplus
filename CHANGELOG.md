# CHANGELOG

# General Info

Binaries are at the end of this page.

>[!NOTE]
>For Raspberry Pi Pico 2 you need to download the .uf2 files starting with pico2_


[See readme section how to install and wire up](https://github.com/fhoedemakers/pico-smsplus#pico-smsplus). For more detailed instructions how to setup specific configurations, see the [Pico-InfonesPlus sister project](https://github.com/fhoedemakers/pico-infonesPlus).

- picosmsPlusAdaFruitDVISD.uf2: Printed Circuit Board or Breadboard config with Pico
- picosmsPlusimoroniDV.uf2: Pimoroni Pico DV Demo Base with Pico
- picosmsPlusFeatherDVI.uf2: Adafruit Feather DVI
- picosmsPlusWsRP2040PiZero.uf2: Waveshare RP2040-Pizero
- pico2_picosmsPlusAdaFruitDVISD.uf2: Printed Circuit Board or Breadboard config with Pico 2/RP2350
- pico2_picosmsPlusimoroniDV.uf2: Pimoroni Pico DV Demo Base with Pico 2/RP2350
- pico_nesPCB_v20.zip: PCB Design. For more info see the [Pico-InfonesPlus sister project](https://github.com/fhoedemakers/pico-infonesPlus#pcb-with-raspberry-pi-pico-or-pico-2).

3D-printed case design for Waveshare RP2040-PiZero: [https://www.thingiverse.com/thing:6758682](https://www.thingiverse.com/thing:6758682)

# Release notes

## v0.11

### Features

Replaced font in menu system with new more readable font. [@biblioeteca](https://github.com/biblioeteca)

### Fixes

- none

## v0.10

### Features

- Wii-classic controller now works with WaveShare RP2040-PiZero. [#64](https://github.com/fhoedemakers/pico-infonesPlus/issues/64)

For this to work you need a [Adafruit STEMMA QT / Qwiic JST SH 4-pin Cable with Premium Female Sockets](https://www.adafruit.com/product/4397), a [Adafruit Wii Nunchuck Breakout Adapter - Qwiic](https://www.adafruit.com/product/4836) and a [Wii-classic controller](https://www.amazon.com/s?k=wii-classic+controller)

Connections are as follows:

| Nunchuck Breakout Adapter | RP2040-PiZero |
| ---------------------- | ------------ |
| 3.3V                   | 3V3          |
| GND                    | GND          |
| SDA                    | GPIO2        |
| SCL                    | GPIO3        |


### Fixes

- none

## v0.9

### Features

Added support for Raspberry Pi Pico 2 using these configurations:

- Pimoroni Pico DV Demo Base: pico2_picosmsPlusPlusAdaFruitDVISD.uf2
- Custom PCB: pico2_picosmsPlusAdaFruitDVISD.uf2
- BreadBoard: pico2_picosmsPlusAdaFruitDVISD.uf2

Introducing redesigned PCB. (V2.0) with two NES controller ports for 1 or 2-player games. Design by [@johnedgarpark](https://twitter.com/johnedgarpark)

### Fixes

- None


## v0.8

### Features

- none

### Fixes

- none

### Technical changes

- Executables are built with Pico SDK 2.0.0

## v0.7

### Features

For two player games. When a USB controller is connected, you can connect a NES controller to either Port 1 or Port 2.  
The USB controller is always player 1, the NES controller on Port 1 or Port 2 is player 2. 
In this situation you don't need an extra NES controller port wired for port 2 for playing two player games. The controller connected to port 1 can then be used for player two.

When no USB controller is connected. You can use two NES controllers for two player games. Port 1 is player 1, Port 2 is Player 2.

| | Player 1 | Player 2 |
| --- | -------- | -------- |
| USB controller connected | USB | NES port 1 or NES port 2 |
| No usb controller connected | NES port 1| NES port 2 |

Updated README for two player setup.

### Fixes

- none
  
## v0.6

### Features

- Two player games can now be played. An extra NES controller port can be added to any configuration. Controller port 1 can be a USB or NES controller, controller 2 must be a NES controller. At the moment, no second USB controller can be connected.


### Fixes

- Fixed some compiler errors when building with recent versions of gcc.
  
### Technical changes:

- Pimoroni Pico DV Demo Base: uart output fore debug printf messages is disabled, because gpio1 is needed for the second NES controller port.


## v0.5

### Features

- Display program version on lower right corber of the menu screen.

### Fixes

- Fix memory overflow that causes sound corruption [(issue #6)](https://github.com/fhoedemakers/pico-smsplus/issues/6).
- Variabele which contained the path for the rom to flash was too short in main.cpp.
- When tile overflow occurs, suppress multiple consecutive printf's to console screen to minimize screen flickering. (Prince of Persia)

## v0.4

### Features

- [Game Gear roms can now be played (issue #7).](https://github.com/fhoedemakers/pico-smsplus/issues/7)

### Fixes

- Using DVI functions to properly set top and bottom margins, so emulator display is more or less centered on screen.


## v0.3

### Features

- Added framerate toggle (START + A)

### Fixes

- Fixed colors not rendered properly in the emulator ([issue #4](https://github.com/fhoedemakers/pico-smsplus/issues/4)
- Preserving memory by sharing memory between emulator, main and menu.
- Fixing colors not rendered properly in menu


## v0.2

### Features

- Added several hardware configurations
- Added SDcard and menu
- Added NES and WII-classic controller support

### Fixes

## v0.1

### Features
- Initial release, based on infonesPlus.

### Fixes

