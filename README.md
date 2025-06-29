
# pico-smsplus

This software is a port of [SmsPlus](https://segaretro.org/SMS_Plus), a Sega Master System and Game Gear emulator for RP2040/RP2350 based microcontroller boards like the RaspberryPi Pico and Pico 2. Sound and video are ouput over HDMI.
The code for HDMI output is based on [Shuichi Takano's Pico-InfoNes project](https://github.com/shuichitakano/pico-infones) which in turn is based on [PicoDVI](https://github.com/Wren6991/PicoDVI).

Put your Master System (.sms) and Game Gear (.gg) rom files on a FAT32 formatted SD card. You can organize the roms in directories. A menu is displayed on which you can select the rom to play.

Supports two controllers for two player Master System games. [See "about two player games" below for specifics and limitations](#about-two-player-games) 

***

## Video
Click on image below to see a demo video.

[![Video](https://img.youtube.com/vi/__E8h2Ay3g8/0.jpg)](https://www.youtube.com/watch?v=__E8h2Ay3g8)

***
## Not perfect

**There are still some issues to be fixed. The emulator runs decently well, especially on the Pico 2/RP2350. On the Pico/RP2040, Some games may not run at full speed or at all. (afterburner).**

>[!WARNING] 
> On Pico/RP2040, some games show red flashing between screens. This can be occasionally or severe depending on the game. If you are sensitive for this, or experience health issues while playing those games, please stop playing immediately.
> Runs much better on Pico 2/RP2350.

***

## Save Game Support for SMS Games

This update adds save game functionality for Sega Master System (SMS) games that support it.

### How It Works

- **Saving**: To ensure game data is written to the SD card, you must return to the main menu by pressing **Select + Start**.
- **Save Location**: Save files are stored in the `/SAVES` directory with a `.SAV` extension.
- **Auto-Loading**: When you start a game, its corresponding `.SAV` file will be loaded automatically (if available).

>[!NOTE]
>Make sure to return to the menu before powering off or switching games, or the save data may be lost.

***

## System requirements and setup - What do yo need?

The binary specific for your config can be downloaded from the [releases](https://github.com/fhoedemakers/pico-smsplus/releases/latest) page.

You need a FAT32 formatted SD card to put your .sms and .gg roms on.

>[!NOTE]
> For detailed instructions how to setup specific configurations, see the [Pico-InfonesPlus sister project](https://github.com/fhoedemakers/pico-infonesPlus).

*** 

## For Raspberry Pi Pico / Pico W / Pico 2

- Raspberry Pi Pico and [Pimoroni Pico DV Demo Base](https://shop.pimoroni.com/products/pimoroni-pico-dv-demo-base?variant=39494203998291)
- [Custom Printed Circuit Board](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#pcb-with-raspberry-pi-pico)
- Breadboard with components.

*** 

## Other RP2040 based boards

These boards already contain an RP2040 cpu, a separate Raspberry Pi Pico is not needed.

- [Adafruit Feather RP2040 with DVI Output](https://www.adafruit.com/product/5710) and [FeatherWing - RTC + SD](https://www.adafruit.com/product/2922).
- [Waveshare RP2040-PiZero Development Board](https://www.waveshare.com/rp2040-pizero.htm)

## Other RP2350 based boards

- [Adafruit Metro RP2350](https://www.adafruit.com/product/6003) or [Adafruit Metro RP2350 with PSRAM](https://www.adafruit.com/product/6267)


***

## Supported USB controllers
The following controllers are supported.

- BUFFALO BGC-FC801
- SONY DUALSHOCK 4
- SONY DualSense
- Xinput type controllers (Xbox)
- and more...

Also original NES and WII-classic controllers are supported in some configurations. 

See the [Pico-InfonesPlus sister project](https://github.com/fhoedemakers/pico-infonesPlus?tab=readme-ov-file#gamecontroller-support) for more info.

***

## About two player games

The emulator supports two player games using two NES controllers or an USB gamecontroller and a NES controller.

> [!NOTE]
> You cannot use two USB controllers for two player games.
> At the moment only one USB controller is recognized by the driver. In this case the USB controller is always player 1. Player 2 must be a NES controller.


| | Player 1 | Player 2 |
| --- | -------- | -------- |
| USB controller connected | USB | NES port 1 or NES port 2 |
| No usb controller connected | NES port 1| NES port 2 |

***

## Menu Usage
Gamepad buttons:
- UP/DOWN: Next/previous item in the menu.
- LEFT/RIGHT: next/previous page.
- A (Circle): Open folder/flash and start game.
- B (X): Back to parent folder.
- START: Starts game currently loaded in flash.

***

## Emulator (in game)
Gamepad buttons:
- SELECT + START: Resets back to the SD Card menu. Game saves are saved to the SD card.
- SELECT + UP/SELECT + DOWN: switches screen modes.
- SELECT + A/B: toggle rapid-fire.
- START + A : Toggle framerate display.

***

## Building from source

When using Visual Studio code, make sure to build in Release or RelWithDbinfo mode, as the emulator is too slow in the other modes.

Build shell scripts are available:

- build.sh : Builds .uf2 for the Pimoroni DV Deno Base
- build_alternate.sh: For the PCB or breadboard variant
- build_feather_dvi.sh: For the Adafruit feather
- build_ws_rp2040_pizero.sh: For the Wavehare device

Make sure they are executable by running `chmod +x build*.sh` in the terminal.

The _debug.sh scripts can be use to create a debug build for each system.

***

## Things to do (if possible):

- [ ] Improve performance
- [ ] Save states

