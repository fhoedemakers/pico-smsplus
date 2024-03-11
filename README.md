# pico-smsplus

This software is a port of [SmsPlus](https://segaretro.org/SMS_Plus), a Sega Master System emulator, for the Raspberry Pi Pico, and supports video and audio output over HDMI.
The code for HDMI output is based on [Shuichi Takano's Pico-InfoNes project](https://github.com/shuichitakano/pico-infones). 

## ROM
The ROM should be placed in some way from 0x10080000, and can be easily transferred using [picotool](https://github.com/raspberrypi/picotool).
```
picotool load foo.sms -t bin -o 0x10080000
```

## Video
Click on image below to see a demo video.

[![Video](https://img.youtube.com/vi/7Hi5HdAcsqc/0.jpg)](https://www.youtube.com/watch?v=7Hi5HdAcsqc)

## Work in progress

This is a work in progress, and there are still some issues to be fixed. The video is not as smooth as it should be. The emulator is also not very fast, and some games may not run at full speed or att all.

Things to do (if possible):

- [ ] Improve video rendering
- [ ] Implement audio
- [ ] Improve performance
- [ ] Controller support
- [ ] SD card support.
- [ ] Menu to select and play ROMs from the SD card.
- [ ] Save states
- [ ] Game Gear support
