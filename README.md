
# pico-smsplus, A Sega Master System and Game Gear emulator for RP2040/RP2350 based boards.

This software is a port of [SmsPlus](https://segaretro.org/SMS_Plus), a Sega Master System and Game Gear emulator for RP2040/RP2350 based microcontroller boards like the RaspberryPi Pico and Pico 2. Sound and video are ouput over HDMI.
The code for HDMI output is based on [Shuichi Takano's Pico-InfoNes project](https://github.com/shuichitakano/pico-infones) which in turn is based on [PicoDVI](https://github.com/Wren6991/PicoDVI).

Create a FAT32 or exFAT formatted SD card and copy your Master System (.sms) and/or Game Gear (.gg) roms and optional [metadata](#using-metadata) on to it.
You can organize the roms in directories. A menu is displayed on which you can select the rom to play.

Supports two controllers for two player Master System games. [See "about two player games" below for specifics and limitations](#about-two-player-games) 

See the [releases](https://github.com/fhoedemakers/pico-smsplus/releases/latest) page for the supported RP2040/RP2350 boards.

***

## Video
Click on image below to see a demo video.

[![Video](https://img.youtube.com/vi/__E8h2Ay3g8/0.jpg)](https://www.youtube.com/watch?v=__E8h2Ay3g8)

***
## RP2040 notes

**The emulator works very well on the Pico 2/RP2350. On the Pico/RP2040, some games may not run at full speed or at all. (like afterburner).**

>[!WARNING] 
> On Pico/RP2040, some games show red flashing between screens. This can be occasionally or severe depending on the game. If you are sensitive for this, or experience health issues while playing those games, please stop playing immediately.

***

## Save Game Support for SMS Games

[For Sega Master System (SMS) games that support it](https://consolemods.org/wiki/Master_System:List_of_Master_System_Games_with_Save_Batteries), progress can be saved to SD card.

### How It Works

- **Saving**: To ensure game data is written to the SD card, you must return to the main menu by pressing **Select + Start**.
- **Save Location**: Save files are stored in the `/SAVES` directory with a `.SAV` extension.
- **Auto-Loading**: When you start a game, its corresponding `.SAV` file will be loaded automatically (if available).

>[!NOTE]
>Make sure to return to the menu before powering off, or the save data may be lost.

***

## System requirements and setup - What do yo need?

The binary specific for your config can be downloaded from the [releases](https://github.com/fhoedemakers/pico-smsplus/releases/latest) page.

You need a FAT32 or exFAT formatted SD card to put your .sms and .gg roms on.

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


- [Adafruit Fruit Jam](https://www.adafruit.com/product/6200)
- [Adafruit Metro RP2350](https://www.adafruit.com/product/6003) or [Adafruit Metro RP2350 with PSRAM](https://www.adafruit.com/product/6267)
- [Pimoroni Pico Plus 2](https://shop.pimoroni.com/products/pimoroni-pico-plus-2?variant=42092668289107)
  Use the breadboard config or Pimoroni Pico DV Demo base. This board does not fit the PCB because of the SP/CE connector on back of the board.
  The PSRAM on the board is used in stead of flash to load the roms from SD.

## Waveshare RP2040 & RP2350 Zero

An additional PCB design for Waveshare RP2040 & RP2350 Zero including case design by DynaMight1124 based around cheaper but harder to solder components for those that fancy a bigger challenge. It also allows the design to be smaller.

***

## Supported USB controllers
The following controllers are supported.

- BUFFALO BGC-FC801
- SONY DUALSHOCK 4
- SONY DualSense
- Xinput type controllers (Xbox)
- [Retro-bit 8 button Genesis-USB](https://www.retro-bit.com/controllers/genesis/#usb). 
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

# Gamepad and keyboard usage
Below the button mapping for different controllers. You can also use a USB-keyboard.
|     | (S)NES | Genesis | XInput | Dual Shock/Sense |
| --- | ------ | ------- | ------ | ---------------- |
| Button1 | B  |    A    |   A    |    X             |
| Button2 | A  |    B    |   B    |   Circle         |
| Select  | select | Mode or C | Select | Select     |

## In menu
Gamepad buttons:
- UP/DOWN: Next/previous item in the menu.
- LEFT/RIGHT: next/previous page.
- Button2 : Open folder/flash and start game.
- Button1 : Back to parent folder.
- START: Show metadata and box art (when available). 

The colors in the menu can be changed and saved:
  - Select + Up/Down changes the foreground color.
  - Select + Left/Right changes the background color.
  - Select + Button2 saves the colors. Screen will flicker when saved.
  - Select + Button1 resets the colors to default. (Black on white)


## Emulator (in game)
Gamepad buttons:
- SELECT + START, Xbox button: Resets back to the SD Card menu. Game saves are saved to the SD card.
- SELECT + UP/SELECT + DOWN: switches screen modes.
- START + Button2 : Toggle framerate display
- **Pimoroni Pico DV Demo Base only**: SELECT + LEFT: Switch audio output to the connected speakers on the line-out jack of the Pimoroni Pico DV Demo Base. The speaker setting will be remembered when the emulator is restarted.
- **Fruit Jam Only** 
  - pushbutton 1 (on board): Mute audio of built-in speaker. Audio is still outputted to the audio jack.
  - SELECT + UP: Toggle scanlines. 
  - pushbutton 2 (on board) or SELECT + RIGHT: Toggles the VU meter on or off. (NeoPixel LEDs light up in sync with the music rhythm)
- **Genesis Mini Controller**: When using a Genesis Mini 3 button controller, press C for SELECT. On the 8-button Genesis controllers, MODE acts as SELECT.
- **USB-keyboard**: When using an USB-Keyboard
  - Cursor keys: up, down, left, right
  - A: Select
  - S: Start
  - Z: Button2
  - X: Button1

## Using metadata.

<img alt="Screenshot 2025-10-19 14-57-45" src="https://github.com/user-attachments/assets/da91016f-093b-4b96-8d8e-5a0f37cf2506" />

Download the metadata pack from the [releases page](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/SMSPlusMetadata.zip) It contains box art, game info and themed borders/bezels for many games. The metadata is used in the menu to show box art and game info when a rom is selected.  When the screensaver is started, random box art is shown. Extra on RP2350 boards: When in-game, themed borders/bezels are shown around the game screen.

- Download pack [here](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/SMSPlusMetadata.zip).  
  - Extract the zip contents to the **root of the SD card**.  
  - In the menu:  
    - Highlight a game and press **START** → show cover art and metadata.  
    - Press **SELECT** → show full game description.  
    - Press **Button1** → return to menu.  
    - Press **START** or **Button2** → start the game.
  - **RP2350-only**: In-game: A themed bezel is shown:

| Master System | Game Gear | 
| ------------- | --------  |
| <img width="320" alt="Screenshot 2025-10-19 14-57-33" src="https://github.com/user-attachments/assets/19db1e45-0cda-442f-ac1a-3b54b7ccfa84" /> | <img width="320" alt="Screenshot 2025-10-21 17-04-42" src="https://github.com/user-attachments/assets/8f9bed95-fea8-4ce8-aa1c-edcbeacb1945" /> |


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


