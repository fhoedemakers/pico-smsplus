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
#include "hardware/pio.h"
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
#include "nespad.h"
#include "wiipad.h"
#include "FrensHelpers.h"
#ifdef __cplusplus

#include "ff.h"

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
static char fpsString[3] = "00";
#define fpsfgcolor 0;     // black
#define fpsbgcolor 0xFFF; // white

#define MARGINTOP 24
#define MARGINBOTTOM 4

#define FPSSTART (((MARGINTOP + 7) / 8) * 8)
#define FPSEND ((FPSSTART) + 8)

bool reset = false;

// The Sega Master system color palette converted to RGB444
// so it can be used with the DVI library.
// from https://segaretro.org/Palette
WORD SMSPaletteRGB444[64] = {
    0x0, 0x500, 0xA00, 0xF00, 0x50, 0x550, 0xA50, 0xF50,
    0xA0, 0x5A0, 0xAA0, 0xFA0, 0xF0, 0x5F0, 0xAF0, 0xFF0,
    0x5, 0x505, 0xA05, 0xF05, 0x55, 0x555, 0xA55, 0xF55,
    0xA5, 0x5A5, 0xAA5, 0xFA5, 0xF5, 0x5F5, 0xAF5, 0xFF5,
    0xA, 0x50A, 0xA0A, 0xF0A, 0x5A, 0x55A, 0xA5A, 0xF5A,
    0xAA, 0x5AA, 0xAAA, 0xFAA, 0xFA, 0x5FA, 0xAFA, 0xFFA,
    0xF, 0x50F, 0xA0F, 0xF0F, 0x5F, 0x55F, 0xA5F, 0xF5F,
    0xAF, 0x5AF, 0xAAF, 0xFAF, 0xFF, 0x5FF, 0xAFF, 0xFFF};

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
    // Waveshare RP2040-PiZero DVI
    constexpr dvi::Config dviConfig_WaveShareRp2040 = {
        .pinTMDS = {26, 24, 22},
        .pinClock = 28,
        .invert = false,
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
        printf("Screen mode: %d\n", static_cast<int>(screenMode_));
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

    if (offset == (IS_GG ? 24 : 0))
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

extern "C" void in_ram(sms_palette_syncGG)(int index)
{
    // The GG has a different palette format
    int r = ((vdp.cram[(index << 1) | 0] >> 1) & 7) << 5;
    int g = ((vdp.cram[(index << 1) | 0] >> 5) & 7) << 5;
    int b = ((vdp.cram[(index << 1) | 1] >> 1) & 7) << 5;

    int r444 = ((r << 4) + 127) >> 8; // equivalent to (r888 * 15 + 127) / 255
    int g444 = ((g << 4) + 127) >> 8; // equivalent to (g888 * 15 + 127) / 255
    int b444 = ((b << 4) + 127) >> 8;
    palette444[index] = (r444 << 8) | (g444 << 4) | b444;
}

extern "C" void in_ram(sms_palette_sync)(int index)
{
#if 1
    // Get SMS palette color index from CRAM
    WORD r = ((vdp.cram[index] >> 0) & 3);
    WORD g = ((vdp.cram[index] >> 2) & 3);
    WORD b = ((vdp.cram[index] >> 4) & 3);
    WORD tableIndex = b << 4 | g << 2 | r;
    // Get the RGB444 color from the SMS RGB444 palette
    palette444[index] = SMSPaletteRGB444[tableIndex];
#endif

#if 0
    // Alternative color rendering below
    WORD r = ((vdp.cram[index] >> 0) & 3) << 6;
    WORD g = ((vdp.cram[index] >> 2) & 3) << 6;
    WORD b = ((vdp.cram[index] >> 4) & 3) << 6;
    int r444 = ((r << 4) + 127) >> 8; // equivalent to (r888 * 15 + 127) / 255
    int g444 = ((g << 4) + 127) >> 8; // equivalent to (g888 * 15 + 127) / 255
    int b444 = ((b << 4) + 127) >> 8;
    palette444[index] = (r444 << 8) | (g444 << 4) | b444;
#endif
    return;
}

extern "C" void in_ram(sms_render_line)(int line, const uint8_t *buffer)
{
    // DVI top margin has #MARGINTOP lines
    // DVI bottom margin has #MARGINBOTTOM lines
    // DVI usable screen estate: MARGINTOP .. (240 - #MARGINBOTTOM)
    // SMS has 192 lines
    // GG  has 144 lines
    // gg : Line starts at line 24
    // sms: Line starts at line 0
    // Emulator loops from scanline 0 to 261
    // Audio needs to be processed per scanline

    processaudio(line);
    // Adjust line number to center the emulator display
    line += MARGINTOP;
    // Only render lines that are visible on the screen, keeping into account top and bottom margins
    if (line < MARGINTOP || line >= 240 - MARGINBOTTOM)
        return;

    auto b = dvi_->getLineBuffer();
    uint16_t *sbuffer;
    if (buffer)
    {
        uint16_t *sbuffer = b->data() + 32 + (IS_GG ? 48 : 0);
        for (int i = screenCropX; i < BMP_WIDTH - screenCropX; i++)
        {
            sbuffer[i - screenCropX] = palette444[(buffer[i + BMP_X_OFFSET]) & 31];
        }
    }
    else
    {
        sbuffer = b->data() + 32;
        __builtin_memset(sbuffer, 0, 512);
    }
    // Display frame rate
    if (fps_enabled && line >= FPSSTART && line < FPSEND)
    {
        WORD *fpsBuffer = b->data() + 40;
        int rowInChar = line % 8;
        for (auto i = 0; i < 2; i++)
        {
            char firstFpsDigit = fpsString[i];
            char fontSlice = getcharslicefrom8x8font(firstFpsDigit, rowInChar);
            for (auto bit = 0; bit < 8; bit++)
            {
                if (fontSlice & 1)
                {
                    *fpsBuffer++ = fpsfgcolor;
                }
                else
                {
                    *fpsBuffer++ = fpsbgcolor;
                }
                fontSlice >>= 1;
            }
        }
    }
    dvi_->setLineBuffer(line, b);
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
#if NES_PIN_CLK != -1
    nespad_read_start();
#endif
    auto count = dvi_->getFrameCounter();
    auto onOff = hw_divider_s32_quotient_inlined(count, 60) & 1;
#if LED_GPIO_PIN != -1
    gpio_put(LED_GPIO_PIN, onOff);
#endif
#if NES_PIN_CLK != -1
    nespad_read_finish(); // Sets global nespad_state var
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
        fpsString[0] = '0' + (fps / 10);
        fpsString[1] = '0' + (fps % 10);
    }
    return count;
}

static DWORD prevButtons[2]{};
static DWORD prevButtonssystem[2]{};
static DWORD prevOtherButtons[2]{};

#define OTHER_BUTTON1 (0b1)
#define OTHER_BUTTON2 (0b10)

#define NESPAD_SELECT (0x04)
#define NESPAD_START (0x08)
#define NESPAD_UP (0x10)
#define NESPAD_DOWN (0x20)
#define NESPAD_LEFT (0x40)
#define NESPAD_RIGHT (0x80)
#define NESPAD_A (0x01)
#define NESPAD_B (0x02)

static int rapidFireMask[2]{};
static int rapidFireCounter = 0;
void processinput(DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem, bool ignorepushed, char *gamepadType)
{
    // pwdPad1 and pwdPad2 are only used in menu and are only set on first push
    *pdwPad1 = *pdwPad2 = *pdwSystem = 0;

    int smssystem[2]{};
    unsigned long pushed, pushedsystem, pushedother;
    bool usbConnected = false;
    for (int i = 0; i < 2; i++)
    {
        int nespadbuttons = 0;
        auto &dst = (i == 0) ? *pdwPad1 : *pdwPad2;
        auto &gp = io::getCurrentGamePadState(i);
        if ( i == 0 )
        {
            usbConnected = gp.isConnected();
            if (gamepadType) {
                strcpy(gamepadType, gp.GamePadName);
            }
        }
        int smsbuttons = (gp.buttons & io::GamePadState::Button::LEFT ? INPUT_LEFT : 0) |
                         (gp.buttons & io::GamePadState::Button::RIGHT ? INPUT_RIGHT : 0) |
                         (gp.buttons & io::GamePadState::Button::UP ? INPUT_UP : 0) |
                         (gp.buttons & io::GamePadState::Button::DOWN ? INPUT_DOWN : 0) |
                         (gp.buttons & io::GamePadState::Button::A ? INPUT_BUTTON1 : 0) |
                         (gp.buttons & io::GamePadState::Button::B ? INPUT_BUTTON2 : 0) | 0;
        int otherButtons = (gp.buttons & io::GamePadState::Button::X ? OTHER_BUTTON1 : 0) |
                           (gp.buttons & io::GamePadState::Button::Y ? OTHER_BUTTON2 : 0) | 0;
        smssystem[i] =
            (gp.buttons & io::GamePadState::Button::SELECT ? INPUT_PAUSE : 0) |
            (gp.buttons & io::GamePadState::Button::START ? INPUT_START : 0) |
            0;

#if NES_PIN_CLK != -1
        // When USB controller is connected both NES ports act as controller 2
        if (usbConnected)
        {          
            if (i == 1)
            {
                 nespadbuttons= nespadbuttons | nespad_states[1] | nespad_states[0];
            }
        }
        else
        {
            nespadbuttons |= nespad_states[i];
        }
#endif
// When USB controller is connected  wiipad acts as controller 2 
#if WII_PIN_SDA >= 0 and WII_PIN_SCL >= 0
        if (usbConnected)
        {
            if (i == 1)
            {
                nespadbuttons |= wiipad_read();
            }
        }
        else // if no USB controller is connected, wiipad acts as controller 1
        {
            if (i == 0)
            {
                nespadbuttons |= wiipad_read();
            }
        }
#endif
        if (nespadbuttons > 0)
        {
            smsbuttons |= ((nespadbuttons & NESPAD_UP ? INPUT_UP : 0) |
                           (nespadbuttons & NESPAD_DOWN ? INPUT_DOWN : 0) |
                           (nespadbuttons & NESPAD_LEFT ? INPUT_LEFT : 0) |
                           (nespadbuttons & NESPAD_RIGHT ? INPUT_RIGHT : 0) |
                           (nespadbuttons & NESPAD_A ? INPUT_BUTTON1 : 0) |
                           (nespadbuttons & NESPAD_B ? INPUT_BUTTON2 : 0) | 0);
            smssystem[i] |= ((nespadbuttons & NESPAD_SELECT ? INPUT_PAUSE : 0) |
                             (nespadbuttons & NESPAD_START ? INPUT_START : 0) | 0);
        }

        // if (gp.buttons & io::GamePadState::Button::SELECT) printf("SELECT\n");
        // if (gp.buttons & io::GamePadState::Button::START) printf("START\n");
        input.pad[i] = smsbuttons;
        auto p1 = smssystem[i];
        if (ignorepushed == false)
        {
            pushed = smsbuttons & ~prevButtons[i];
            pushedsystem = smssystem[i] & ~prevButtonssystem[i];
            pushedother = otherButtons & ~prevOtherButtons[i];
        }
        else
        {
            pushed = smsbuttons;
            pushedsystem = smssystem[i];
            pushedother = otherButtons;
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
        prevOtherButtons[i] = otherButtons;
        // return only on first push
        if (pushed)
        {
            dst = smsbuttons;
        }
        if (pushedother)
        {
            if (pushedother & OTHER_BUTTON1)
            {
                printf("Other 1\n");
            }
            if (pushedother & OTHER_BUTTON2)
            {
                printf("Other 2\n");
            }
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
        processinput(&pdwPad1, &pdwPad2, &pdwSystem, false, nullptr);
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
    char selectedRom[FF_MAX_LFN];
    romName = selectedRom;
    char errMSG[ERRORMESSAGESIZE];
    errMSG[0] = selectedRom[0] = 0;
    int fileSize = 0;
    bool isGameGear = false;
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

#if LED_GPIO_PIN != -1
    gpio_init(LED_GPIO_PIN);
    gpio_set_dir(LED_GPIO_PIN, GPIO_OUT);
    gpio_put(LED_GPIO_PIN, 1);
#endif

    // usb initialise
    printf("USB Initialising\n");
    tusb_init();
    if ((isFatalError = !initSDCard()) == false)
    {
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
                isGameGear = Frens::cstr_endswith(selectedRom, ".gg");
                printf("Current game: %s\n", selectedRom);
                printf("Console is %s\n", isGameGear ? "Game Gear" : "Master System");
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
    }
    // When a game is started from the menu, the menu will reboot the device.
    // After reboot the emulator will start the selected game.
    if (watchdog_enable_caused_reboot() && isFatalError == false && selectedRom[0] != 0)
    {
        // Determine loaded rom
        printf("Rebooted by menu\n");

        printf("Starting (%d) %s\n", strlen(selectedRom), selectedRom);
        printf("Checking for /START file. (Is start pressed in Menu?)\n");
        fr = f_unlink("/START");
        if (fr == FR_NO_FILE)
        {
            printf("Start not pressed, flashing rom.\n");
            // Allocate buffer for flashing. Borrow emulator memory for this.
            // size_t bufsize = 0;                                 // 0x2000;
            // BYTE *buffer = getcachestorefromemulator(&bufsize); //(BYTE *)malloc(bufsize);
            size_t bufsize = 0x1000;    // Write 4k at a time, larger sizes will increases the risk of making XInput unresponsive. (Still happens sometimes)                              
            BYTE *buffer = (BYTE *)malloc(bufsize);

            auto ofs = SMS_FILE_ADDR - XIP_BASE;
            printf("write rom %s to flash %x\n", selectedRom, ofs);
            UINT totalBytes = 0;
            fr = f_open(&fil, selectedRom, FA_READ);
#if LED_GPIO_PIN != -1
            bool onOff = true;
#endif
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
#if LED_GPIO_PIN != -1
                            gpio_put(LED_GPIO_PIN, onOff);
                            onOff = !onOff;
#endif
                            // Disable interupts, erase, flash and enable interrupts
                            uint32_t ints = save_and_disable_interrupts();
                            flash_range_erase(ofs, bufsize);
                            flash_range_program(ofs, buffer, bufsize);
                            restore_interrupts(ints);
                            ofs += bufsize;
                            totalBytes += bytesRead;
                            // keep the usb stack running
                            tuh_task();
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
                printf("Wrote %d bytes to flash\n", totalBytes);
            }
            else
            {
                snprintf(ErrorMessage, 40, "Cannot open rom %d", fr);
                printf("%s\n", ErrorMessage);
                selectedRom[0] = 0;
            }
            free(buffer);
            printf("Flashing done\n");
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

    // Adjust the top and bottom to center the emulator screen
    dvi_->getBlankSettings().top = MARGINTOP * 2;
    dvi_->getBlankSettings().bottom = MARGINBOTTOM * 2;
    // dvi_->setScanLine(true);

    applyScreenMode();
#if NES_PIN_CLK != -1
    nespad_begin(0, CPUFreqKHz, NES_PIN_CLK, NES_PIN_DATA, NES_PIN_LAT, NES_PIO);
#endif
#if NES_PIN_CLK_1 != -1
    nespad_begin(1, CPUFreqKHz, NES_PIN_CLK_1, NES_PIN_DATA_1, NES_PIN_LAT_1, NES_PIO_1);
#endif
#if WII_PIN_SDA >= 0 and WII_PIN_SCL >= 0
    wiipad_begin();
#endif
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
            // reset margin to give menu more screen space
            dvi_->getBlankSettings().top = 4 * 2;
            dvi_->getBlankSettings().bottom = 4 * 2;
            screenMode_ = ScreenMode::NOSCANLINE_8_7;
            applyScreenMode();
            menu(SMS_FILE_ADDR, ErrorMessage, isFatalError, reset);
        }
        reset = false;
        printf("Now playing: %s\n", selectedRom);
        load_rom(fileSize, isGameGear);
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
