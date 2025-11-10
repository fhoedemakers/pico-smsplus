# CHANGELOG

# General Info

[Binaries for each configuration and PCB design are at the end of this page](#downloads___).


[See setup section in in Pico-infoNesPlus readme how to install and wire up](https://github.com/fhoedemakers/pico-infonesPlus#pico-setup)


# v0.22 Rekease Notes

- Settings are saved to /settings_sms.dat instead of /settings.dat. This allows to have separate settings files for different emulators (e.g. pico-infonesPlus and pico-peanutGB etc.).
- Added a settings menu. Press SELECT in the main menu to open it and change settings instead of using in‑game button combos.
- Switched to Fatfs R0.16.

# v0.21 Release Notes

- In-game hotkeys are now consistent with the other emulators. 
- On RP2350 based boards: Themed borders/bezels are shown in-game. For this you need the latest [metadata pack](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/SMSPlusMetadata.zip) from the releases section. Download and unzip it's contents to the root of the SD card.
- Support for [Retro-bit 8 button Genesis-USB](https://www.retro-bit.com/controllers/genesis/#usb). 

# v0.20 Release Notes

- Added support for [Adafruit Fruit Jam](https://www.adafruit.com/product/6200):  
  - Uses HSTX for video output.  
  - Audio is not supported over HSTX — connect speakers via the **audio jack** or the **4–8 Ω speaker connector**.  
  - Audio is simultaneousy played through speaker and jack. Speaker audio can be muted with **Button 1**.  
  - Controller options:  
    - **USB gamepad** on USB 1.  
    - **Wii Classic controller** via [Adafruit Wii Nunchuck Adapter](https://www.adafruit.com/product/4836) on the STEMMA QT port.  
  - Two-player mode:  
    - Player 1: USB gamepad (USB 1).  
    - Player 2: Wii Classic controller.  
    - Dual USB (USB 1 + USB 2) multiplayer is **not yet supported**.  
  - Scanlines can be toggled with **SELECT + UP**.  
  - NeoPixel leds act as a VU meter. Can be toggled on or of via Button2 on the Fruit Jam, or SELECT + RIGHT on the controller.

- Added support for [Waveshare RP2350-PiZero](https://www.waveshare.com/rp2350-pizero.htm):  
  - Gamepad must be connected via the **PIO USB port**.  
  - The built-in USB port is now dedicated to **power and firmware flashing**, removing the need for a USB-Y cable.  
  - Optional: when you solder the optional PSRAM chip on the board, the emulator will make use of it. Roms will be loaded much faster using PSRAM.

- Added support for Waveshare RP2350-USBA with PCB. More info and build guide at: https://www.instructables.com/PicoNES-RaspberryPi-Pico-Based-NES-Emulator/
- Added support for [Spotpear HDMI](https://spotpear.com/index/product/detail/id/1207.html) board.

- **RP2350 Only** Framebuffer implemented in SRAM. This eliminates the red flicker during slow operations, such as SD card I/O.

- **Cover art and metadata support**:  
  - Download pack [here](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/SMSPlusMetadata.zip).  
  - Extract the zip contents to the **root of the SD card**.  
  - In the menu:  
    - Highlight a game and press **START** → show cover art and metadata.  
    - Press **SELECT** → show full game description.  
    - Press **B** → return to menu.  
    - Press **START** or **A** → start the game.

- Screensaver
  - Block screensaver, which is shown when no metadata is available, is replaced by static floating image.

Huge thanks to [Gavin Knight](https://github.com/DynaMight1124) for providing the metadata and images as well as testing the different builds!

## Fixes

- Audio improvements.
- misc fixes.

# v0.19 Release Notes
- PSRAM will be used if detected. (RP2350 only, default pin 47). ROMs load from the SD card into PSRAM instead of flash. This speeds up loading because the board no longer has to reboot to copy the ROM from the SD card to flash. Based on https://github.com/AndrewCapon/PicoPlusPsram Boards with PSRAM are the [Adafruit Metro RP2350 with PSRAM](https://www.adafruit.com/product/6267) and [Pimoroni Pico Plus 2](https://shop.pimoroni.com/products/pimoroni-pico-plus-2?variant=42092668289107).
- Added -s option to bld.sh to allow an alternative GPIO pin for PSRAM chip select.
- Added support for [Pimoroni Pico Plus 2](https://shop.pimoroni.com/products/pimoroni-pico-plus-2?variant=42092668289107). (Uses hardware configuration 2, which is also used for breadboard and PCB). No extra binary needed.
- In some configurations, a second USB port can be added. This port can be used to connect a gamepad. The built-in usb port will be used for power and flashing the firmware. With this there is no need to use a USB-Y cable anymore. For more info, see [pio_usb.md](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/pio_usb.md). You have to build the firmware from source to enable this feature. The pre-built binaries do not support this.
- Enable I2S audio on the Pimoroni Pico DV Demo Base. This allows audio output through external speakers connected to the line-out jack of the Pimoroni Pico DV Demo Base. You can toggle audio output to this jack with SELECT + LEFT. Thanks to [Layer812](https://github.com/Layer812) for testing and providing feedback.
- Added second PCB design for use with Waveshare [RP2040-Zero](https://www.waveshare.com/rp2040-zero.htm) or [RP2350-Zero](https://www.waveshare.com/rp2350-zero.htm) mini development board. The PCB is designed to fit in a 3D-printed case. PCB and Case design by [@DynaMight1124](https://github.com/DynaMight1124).
Based around cheaper but harder to solder components for those that fancy a bigger challenge. It also allows the design to be smaller.
- Added new configuration to BoardConfigs.cmake and bld.sh to support the new configuration for this PCB.

| | |
| ---- | ---- |
| ![NESMiniPCB](https://github.com/user-attachments/assets/64696de1-2896-4a9c-94e9-692f125c55b6) | ![NESMiniCase](https://github.com/user-attachments/assets/a68f31ff-529f-49fb-9ec4-f3512c8e9e38) |


> [!NOTE]
> Some low USB speed devices like keyboards do not work properly when connected to the second USB port. See https://github.com/sekigon-gonnoc/Pico-PIO-USB/issues/18

## Fixes
- Fix crash in my_chdir that may occur on RP2040 boards.

# v0.18 Release Notes

- Save game functionality has been added for [SMS games that support it](https://consolemods.org/wiki/Master_System:List_of_Master_System_Games_with_Save_Batteries). To ensure game data is properly saved to the SD card, **you must return to the menu by pressing Select + Start.**
Save files are stored in the /SAVES directory with a .SAV extension.
When you launch the game again, the corresponding save file will be loaded automatically.


## Fixes
- none


<a name="downloads___"></a>
## Downloads by configuration

Binaries for each configuration are listed below. Binaries for Pico(2) also work for Pico(2)-w. No blinking led however on the -w boards.
For some configurations risc-v binaries are available. It is recommended however to use the arm binaries. 

### Standalone boards

| Board | Binary | Readme | |
|:--|:--|:--|:--|
| Adafruit Metro RP2350 | [picosmsPlus_AdafruitMetroRP2350_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_AdafruitMetroRP2350_arm.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#adafruit-metro-rp2350) | |
| Adafruit Fruit Jam | [picosmsPlus_AdafruitFruitJam_arm_piousb.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_AdafruitFruitJam_arm_piousb.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#adafruit-fruit-jam)| |
| Waveshare RP2040-PiZero | [picosmsPlus_WaveShareRP2040PiZero_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_WaveShareRP2040PiZero_arm.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#waveshare-rp2040rp2350-pizero-development-board)| [3-D Printed case](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#3d-printed-case-for-rp2040rp2350-pizero) |
| Waveshare RP2350-PiZero | [picosmsPlus_WaveShareRP2350PiZero_arm_piousb.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_WaveShareRP2350PiZero_arm_piousb.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#waveshare-rp2040rp2350-pizero-development-board)| [3-D Printed case](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#3d-printed-case-for-rp2040rp2350-pizero) |

### Breadboard

| Board | Binary | Readme |
|:--|:--|:--|
| Pico| [picosmsPlus_AdafruitDVISD_pico_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_AdafruitDVISD_pico_arm.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#raspberry-pi-pico-or-pico-2-setup-with-adafruit-hardware-and-breadboard) |
| Pico W | [picosmsPlus_AdafruitDVISD_pico_w_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_AdafruitDVISD_pico_w_arm.uf2) | [Readme](README.md#raspberry-pi-pico-or-pico-2-setup-with-adafruit-hardware-and-breadboard) |
| Pico 2 | [picosmsPlus_AdafruitDVISD_pico2_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_AdafruitDVISD_pico2_arm.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#raspberry-pi-pico-or-pico-2-setup-with-adafruit-hardware-and-breadboard) |
| Pico 2 W | [picosmsPlus_AdafruitDVISD_pico2_w_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_AdafruitDVISD_pico2_w_arm.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#raspberry-pi-pico-or-pico-2-setup-with-adafruit-hardware-and-breadboard) |
| Adafruit feather rp2040 DVI | [picosmsPlus_AdafruitFeatherDVI_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_AdafruitFeatherDVI_arm.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#adafruit-feather-rp2040-with-dvi-hdmi-output-port-setup) |
| Pimoroni Pico Plus 2 | [picosmsPlus_AdafruitDVISD_pico2_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_AdafruitDVISD_pico2_arm.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#raspberry-pi-pico-or-pico-2-setup-with-adafruit-hardware-and-breadboard) |


### PCB Pico/Pico2

| Board | Binary | Readme |
|:--|:--|:--|
| Pico| [picosmsPlus_AdafruitDVISD_pico_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_AdafruitDVISD_pico_arm.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#pcb-with-raspberry-pi-pico-or-pico-2) |
| Pico W| [picosmsPlus_AdafruitDVISD_pico_w_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_AdafruitDVISD_pico_w_arm.uf2) | [Readme](README.md#pcb-with-raspberry-pi-pico-or-pico-2) |
| Pico 2 | [picosmsPlus_AdafruitDVISD_pico2_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_AdafruitDVISD_pico2_arm.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#pcb-with-raspberry-pi-pico-or-pico-2) |
| Pico 2 W | [picosmsPlus_AdafruitDVISD_pico2_w_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_AdafruitDVISD_pico2_w_arm.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#pcb-with-raspberry-pi-pico-or-pico-2) |

PCB [pico_nesPCB_v2.1.zip](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/pico_nesPCB_v2.1.zip)

3D-printed case designs for PCB:

[https://www.thingiverse.com/thing:6689537](https://www.thingiverse.com/thing:6689537). 
For the latest two player PCB 2.0, you need:

- Top_v2.0_with_Bootsel_Button.stl. This allows for software upgrades without removing the cover. (*)
- Base_v2.0.stl
- Power_Switch.stl.
(*) in case you don't want to access the bootsel button on the Pico, you can choose Top_v2.0.stl

### PCB WS2XX0-Zero (PCB required)

| Board | Binary | Readme |
|:--|:--|:--|
| Waveshare RP2040-Zero | [picosmsPlus_WaveShareRP2040ZeroWithPCB_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_WaveShareRP2040ZeroWithPCB_arm.uf2) | [Readme](README.md#pcb-with-waveshare-rp2040rp2350-zero) |
| Waveshare RP2350-Zero | [picosmsPlus_WaveShareRP2350ZeroWithPCB_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsplus_WaveShareRP2350ZeroWithPCB_arm.uf2) | [Readme](README.md#pcb-with-waveshare-rp2040rp2350-zero) |

PCB: [Gerber_PicoNES_Mini_PCB_v2.0.zip](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/Gerber_PicoNES_Mini_PCB_v2.0.zip)

3D-printed case designs for PCB WS2XX0-Zero:
[https://www.thingiverse.com/thing:7041536](https://www.thingiverse.com/thing:7041536)

### PCB Waveshare RP2350-USBA with PCB
[Binary](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_WaveShare2350USBA_arm_piousb.uf2)

PCB: [Gerber_PicoNES_Micro_v1.2.zip](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/Gerber_PicoNES_Micro_v1.2.zip)

[Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#pcb-with-waveshare-rp2350-usb-a)

[Build guide](https://www.instructables.com/PicoNES-RaspberryPi-Pico-Based-NES-Emulator/)


### Pimoroni Pico DV

| Board | Binary | Readme |
|:--|:--| :--|
| Pico/Pico w | [picosmsPlus_PimoroniDVI_pico_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_PimoroniDVI_pico_arm.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#raspberry-pi-pico-or-pico-2-setup-for-pimoroni-pico-dv-demo-base) |
| Pico 2/Pico 2 w | [picosmsPlus_PimoroniDVI_pico2_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_PimoroniDVI_pico2_arm.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#raspberry-pi-pico-or-pico-2-setup-for-pimoroni-pico-dv-demo-base) |
| Pimoroni Pico Plus 2 | [picosmsPlus_PimoroniDVI_pico2_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_PimoroniDVI_pico2_arm.uf2) | [Readme](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/README.md#raspberry-pi-pico-or-pico-2-setup-for-pimoroni-pico-dv-demo-base) |

> [!NOTE]
> On Pico W and Pico2 W, the CYW43 driver (used only for blinking the onboard LED) causes a DMA conflict with I2S audio on the Pimoroni Pico DV Demo Base, leading to emulator lock-ups. For now, no Pico W or Pico2 W binaries are provided; please use the Pico or Pico2 binaries instead. (#132)


### Other downloads

- Metadata: [SMSPlusMetadata.zip](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/SMSPlusMetadata.zip)


Extract the zip file to the root folder of the SD card. Select a game in the menu and press START to show more information and box art. Works for most official released games. Screensaver shows floating random cover art. Works for RP2040 and RP2350.
Extra on RP2350: when a game is loaded a themed bezel/border is shown.


























