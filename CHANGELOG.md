# CHANGELOG

# General Info

Binaries are at the end of this page.

- For Raspberry Pi Pico (RP2040) you need to download the .uf2 files starting with pico_picosmsPlus.
- For Raspberry Pi Pico w (rp2040) you can download the .uf2 files starting with pico_w_picosmsPlus. Although you can also use the pico_picosmsPlus binaries on the Pico w if you don't mind the blinking led.
- For Raspberry Pi Pico 2 (RP2350) you need to download the .uf2 files starting with pico2_picosmsPlus for ARM or pico2_riscv_picosmsPlus  for Risc-V. 
- For Raspberry Pi Pico 2 w (RP2350) you can download the .uf2 files starting with pico2_w_picosmsPlus for ARM or pico2_w_riscv_picosmsPlus for Risc-V Although you can also use the pico2_picosmsPlus binaries on the Pico w if you don't mind the blinking led.
[See readme section how to install and wire up](https://github.com/fhoedemakers/pico-smsplus#pico-smsplus). 

For more detailed instructions how to setup specific configurations, see the [Pico-InfonesPlus sister project](https://github.com/fhoedemakers/pico-infonesPlus).


3D-printed case design for PCB: [https://www.thingiverse.com/thing:6689537](https://www.thingiverse.com/thing:6689537). 
For the latest two player PCB 2.0, you need:

- Top_v2.0_with_Bootsel_Button.stl. This allows for software upgrades without removing the cover. (*)
- Base_v2.0.stl
- Power_Switch.stl.

(*) in case you don't want to access the bootsel button on the Pico, you can choose Top_v2.0.stl

3D-printed case design for Waveshare RP2040-PiZero: [https://www.thingiverse.com/thing:6758682](https://www.thingiverse.com/thing:6758682)

# v0.19 Release Notes (to be released)
- PSRAM will be used if detected. (RP2350 only, default pin 47). ROMs load from the SD card into PSRAM instead of flash. This speeds up loading because the board no longer has to reboot to copy the ROM from the SD card to flash. Based on https://github.com/AndrewCapon/PicoPlusPsram Boards with PSRAM are the [Adafruit Metro RP2350 with PSRAM](https://www.adafruit.com/product/6267) and [Pimoroni Pico Plus 2](https://shop.pimoroni.com/products/pimoroni-pico-plus-2?variant=42092668289107).
- Added -s option to bld.sh to allow an alternative GPIO pin for PSRAM chip select.
- Added support for [Pimoroni Pico Plus 2](https://shop.pimoroni.com/products/pimoroni-pico-plus-2?variant=42092668289107). (Uses hardware configuration 2, which is also used for breadboard and PCB). No extra binary needed.
- In some configurations, a second USB port can be added. This port can be used to connect a gamepad. The built-in usb port will be used for power and flashing the firmware. With this there is no need to use a USB-Y cable anymore. For more info, see [pio_usb.md](https://github.com/fhoedemakers/pico-infonesPlus/blob/main/pio_usb.md). You have to build the firmware from source to enable this feature. The pre-built binaries do not support this.
- Enable I2S audio on the Pimoroni Pico DV Demo Base. This allows audio output through external speakers connected to the line-out jack of the Pimoroni Pico DV Demo Base. You can toggle audio output to this jack with SELECT + LEFT. Thanks to [Layer812](https://github.com/Layer812) for testing and providing feedback.

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


