# CHANGELOG

# General Info

[Binaries for each configuration and PCB design are at the end of this page](#downloads___).


[See setup section in in Pico-infoNesPlus readme how to install and wire up](https://github.com/fhoedemakers/pico-infonesPlus#pico-setup)

# v0.25 Release notes

For the boards that use HSTX in stead of PicoDVI: HDMI audio is now supported via the new HSTX video driver. Huge thanks to [@fliperama86](https://github.com/fliperama86) for the awesome [pico_hdmi](https://github.com/fliperama86/pico_hdmi) driver that made this possible and for helping out.

- Adafruit Fruit Jam.
- Murmulator M2. 

Other RP2350 configurations that now use HSTX (GPIO 12 - 19) in stead of PicoDVI:

- [Breadboard](https://github.com/fhoedemakers/pico-infonesPlus?tab=readme-ov-file#raspberry-pi-pico-or-pico-2-setup-with-adafruit-hardware-and-breadboard)
- [PCB](https://github.com/fhoedemakers/pico-infonesPlus?tab=readme-ov-file#pcb-with-raspberry-pi-pico-or-pico-2)
- [Adafruit Metro RP2350](https://github.com/fhoedemakers/pico-infonesPlus?tab=readme-ov-file#adafruit-metro-rp2350)
  
All the other boards still use PicoDVI.

To enable audio over hdmi, make sure external audio is disabled in the settings menu.

- Added option in settings menu to enter bootsel mode for flashing firmware. 

## Fixes

- Fix for losing signal on monitors: Removed _not_in_flash_func predicates from emulator code on RP2350. (???)

# previous changes

See [HISTORY.md](https://github.com/fhoedemakers/pico-smsplus/blob/main/HISTORY.md)

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

### Murmulator M1

For more info about the Murmulator see this website: https://murmulator.ru/ and [#150](https://github.com/fhoedemakers/pico-infonesPlus/issues/150)

| Board | Binary |
|:--|:--|
| Pico/Pico w | [picosmsPlus_MurmulatorM1_pico_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_MurmulatorM1_pico_arm.uf2) |
| Pico 2/Pico 2 w | [picosmsPlus_MurmulatorM1_pico2_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_MurmulatorM1_pico2_arm.uf2) |

### Murmulator M2

For more info about the Murmulator see this website: https://murmulator.ru/ and [#150](https://github.com/fhoedemakers/pico-infonesPlus/issues/150)

| Board | Binary |
|:--|:--|
| Pico/Pico w | [picosmsPlus_MurmulatorM2_arm.uf2](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/picosmsPlus_MurmulatorM2_arm.uf2) |

### Other downloads

- Metadata: [SMSPlusMetadata.zip](https://github.com/fhoedemakers/pico-smsplus/releases/latest/download/SMSPlusMetadata.zip)


Extract the zip file to the root folder of the SD card. Select a game in the menu and press START to show more information and box art. Works for most official released games. Screensaver shows floating random cover art. Works for RP2040 and RP2350.
Extra on RP2350: when a game is loaded a themed bezel/border is shown.



























