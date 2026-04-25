# CHANGELOG

> **HSTX replaces PicoDVI** on more boards, **HSTX now has picture and sound over HDMI**, in-game game reset, automatic headphone detection on Fruit Jam, and a save-state crash fix.

# General Info


[Binaries for each configuration and PCB design are at the end of this page](#downloads___).


[See setup section in in Pico-infoNesPlus readme how to install and wire up](https://github.com/fhoedemakers/pico-infonesPlus#pico-setup)

# v0.25 Release notes

This release replaces PicoDVI with HSTX on more boards, HSTX now also carries audio over HDMI,
adds smoother on-screen motion, a few new in-game conveniences, and a
handful of fixes that make the emulator more reliable in everyday use.

A huge thank you to [@fliperama86](https://github.com/fliperama86) for the
excellent [pico_hdmi](https://github.com/fliperama86/pico_hdmi) driver that
made the new HDMI output possible, and for all the help along the way.

## What's new

### HSTX Video and sound over HDMI

On the technical side, several RP2350 board configurations have switched
from the **PicoDVI** software-driven video output to **HSTX**, the
RP2350's dedicated High-Speed Serial Transmit hardware (GPIO 12 – 19).
HSTX has been used for video on some boards before, but in this release
it also carries **audio embedded in the HDMI stream** for the first
time — that's the new capability HSTX gains here. (PicoDVI has always
been able to embed audio; HSTX is just catching up on that front while
offloading the work from the CPU to dedicated hardware.)

In practice, on these boards picture and sound now travel together over a
single HDMI cable — no separate audio jack needed:

- Adafruit Fruit Jam
- Murmulator M2

These RP2350 boards have also been switched from PicoDVI to HSTX. They
keep using a separate audio output for now, but picture quality should
look the same and the change frees up CPU cycles for future improvements:

- [Breadboard build](https://github.com/fhoedemakers/pico-infonesPlus?tab=readme-ov-file#raspberry-pi-pico-or-pico-2-setup-with-adafruit-hardware-and-breadboard)
- [PCB build](https://github.com/fhoedemakers/pico-infonesPlus?tab=readme-ov-file#pcb-with-raspberry-pi-pico-or-pico-2)
- [Adafruit Metro RP2350](https://github.com/fhoedemakers/pico-infonesPlus?tab=readme-ov-file#adafruit-metro-rp2350)

All other boards continue to use PicoDVI and work as before.

### Smoother gameplay

Frame timing now follows the display's own refresh signal, which removes
small stutters and gives the picture a more consistent, smooth feel.

### New options and conveniences

- **Reset the running game** from the in-game menu — no need to power-cycle
  the device to restart.
- **Enter flashing mode from the settings menu**, so you can update the
  firmware without having to unplug the device and hold the BOOTSEL button.
- **Scanline effect for HDMI boards** — turn on the classic CRT scanline
  look from the settings menu when running on an HDMI-capable board.

### Adafruit Fruit Jam

- **Automatic headphone detection.** Plug headphones in and the built-in
  speaker mutes itself; unplug them and the speaker comes back. The old
  manual mute setting and pushbutton-1 mute shortcut have been removed —
  they are no longer needed.

## Fixes

- **Save states work again.** A bug introduced in v0.24 could crash the
  emulator when loading a saved game. This is now fixed. Note: save state
  files created by earlier v0.25 development builds are not compatible
  with this fix and will need to be re-created.
- **Picture-loss recovery.** On the new HSTX output when set to
  video-only (DVI) mode, the monitor could occasionally lose the picture.
  The emulator now detects this and automatically restores the signal
  without needing a restart. (Not observed in full HDMI mode, but the
  same safety net is enabled there too just in case.)
- **Screen cropping fix.** A small visual glitch where part of the game
  picture was being cut off incorrectly has been corrected.

## Credits

Updated to acknowledge new contributors — see the splash screen on
startup.

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



























