# pico-smsplus

This software is a port of [SmsPlus](https://segaretro.org/SMS_Plus), a Sega Master System emulator, for the Raspberry Pi Pico, and supports video and audio output over HDMI.
The code for HDMI output is based on [Shuichi Takano's Pico-InfoNes project](https://github.com/shuichitakano/pico-infones) which in turn is based on [PicoDVI](https://github.com/Wren6991/PicoDVI).

## ROM
The ROM should be placed in some way from 0x10080000, and can be easily transferred using [picotool](https://github.com/raspberrypi/picotool).
```
picotool load foo.sms -t bin -o 0x10080000
```

## Video
Click on image below to see a demo video.

[![Video](https://img.youtube.com/vi/__E8h2Ay3g8/0.jpg)](https://www.youtube.com/watch?v=__E8h2Ay3g8)

## Work in progress

This is a work in progress, and there are still some issues to be fixed. The video is not as smooth as it should be. The emulator is also not very fast, and some games may not run at full speed or att all.

Things to do (if possible):

- [ ] Improve video rendering
- [x] Implement audio (but needs improvement)
- [ ] Improve performance
- [x] USB Controller support
- [ ] Nes controller support
- [ ] WII-controller support
- [ ] SD card support.
- [ ] Menu to select and play ROMs from the SD card.
- [ ] Save states
- [ ] Game Gear support

## Building
I use Visual Studio Code on a Raspberry PI 400. Make sure to build in Release or RelWithDbinfo mode, as the emulator is too slow in the other modes.

## Supported USB controller
The following controllers are supported.

- BUFFALO BGC-FC801
- SONY DUALSHOCK 4
- SONY DualSense

more to come.
