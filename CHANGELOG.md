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

# v0.17 release notes
- Releases now built with SDK 2.1.1
- Support added for Adafruit Metro RP2350 board. See README for more info. No RISCV support yet.
- Switched to SD card driver pico_fatfs from https://github.com/elehobica/pico_fatfs. This is required for the Adafruit Metro RP2350. The Pimoroni Pico DV does not work with this updated version and still needs the old version. (see [https://github.com/elehobica/pico_fatfs/issues/7#issuecomment-2817953143](https://github.com/elehobica/pico_fatfs/issues/7#issuecomment-2817953143) ) Therefore, the old version is still included in the repository. (pico_shared/drivers/pio_fatfs) 
    This is configured in CMakeLists.txt file by setting USE_OLD_SDDRIVER to 1.
- Besides FAT32, SD cards can now also be formatted as exFAT.
- Nes controller PIO code updated by [@ManCloud](https://github.com/ManCloud). This fixes the NES controller issues on the Waveshare RP2040 - PiZero board. [#8](https://github.com/fhoedemakers/pico_shared/issues/8)

## Fixes
- Fixed Pico 2 W: Led blinking causes screen flicker and ioctl timeouts [#2](https://github.com/fhoedemakers/pico_shared/issues/2). Solved with in SDK 2.1.1
- WII classic controller: i2c bus instance (i2c0 / i2c1) not hardcoded anymore but configurable via CMakeLists.txt. 


All changes are in the pico_shared submodule. When building from source, make sure you do a **git submodule update --init** from within the source folder to get the latest pico_shared module.
