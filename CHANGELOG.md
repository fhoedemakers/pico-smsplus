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

# v0.18 Release Notes

- Save game functionality has been added for SMS games that support it. To ensure game data is properly saved to the SD card, **you must return to the menu by pressing Select + Start.**
Save files are stored in the /SAVES directory with a .SAV extension.
When you launch the game again, the corresponding save file will be loaded automatically.


## Fixes
- none


