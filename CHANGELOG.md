# CHANGELOG

# General Info

Binaries are at the end of this page.

[See readme section how to install and wire up](https://github.com/fhoedemakers/pico-smsplus#pico-smsplus). For more detailed instructions how to setup specific configurations, see the [Pico-InfonesPlus sister project](https://github.com/fhoedemakers/pico-infonesPlus).

- picosmsPlusAdaFruitDVISD.uf2: Printed Circuit Board or Breadboard config
- picosmsPlusFeatherDVI.uf2: Adafruit Feather DVI
- picosmsPlusimoroniDV.uf2: Pimoroni Pico DV Demo Base
- picosmsPlusWsRP2040PiZero.uf2: Waveshare RP2040-Pizero



# Release notes

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

