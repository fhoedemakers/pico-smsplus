# CHANGELOG

# General Info

Binaries for each configuration and PCB design are at the end of this page.

- For Raspberry Pi Pico (RP2040): Download the .uf2 files that start with pico_picosmsPlus.
- For Raspberry Pi Pico W (RP2040 with WiFi): Download the .uf2 files that start with pico_w_picosmsPlus. You can also use the regular pico_picosmsPlus files if you don’t mind the WiFi LED blinking.
- For Raspberry Pi Pico 2 (RP2350): Download the .uf2 files that start with pico2_picosmsPlus (for ARM) or pico2_riscv_picosmsPlus (for RISC-V).
- For Raspberry Pi Pico 2 W (RP2350 with WiFi): Download the .uf2 files that start with pico2_w_picosmsPlus (for ARM) or pico2_w_riscv_picosmsPlus (for RISC-V). You can also use the non-W files if you don’t mind the WiFi LED blinking.

[Click here for tested configurations](testresults.md).

[See setup section in readme how to install and wire up](https://github.com/fhoedemakers/pico-infonesPlus#pico-setup)

# 3D-printed case designs for custom PCBs

## Raspberry Pi Pico and Pico 2 PCB

[https://www.thingiverse.com/thing:6689537](https://www.thingiverse.com/thing:6689537). 
For the latest two player PCB 2.0, you need:

- Top_v2.0_with_Bootsel_Button.stl. This allows for software upgrades without removing the cover. (*)
- Base_v2.0.stl
- Power_Switch.stl.

(*) in case you don't want to access the bootsel button on the Pico, you can choose Top_v2.0.stl

## Waveshare RP2040-Zero and RP2350-Zero PCB

[https://www.thingiverse.com/thing:7041536](https://www.thingiverse.com/thing:7041536)

## Waveshare RP2040-PiZero

[https://www.thingiverse.com/thing:6758682](https://www.thingiverse.com/thing:6758682)


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


