/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/divider.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/interp.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "hardware/watchdog.h"
#include <hardware/sync.h>
#include <pico/multicore.h>
#include <hardware/flash.h>
#include <memory>
#include <math.h>
#include <dvi/dvi.h>
#include <util/dump_bin.h>
#include <util/exclusive_proc.h>
#include <util/work_meter.h>
#include <tusb.h>

#include "shared.h"
#include "mytypes.h"
#include "gamepad.h"
#include "menu.h"
// #include "nespad.h"
// #include "wiipad.h"
#ifdef __cplusplus

#include "ff.h"

#endif

#if LED_DISABLED == 0
const uint LED_PIN = LED_GPIO_PIN;
#endif

#ifndef DVICONFIG
// #define DVICONFIG dviConfig_PicoDVI
// #define DVICONFIG dviConfig_PicoDVISock
#define DVICONFIG dviConfig_PimoroniDemoDVSock
#endif

#define ERRORMESSAGESIZE 40
#define GAMESAVEDIR "/SAVES"
util::ExclusiveProc exclProc_;
std::unique_ptr<dvi::DVI> dvi_;

char *ErrorMessage;
bool isFatalError = false;
static FATFS fs;
char *romName;

static bool fps_enabled = false;
static uint32_t start_tick_us = 0;
static uint32_t fps = 0;

bool reset = false;

// 16 bit palette
const int __not_in_flash_func(SMSPalette)[64] = {
    0x000000, 0x550000, 0xAA0000, 0xFF0000, 0x000055, 0x550055, 0xAA0055, 0xFF0055,
    0x005500, 0x555500, 0xAA5500, 0xFF5500, 0x005555, 0x555555, 0xAA5555, 0xFF5555,
    0x00AA00, 0x55AA00, 0xAAAA00, 0xFFAA00, 0x00AA55, 0x55AA55, 0xAAAA55, 0xFFAA55,
    0x00FF00, 0x55FF00, 0xAAFF00, 0xFFFF00, 0x00FF55, 0x55FF55, 0xAAFF55, 0xFFFF55,
    0x0000AA, 0x5500AA, 0xAA00AA, 0xFF00AA, 0x0000FF, 0x5500FF, 0xAA00FF, 0xFF00FF,
    0x0055AA, 0x5555AA, 0xAA55AA, 0xFF55AA, 0x00FF55, 0x5555FF, 0xAA55FF, 0xFF55FF,
    0x00AAAA, 0x55AAAA, 0xAAAAAA, 0xFFAAAA, 0x00AAFF, 0x55AAFF, 0xAAAAFF, 0xFFAAFF,
    0x00FFAA, 0x55FFAA, 0xAAFFAA, 0xFFFFAA, 0x00FFFF, 0x55FFFF, 0xAAFFFF, 0xFFFFFF};

namespace
{
    constexpr uint32_t CPUFreqKHz = 252000;

    constexpr dvi::Config dviConfig_PicoDVI = {
        .pinTMDS = {10, 12, 14},
        .pinClock = 8,
        .invert = true,
    };
    // Breadboard with Adafruit compononents
    constexpr dvi::Config dviConfig_PicoDVISock = {
        .pinTMDS = {12, 18, 16},
        .pinClock = 14,
        .invert = false,
    };
    // Pimoroni Digital Video, SD Card & Audio Demo Board
    constexpr dvi::Config dviConfig_PimoroniDemoDVSock = {
        .pinTMDS = {8, 10, 12},
        .pinClock = 6,
        .invert = true,
    };
    // Adafruit Feather RP2040 DVI
    constexpr dvi::Config dviConfig_AdafruitFeatherDVI = {
        .pinTMDS = {18, 20, 22},
        .pinClock = 16,
        .invert = true,
    };

    enum class ScreenMode
    {
        SCANLINE_8_7,
        NOSCANLINE_8_7,
        SCANLINE_1_1,
        NOSCANLINE_1_1,
        MAX,
    };
    ScreenMode screenMode_{};

    bool scaleMode8_7_ = true;

    void applyScreenMode()
    {
        bool scanLine = false;

        switch (screenMode_)
        {
        case ScreenMode::SCANLINE_1_1:
            scaleMode8_7_ = false;
            scanLine = true;
            break;

        case ScreenMode::SCANLINE_8_7:
            scaleMode8_7_ = true;
            scanLine = true;
            break;

        case ScreenMode::NOSCANLINE_1_1:
            scaleMode8_7_ = false;
            scanLine = false;
            break;

        case ScreenMode::NOSCANLINE_8_7:
            scaleMode8_7_ = true;
            scanLine = false;
            break;
        }

        dvi_->setScanLine(scanLine);
    }
    void screenMode(int incr)
    {
        screenMode_ = static_cast<ScreenMode>((static_cast<int>(screenMode_) + incr) & 3);
        applyScreenMode();
    }
}

namespace
{
    dvi::DVI::LineBuffer *currentLineBuffer_{};
}

void __not_in_flash_func(drawWorkMeterUnit)(int timing,
                                            [[maybe_unused]] int span,
                                            uint32_t tag)
{
    if (timing >= 0 && timing < 640)
    {
        auto p = currentLineBuffer_->data();
        p[timing] = tag; // tag = color
    }
}

void __not_in_flash_func(drawWorkMeter)(int line)
{
    if (!currentLineBuffer_)
    {
        return;
    }

    memset(currentLineBuffer_->data(), 0, 64);
    memset(&currentLineBuffer_->data()[320 - 32], 0, 64);
    (*currentLineBuffer_)[160] = 0;
    if (line == 4)
    {
        for (int i = 1; i < 10; ++i)
        {
            (*currentLineBuffer_)[16 * i] = 31;
        }
    }

    constexpr uint32_t clocksPerLine = 800 * 10;
    constexpr uint32_t meterScale = 160 * 65536 / (clocksPerLine * 2);
    util::WorkMeterEnum(meterScale, 1, drawWorkMeterUnit);
}

bool initSDCard()
{
    FRESULT fr;
    TCHAR str[40];
    sleep_ms(1000);

    printf("Mounting SDcard");
    fr = f_mount(&fs, "", 1);
    if (fr != FR_OK)
    {
        snprintf(ErrorMessage, ERRORMESSAGESIZE, "SD card mount error: %d", fr);
        printf("\n%s\n", ErrorMessage);
        return false;
    }
    printf("\n");

    fr = f_chdir("/");
    if (fr != FR_OK)
    {
        snprintf(ErrorMessage, ERRORMESSAGESIZE, "Cannot change dir to / : %d", fr);
        printf("\n%s\n", ErrorMessage);
        return false;
    }
    // for f_getcwd to work, set
    //   #define FF_FS_RPATH		2
    // in drivers/fatfs/ffconf.h
    fr = f_getcwd(str, sizeof(str));
    if (fr != FR_OK)
    {
        snprintf(ErrorMessage, ERRORMESSAGESIZE, "Cannot get current dir: %d", fr);
        printf("\n%s\n", ErrorMessage);
        return false;
    }
    printf("Current directory: %s\n", str);
    printf("Creating directory %s\n", GAMESAVEDIR);
    fr = f_mkdir(GAMESAVEDIR);
    if (fr != FR_OK)
    {
        if (fr == FR_EXIST)
        {
            printf("Directory already exists.\n");
        }
        else
        {
            snprintf(ErrorMessage, ERRORMESSAGESIZE, "Cannot create dir %s: %d", GAMESAVEDIR, fr);
            printf("%s\n", ErrorMessage);
            return false;
        }
    }
    return true;
}
int sampleIndex = 0;
void in_ram(processaudio)(int offset)
{
    int samples = 4; // 735/192 = 3.828125 192*4=768 735/3=245
    if (offset == 0)
    {
        sampleIndex = 0;
    }
    else
    {
        sampleIndex += samples;
        if (sampleIndex >= 735)
        {
            return;
        }
    }
    short *p1 = snd.buffer[0] + sampleIndex;
    short *p2 = snd.buffer[1] + sampleIndex;
    while (samples)
    {
        auto &ring = dvi_->getAudioRingBuffer();
        auto n = std::min<int>(samples, ring.getWritableSize());
        if (!n)
        {
            return;
        }
        auto p = ring.getWritePointer();
        int ct = n;
        while (ct--)
        {
            int l = (*p1++ << 16) + *p2++;
            // works also : int l = (*p1++ + *p2++) / 2;
            int r = l;
            // int l = *wave1++;
            *p++ = {static_cast<short>(l), static_cast<short>(r)};
        }
        ring.advanceWritePointer(n);
        samples -= n;
    }
}
uint32_t time_us()
{
    absolute_time_t t = get_absolute_time();
    return to_us_since_boot(t);
}
extern "C" void in_ram(sms_palette_sync)(int index)
{
    int r, g, b;
    // Calculate the correct rgb color values
    // The R, G and B values are binary 01 10 11 00 shifted 6 bits to the left
    // So 01 = 01000000 = 40, 10 = 10000000 = 128, 11 = 11000000 = 192, 00 = 00000000 = 0
    // See https://segaretro.org/Palette
    // r =  bitmap.pal.color[index][0] << 0;
    // g =  bitmap.pal.color[index][1] << 2;
    // b =  bitmap.pal.color[index][2] << 4;
    // // Store the RGB565 value in the palette
    // int index = r | g | b;

    switch (bitmap.pal.color[index][0])
    {
    case 40:
        r = 85;
        break;
    case 128:
        r = 170;
        break;
    case 192:
        r = 255;
        break;
    default:
        r = 0;
    }
    switch (bitmap.pal.color[index][1])
    {
    case 40:
        g = 85;
        break;
    case 128:
        g = 170;
        break;
    case 192:
        g = 255;
        break;
    default:
        g = 0;
    }
    switch (bitmap.pal.color[index][2])
    {
    case 40:
        b = 85;
        break;
    case 128:
        b = 170;
        break;
    case 192:
        b = 255;
        break;
    default:
        b = 0;
    }

    // Store the RGB565 value in the palette
    palette565[index] = MAKE_PIXEL(r, g, b);
}
#define SCANLINEOFFSET 25
extern "C" void in_ram(sms_render_line)(int line, const uint8_t *buffer)
{
    // screen line 0 - 3 do not use
    // screen Line 4 - 235 are the visible screen
    // SMS renders 192 lines

    // Audio needs to be processed per scanline
    processaudio(line);
    line += SCANLINEOFFSET;
    if (line < 4)
        return;
    if (line == SCANLINEOFFSET)
    {
        // insert blank lines on top
        for (int bl = 4; bl < SCANLINEOFFSET; bl++)
        {
            auto blank = dvi_->getLineBuffer();
            uint16_t *sbuffer = blank->data() + 32;
            __builtin_memset(sbuffer, 0, 512);
            dvi_->setLineBuffer(bl, blank);
        }
    }

    auto b = dvi_->getLineBuffer();
    uint16_t *sbuffer = b->data() + 32;
    for (int i = screenCropX; i < BMP_WIDTH - screenCropX; i++)
    {
        sbuffer[i - screenCropX] = palette565[(buffer[i + BMP_X_OFFSET]) & 31];
    }
    dvi_->setLineBuffer(line, b);
    if (line == (SMS_HEIGHT + SCANLINEOFFSET - 1))
    {
        // insert blank lines on bottom
        for (int bl = line + 1; bl < 236; bl++)
        {
            auto blank = dvi_->getLineBuffer();
            uint16_t *sbuffer = blank->data() + 32;
            __builtin_memset(sbuffer, 0, 512);
            dvi_->setLineBuffer(bl, blank);
        }
    }
}

void system_load_sram(void)
{
    printf("system_load_sram: TODO\n");

    // TODO
}

void system_save_sram()
{
    printf("system_save_sram: saving sram TODO\n");

    // TODO
}

void system_load_state()
{
    // TODO
}

void system_save_state()
{
    // TODO
}

void in_ram(core1_main)()
{
    printf("core1 started\n");
    while (true)
    {
        dvi_->registerIRQThisCore();
        dvi_->waitForValidLine();

        dvi_->start();
        while (!exclProc_.isExist())
        {
            if (scaleMode8_7_)
            {
                dvi_->convertScanBuffer12bppScaled16_7(34, 32, 288 * 2);
                // 34 + 252 + 34
                // 32 + 576 + 32
            }
            else
            {
                dvi_->convertScanBuffer12bpp();
            }
        }

        dvi_->unregisterIRQThisCore();
        dvi_->stop();

        exclProc_.processOrWaitIfExist();
    }
}
int ProcessAfterFrameIsRendered()
{
    // disabled for now
    // nespad_read_start();
    auto count = dvi_->getFrameCounter();
    auto onOff = hw_divider_s32_quotient_inlined(count, 60) & 1;
#if LED_DISABLED == 0
    gpio_put(LED_PIN, onOff);
#endif
    // nespad_read_finish(); // Sets global nespad_state var
    tuh_task();
    // Frame rate calculation
    if (fps_enabled)
    {
        // calculate fps and round to nearest value (instead of truncating/floor)
        uint32_t tick_us = time_us() - start_tick_us;
        fps = (1000000 - 1) / tick_us + 1;
        start_tick_us = time_us();
    }
    return count;
}

static DWORD prevButtons[2]{};
static DWORD prevButtonssystem[2]{};
static int rapidFireMask[2]{};
static int rapidFireCounter = 0;
void processinput(DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem, bool ignorepushed)
{
    // pwdPad1 and pwdPad2 are only used in menu and are only set on first push
    *pdwPad1 = *pdwPad2 = *pdwSystem = 0;
    int smssystem[2]{};
    unsigned long pushed, pushedsystem;
    for (int i = 0; i < 2; i++)
    {

        auto &dst = (i == 0) ? *pdwPad1 : *pdwPad2;
        auto &gp = io::getCurrentGamePadState(i);
        int smsbuttons = (gp.buttons & io::GamePadState::Button::LEFT ? INPUT_LEFT : 0) |
                         (gp.buttons & io::GamePadState::Button::RIGHT ? INPUT_RIGHT : 0) |
                         (gp.buttons & io::GamePadState::Button::UP ? INPUT_UP : 0) |
                         (gp.buttons & io::GamePadState::Button::DOWN ? INPUT_DOWN : 0) |
                         (gp.buttons & io::GamePadState::Button::A ? INPUT_BUTTON1 : 0) |
                         (gp.buttons & io::GamePadState::Button::B ? INPUT_BUTTON2 : 0) | 0;
        // if ( gp.buttons & io::GamePadState::Button::LEFT  ) printf("LEFT\n");
        // if ( gp.buttons & io::GamePadState::Button::RIGHT ) printf("RIGHT\n");
        // if ( gp.buttons & io::GamePadState::Button::UP  ) printf("UP\n");
        // if ( gp.buttons & io::GamePadState::Button::DOWN ) printf("DOWN\n");
        // if ( gp.buttons & io::GamePadState::Button::A ) printf("A\n");
        // if ( gp.buttons & io::GamePadState::Button::B ) printf("B\n");
        // Disable for now, also keep in mind that select and start must go to smssystem
        //         if (i == 0)
        //         {
        //             v |= nespad_state;
        // #if WII_PIN_SDA >= 0 and WII_PIN_SCL >= 0
        //             v |= wiipad_read();
        // #endif
        //        }
        smssystem[i] =
            (gp.buttons & io::GamePadState::Button::SELECT ? INPUT_PAUSE : 0) |
            (gp.buttons & io::GamePadState::Button::START ? INPUT_START : 0) |
            0;
        // if (gp.buttons & io::GamePadState::Button::SELECT) printf("SELECT\n");
        // if (gp.buttons & io::GamePadState::Button::START) printf("START\n");
        input.pad[i] = smsbuttons;
        auto p1 = smssystem[i];
        if (ignorepushed == false)
        {
            pushed = smsbuttons & ~prevButtons[i];
            pushedsystem = smssystem[i] & ~prevButtonssystem[i];
        }
        else
        {
            pushed = smsbuttons;
            pushedsystem = smssystem[i];
        }
        if (p1 & INPUT_PAUSE)
        {
            if (pushedsystem & INPUT_START)
            {
                reset = true;
                printf("Reset pressed\n");
            }
        }
        if (p1 & INPUT_START)
        {
            // Toggle frame rate display
            if (pushed & INPUT_BUTTON1)
            {
                fps_enabled = !fps_enabled;
                printf("FPS: %s\n", fps_enabled ? "ON" : "OFF");
            }
            if (pushed & INPUT_UP)
            {
                screenMode(-1);
            }
            else if (pushed & INPUT_DOWN)
            {
                screenMode(+1);
            }
        }
        prevButtons[i] = smsbuttons;
        prevButtonssystem[i] = smssystem[i];
        // return only on first push
        if (pushed)
        {
            dst = smsbuttons;
        }
    }
    input.system = *pdwSystem = smssystem[0] | smssystem[1];
    // return only on first push
    if (pushedsystem)
    {
        *pdwSystem = smssystem[0] | smssystem[1];
    }
}

void in_ram(process)(void)
{
    DWORD pdwPad1, pdwPad2, pdwSystem; // have only meaning in menu
    while (reset == false)
    {
        processinput(&pdwPad1, &pdwPad2, &pdwSystem, false);
        sms_frame(0);
        ProcessAfterFrameIsRendered();
    }
}

/// @brief
/// Start emulator. Emulator does not run well in DEBUG mode, lots of red screen flicker. In order to keep it running fast enough, we need to run it in release mode or in
/// RelWithDebugInfo mode.
/// @return
int main()
{
    char selectedRom[128];
    romName = selectedRom;
    char errMSG[ERRORMESSAGESIZE];
    errMSG[0] = selectedRom[0] = 0;
    int fileSize = 0;
    FIL fil;
    FIL fil2;
    FRESULT fr;
    FRESULT fr2;
    size_t tmpSize;

    ErrorMessage = errMSG;
    // Set voltage and clock frequency
    vreg_set_voltage(VREG_VOLTAGE_1_20);
    sleep_ms(10);
    set_sys_clock_khz(CPUFreqKHz, true);

    stdio_init_all();
    sleep_ms(500);
    printf("Starting Master System Emulator\n");

#if LED_DISABLED == 0
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
#endif

    // usb initialise
    printf("USB Initialising\n");
    tusb_init();
    isFatalError = !initSDCard();
    // Load info about current game and determine file size.
    printf("Reading current game from %s (if exists).\n", ROMINFOFILE);
    fr = f_open(&fil, ROMINFOFILE, FA_READ);
    if (fr == FR_OK)
    {
        size_t r;
        fr = f_read(&fil, selectedRom, sizeof(selectedRom), &r);

        if (fr != FR_OK)
        {
            snprintf(ErrorMessage, 40, "Cannot read %s:%d\n", ROMINFOFILE, fr);
            selectedRom[0] = 0;
            printf(ErrorMessage);
        }
        else
        {
            // determine file size
            selectedRom[r] = 0;
            printf("Determine filesize of %s\n", selectedRom);
            fr2 = f_open(&fil2, selectedRom, FA_READ);
            if (fr2 == FR_OK)
            {
                fileSize = (int)f_size(&fil2);
                printf("File size: %d Bytes (0x%x)\n", fileSize, fileSize);
            }
            else
            {
                snprintf(ErrorMessage, 40, "Cannot open rom %d", fr2);
                printf("%s\n", ErrorMessage);
                selectedRom[0] = 0;
            }
            f_close(&fil2);
        }
    }
    else
    {
        snprintf(ErrorMessage, 40, "Cannot open %s:%d\n", ROMINFOFILE, fr);
        printf(ErrorMessage);
    }
    f_close(&fil);
    // When a game is started from the menu, the menu will reboot the device.
    // After reboot the emulator will start the selected game.
    if (watchdog_caused_reboot() && isFatalError == false && selectedRom[0] != 0)
    {
        // Determine loaded rom
        printf("Rebooted by menu\n");

        printf("Starting (%d) %s\n", strlen(selectedRom), selectedRom);
        printf("Checking for /START file. (Is start pressed in Menu?)\n");
        fr = f_unlink("/START");
        // 4kb is how many bytes? 0x1000 = 4096
        if (fr == FR_NO_FILE)
        {
            printf("Start not pressed, flashing rom.\n ");
            // Allocate 4k buffer. This is the smallest amount that can be flashed at once.
            size_t bufsize = 0x2000;
            BYTE *buffer = (BYTE *)malloc(bufsize);
            auto ofs = SMS_FILE_ADDR - XIP_BASE;
            printf("write %s rom to flash %x\n", selectedRom, ofs);
            fr = f_open(&fil, selectedRom, FA_READ);

            UINT bytesRead;
            if (fr == FR_OK)
            {
                // filesize already known.
                if ((fileSize / 512) & 1)
                {
                    printf("Skipping header\n");
                    fr = f_lseek(&fil, 512);
                    if (fr != FR_OK)
                    {
                        snprintf(ErrorMessage, 40, "Error skipping header: %d", fr);
                        printf("%s\n", ErrorMessage);
                        selectedRom[0] = 0;
                    }
                }
                if (fr == FR_OK)
                {

                    for (;;)
                    {
                        fr = f_read(&fil, buffer, bufsize, &bytesRead);
                        if (fr == FR_OK)
                        {
                            if (bytesRead == 0)
                            {
                                break;
                            }
                            printf("Flashing %d bytes to flash address %x\n", bytesRead, ofs);
                            printf("  -> Erasing...");

                            // Disable interupts, erase, flash and enable interrupts
                            uint32_t ints = save_and_disable_interrupts();
                            flash_range_erase(ofs, bufsize);
                            printf("\n  -> Flashing...");
                            flash_range_program(ofs, buffer, bufsize);
                            restore_interrupts(ints);
                            //

                            printf("\n");
                            ofs += bufsize;
                        }
                        else
                        {
                            snprintf(ErrorMessage, 40, "Error reading rom: %d", fr);
                            printf("Error reading rom: %d\n", fr);
                            selectedRom[0] = 0;
                            break;
                        }
                    }
                }
                f_close(&fil);
            }
            else
            {
                snprintf(ErrorMessage, 40, "Cannot open rom %d", fr);
                printf("%s\n", ErrorMessage);
                selectedRom[0] = 0;
            }
            free(buffer);
        }
        else
        {
            if (fr != FR_OK)
            {
                snprintf(ErrorMessage, 40, "Cannot delete /START file %d", fr);
                printf("%s\n", ErrorMessage);
                selectedRom[0] = 0;
            }
            else
            {
                printf("Start pressed in menu, not flashing rom.\n");
            }
        }
    }
    else
    {
        selectedRom[0] = 0;
    }
    //
    printf("Initialising DVI\n");
    dvi_ = std::make_unique<dvi::DVI>(pio0, &DVICONFIG,
                                      dvi::getTiming640x480p60Hz());
    //    dvi_->setAudioFreq(48000, 25200, 6144);
    dvi_->setAudioFreq(44100, 28000, 6272);
    dvi_->allocateAudioBuffer(256);
    //    dvi_->setExclusiveProc(&exclProc_);

    dvi_->getBlankSettings().top = 4 * 2;
    dvi_->getBlankSettings().bottom = 4 * 2;
    // dvi_->setScanLine(true);

    applyScreenMode();

    // 空サンプル詰めとく
    dvi_->getAudioRingBuffer().advanceWritePointer(255);

    multicore_launch_core1(core1_main);
    // smsp_gamedata_set(argv[1]);
    // Check the type of ROM
    // sms.console = strcmp(strrchr(argv[1], '.'), ".gg") ? CONSOLE_SMS : CONSOLE_GG;
    // sms.console = CONSOLE_SMS; // For now, we only support SMS
    //

    while (true)
    {
        if (strlen(selectedRom) == 0 || reset == true)
        {
            screenMode_ = ScreenMode::NOSCANLINE_8_7;
            applyScreenMode();
            menu(SMS_FILE_ADDR, ErrorMessage, isFatalError, reset);
        }
        reset = false;
        printf("Now playing: %s\n", selectedRom);
        load_rom(fileSize);
        // Initialize all systems and power on
        system_init(SMS_AUD_RATE);
        // load state if any
        // system_load_state();
        system_reset();
        printf("Starting game\n");
        process();
        selectedRom[0] = 0;
    }

    return 0;
}
