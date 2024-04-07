# CHANGELOG

# General Info

Binaries are at the end of this page.

[See readme section how to install and wire up](https://github.com/fhoedemakers/pico-smsplus#pico-smsplus)

- picosmsPlusAdaFruitDVISD.uf2: Printed Circuit Board or Breadboard config
- picosmsPlusFeatherDVI.uf2: Adafruit Feather DVI
- picosmsPlusimoroniDV.uf2: Pimoroni Pico DV Demo Base
- picosmsPlusWsRP2040PiZero.uf2: Waveshare RP2040-Pizero

# Release notes

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

