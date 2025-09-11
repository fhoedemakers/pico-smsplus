/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/divider.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "hardware/watchdog.h"
#include "util/work_meter.h"
#include "ff.h"
#include "tusb.h"
#include "gamepad.h"
#include "menu.h"
#include "nespad.h"
#include "wiipad.h"
#include "FrensHelpers.h"
#include "settings.h"
#include "FrensFonts.h"
#include "vumeter.h"
#include "shared.h"
#include "mytypes.h"
#include "PicoPlusPsram.h"

bool isFatalError = false;
static FATFS fs;
char *romName;

static bool fps_enabled = false;
static uint32_t start_tick_us = 0;
static uint32_t fps = 0;
static char fpsString[3] = "00";

// Note: When using framebuffer, AUDIOBUFFERSIZE must be increased to 1024
#if PICO_RP2350
#define AUDIOBUFFERSIZE 1024
#else
#define AUDIOBUFFERSIZE 256
#endif

#define EMULATOR_CLOCKFREQ_KHZ 252000 //  Overclock frequency in kHz when using Emulator
static uint32_t CPUFreqKHz = EMULATOR_CLOCKFREQ_KHZ;

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
#if !HSTX
// RGB444 format
WORD SMSPaletteRGB[64] = {
    0x0, 0x500, 0xA00, 0xF00, 0x50, 0x550, 0xA50, 0xF50,
    0xA0, 0x5A0, 0xAA0, 0xFA0, 0xF0, 0x5F0, 0xAF0, 0xFF0,
    0x5, 0x505, 0xA05, 0xF05, 0x55, 0x555, 0xA55, 0xF55,
    0xA5, 0x5A5, 0xAA5, 0xFA5, 0xF5, 0x5F5, 0xAF5, 0xFF5,
    0xA, 0x50A, 0xA0A, 0xF0A, 0x5A, 0x55A, 0xA5A, 0xF5A,
    0xAA, 0x5AA, 0xAAA, 0xFAA, 0xFA, 0x5FA, 0xAFA, 0xFFA,
    0xF, 0x50F, 0xA0F, 0xF0F, 0x5F, 0x55F, 0xA5F, 0xF5F,
    0xAF, 0x5AF, 0xAAF, 0xFAF, 0xFF, 0x5FF, 0xAFF, 0xFFF};
#else
// RGB555 format
WORD SMSPaletteRGB[64] = {
    0x0, 0x500, 0xA00, 0xF00, 0x50, 0x550, 0xA50, 0xF50,
    0xA0, 0x5A0, 0xAA0, 0xFA0, 0xF0, 0x5F0, 0xAF0, 0xFF0,
    0x5, 0x505, 0xA05, 0xF05, 0x55, 0x555, 0xA55, 0xF55,
    0xA5, 0x5A5, 0xAA5, 0xFA5, 0xF5, 0x5F5, 0xAF5, 0xFF5,
    0xA, 0x50A, 0xA0A, 0xF0A, 0x5A, 0x55A, 0xA5A, 0xF5A,
    0xAA, 0x5AA, 0xAAA, 0xFAA, 0xFA, 0x5FA, 0xAFA, 0xFFA,
    0xF, 0x50F, 0xA0F, 0xF0F, 0x5F, 0x55F, 0xA5F, 0xF5F,
    0xAF, 0x5AF, 0xAAF, 0xFAF, 0xFF, 0x5FF, 0xAFF, 0xFFF};
#endif // HSTX
#define HEADER_OFFSET 0x7FF0
#define HEADER_SIZE 16

struct SegaHeader
{
    // 0x7FF0
    char signature[8];
    // 0x7FF8
    uint16_t reserverd;
    // 0x7FFA
    uint16_t checksum;
    // 0x7FFC
    uint8_t product_code[2];
    // 0x7FFE
    uint8_t ProductCodeAndVersion;
    // 0x7FFF
    uint8_t sizeAndRegion;
} *header;
bool detect_rom_type_from_memory(uintptr_t addr, int *size, bool *isGameGear)
{
    char *rom = (char *)addr;
    int actualSize = 0;
    header = (SegaHeader *)(rom + HEADER_OFFSET);
    if (strncmp(header->signature, "TMR SEGA", 8) == 0)
    {
        printf("Sega header found\n");

        uint8_t romsize = header->sizeAndRegion & 0b00001111;
        uint8_t region = (header->sizeAndRegion >> 4) & 0b00001111;
        // https://www.smspower.org/Development/ROMHeader
        switch (romsize)
        {
        case 0:                 // 256KB
            *size = 512 * 1024; // 512KB and 1MB Roms are reported in the header as 256KB.
                                // Setting Rom size to 512KB also works for 256KB roms.
            break;              // Setting rom size to 1MB for 256 or 512KB games does not work.
                                // Only a small set of roms are 1MB.
        case 1:
            *size = 512 * 1024;
            break;
        case 2:
            *size = 1024 * 1024;
            break;
        case 0xa:
            *size = 8 * 1024;
            break;
        case 0xb:
            *size = 16 * 1024;
            break;
        case 0xc:
            *size = 32 * 1024;
            break;
        case 0xd:
            *size = 48 * 1024;
            break;
        case 0xe:
            *size = 64 * 1024;
            break;
        case 0xf:
            *size = 128 * 1024;
            break;
        default:
            printf("Unknown romsize %x\n", romsize);
            *size = 0; // unknown size
            break;
        }
        printf("Romsize %x = %d bytes\n", romsize, *size);
#if PICO_RP2350 && PSRAM_CS_PIN
        // If PSRAM is used, get the actual size of the allocated block to determine the actual size of the rom.
        if (Frens::isPsramEnabled())
        {
            printf("PSRAM enabled, getting size of allocated block\n");
            PicoPlusPsram &psram_ = PicoPlusPsram::getInstance();
            printf("Size in rom: %d bytes, ", *size);
            *size = psram_.GetSize((void *)rom);
            printf("actual size in PSRAM: %d bytes\n", *size);
            printf("Setting rom size to %d bytes\n", *size);
        }
#endif

        *isGameGear = false;

        printf("Region: %x - ", region);
        switch (region)
        {
        case 3:
            printf("SMS Japan\n");
            *isGameGear = false;
            break;
        case 4:
            printf("SMS Export\n");
            *isGameGear = false;
            break;
        case 5:
            printf("GG USA\n");
            *isGameGear = true;
            break;
        case 6:
            printf("GG Export\n");
            *isGameGear = true;
            break;
        case 7:
            printf("GG International\n");
            *isGameGear = true;
            break;
        default:
            printf("Unknown\n");
            break;
        }
    }
    if (*size > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
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
#if !HSTX
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
            int l = *p1++; // (*p1++ << 16) + *p2++;
            // works also : int l = (*p1++ + *p2++) / 2;
            int r = *p2++;
            // int l = *wave1++;
#if EXT_AUDIO_IS_ENABLED
            if (settings.useExtAudio)
            {
                // uint32_t sample32 = (l << 16) | (r & 0xFFFF);
                EXT_AUDIO_ENQUEUE_SAMPLE(l, r);
            }
            else
            {
                *p++ = {static_cast<short>(l), static_cast<short>(r)};
            }
#else
            *p++ = {static_cast<short>(l), static_cast<short>(r)};
#endif
        }
#if EXT_AUDIO_IS_ENABLED
        if (!settings.useExtAudio)
        {
            ring.advanceWritePointer(n);
        }
#else
        ring.advanceWritePointer(n);
#endif
        samples -= n;
    }
#else
    for (i = 0; i < samples; i++)
    {
        int l = *p1++; // (*p1++ << 16) + *p2++;
        // works also : int l = (*p1++ + *p2++) / 2;
        int r = *p2++;
        // int l = *wave1++;
        // int r = l;
        EXT_AUDIO_ENQUEUE_SAMPLE(l, r);
#if ENABLE_VU_METER
        if (settings.flags.enableVUMeter)
        {
            addSampleToVUMeter(l);
        }
#endif
    }
#endif
}

extern "C" void in_ram(sms_palette_syncGG)(int index)
{
    // The GG has a different palette format
    int r = ((vdp.cram[(index << 1) | 0] >> 1) & 7) << 5;
    int g = ((vdp.cram[(index << 1) | 0] >> 5) & 7) << 5;
    int b = ((vdp.cram[(index << 1) | 1] >> 1) & 7) << 5;
#if !HSTX
    int r444 = ((r << 4) + 127) >> 8; // equivalent to (r888 * 15 + 127) / 255
    int g444 = ((g << 4) + 127) >> 8; // equivalent to (g888 * 15 + 127) / 255
    int b444 = ((b << 4) + 127) >> 8;
    palette444[index] = (r444 << 8) | (g444 << 4) | b444;
#else
    int r555 = ((r << 5) + 127) >> 8; // equivalent to (r888 * 31 + 127) / 255
    int g555 = ((g << 5) + 127) >> 8; // equivalent to (g888 * 31 + 127) / 255
    int b555 = ((b << 5) + 127) >> 8;

    palette444[index] = (r555 << 10) | (g555 << 5) | b555;
#endif
    return;
}

extern "C" void in_ram(sms_palette_sync)(int index)
{
#if 0
    // Get SMS palette color index from CRAM
    WORD r = ((vdp.cram[index] >> 0) & 3);
    WORD g = ((vdp.cram[index] >> 2) & 3);
    WORD b = ((vdp.cram[index] >> 4) & 3);
    WORD tableIndex = b << 4 | g << 2 | r;
    // Get the RGB444 color from the SMS RGB444 palette
    palette444[index] = SMSPaletteRGB[tableIndex];
#endif
#if 1
    // Alternative color rendering below
    WORD r = ((vdp.cram[index] >> 0) & 3) << 6;
    WORD g = ((vdp.cram[index] >> 2) & 3) << 6;
    WORD b = ((vdp.cram[index] >> 4) & 3) << 6;

#if !HSTX
    int r444 = ((r << 4) + 127) >> 8; // equivalent to (r888 * 15 + 127) / 255
    int g444 = ((g << 4) + 127) >> 8; // equivalent to (g888 * 15 + 127) / 255
    int b444 = ((b << 4) + 127) >> 8;
    palette444[index] = (r444 << 8) | (g444 << 4) | b444;
#else
    int r555 = ((r << 5) + 127) >> 8; // equivalent to (r888 * 31 + 127) / 255
    int g555 = ((g << 5) + 127) >> 8; // equivalent to (g888 * 31 + 127) / 255
    int b555 = ((b << 5) + 127) >> 8;
    palette444[index] = (r555 << 10) | (g555 << 5) | b555;
#endif
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
#if !HSTX
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
#endif
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
#if !HSTX
    dvi_->setLineBuffer(line, b);
#endif
}

void system_load_sram(void)
{
    char pad[FF_MAX_LFN];
    FILINFO fno;
    char fileName[FF_MAX_LFN];
    strcpy(fileName, Frens::GetfileNameFromFullPath(romName));
    Frens::stripextensionfromfilename(fileName);

    snprintf(pad, FF_MAX_LFN, "%s/%s.SAV", GAMESAVEDIR, fileName);

    FIL fil;
    FRESULT fr;

    size_t bytesRead;
    if (!sms.sram)
    {
        snprintf(ErrorMessage, ERRORMESSAGESIZE, "sms.sram is NULL, cannot load SRAM");
        printf("%s\n", ErrorMessage);
        return;
    }
    printf("Load SRAM from %s\n", pad);
    fr = f_stat(pad, &fno);
    if (fr == FR_NO_FILE)
    {
        printf("No save file found, skipping load.\n");
        return;
    }
    if (fr != FR_OK)
    {
        snprintf(ErrorMessage, ERRORMESSAGESIZE, "f_stat() failed on save file: %d", fr);
        printf("%s\n", ErrorMessage);
        return;
    }
    if (fno.fsize != SRAMSIZEBYTES)
    {
        snprintf(ErrorMessage, ERRORMESSAGESIZE, "Save file size mismatch: %d != %d", fno.fsize, SRAMSIZEBYTES);
        printf("%s\n", ErrorMessage);
        return;
    }

    printf("Loading save file %s\n", pad);
    fr = f_open(&fil, pad, FA_READ);
    if (fr == FR_OK)
    {
        fr = f_read(&fil, sms.sram, SRAMSIZEBYTES, &bytesRead);
        if (fr == FR_OK)
        {
            printf("Savefile read from disk %d bytes\n", bytesRead);
            sms.save = 1;
        }
        else
        {
            snprintf(ErrorMessage, ERRORMESSAGESIZE, "Cannot read save file: %d %d/%d read", fr, bytesRead, SRAMSIZEBYTES);
            printf("%s\n", ErrorMessage);
        }
    }
    else
    {
        snprintf(ErrorMessage, ERRORMESSAGESIZE, "Cannot open save file: %d", fr);
        printf("%s\n", ErrorMessage);
    }
    f_close(&fil);
}

void system_save_sram()
{
    char pad[FF_MAX_LFN];
    char fileName[FF_MAX_LFN];
    printf("system_save_sram: Saving SRAM\n");
    strcpy(fileName, Frens::GetfileNameFromFullPath(romName));
    Frens::stripextensionfromfilename(fileName);
    if (!sms.save)
    {
        printf("SRAM not updated.\n");
        return;
    }
    snprintf(pad, FF_MAX_LFN, "%s/%s.SAV", GAMESAVEDIR, fileName);
    printf("Save SRAM to %s\n", pad);
    FIL fil;
    FRESULT fr;
    fr = f_open(&fil, pad, FA_CREATE_ALWAYS | FA_WRITE);
    if (fr != FR_OK)
    {
        snprintf(ErrorMessage, ERRORMESSAGESIZE, "Cannot open save file: %d", fr);
        printf("%s\n", ErrorMessage);
        return;
    }
    size_t bytesWritten;
    fr = f_write(&fil, sms.sram, SRAMSIZEBYTES, &bytesWritten);
    if (bytesWritten < SRAMSIZEBYTES)
    {
        snprintf(ErrorMessage, ERRORMESSAGESIZE, "Error writing save: %d %d/%d written", fr, bytesWritten, SRAMSIZEBYTES);
        printf("%s\n", ErrorMessage);
    }
    f_close(&fil);
    printf("done\n");
}

void system_load_state()
{
    // TODO
}

void system_save_state()
{
    // TODO
}

int ProcessAfterFrameIsRendered()
{
#if NES_PIN_CLK != -1
    nespad_read_start();
#endif
    auto count =
#if !HSTX
        dvi_->getFrameCounter();
#else
        hstx_getframecounter();
#endif
    auto onOff = hw_divider_s32_quotient_inlined(count, 60) & 1;
    Frens::blinkLed(onOff);
#if NES_PIN_CLK != -1
    nespad_read_finish(); // Sets global nespad_state var
#endif
    // nespad_read_finish(); // Sets global nespad_state var
    tuh_task();
    // Frame rate calculation
    if (fps_enabled)
    {
        // calculate fps and round to nearest value (instead of truncating/floor)
        uint32_t tick_us = Frens::time_us() - start_tick_us;
        fps = (1000000 - 1) / tick_us + 1;
        start_tick_us = Frens::time_us();
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
        if (i == 0)
        {
            usbConnected = gp.isConnected();
            if (gamepadType)
            {
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
                nespadbuttons = nespadbuttons | nespad_states[1] | nespad_states[0];
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
                system_save_sram();
                reset = true;
                printf("Reset pressed\n");
            }
            if (pushed & INPUT_LEFT)
            {
#if EXT_AUDIO_IS_ENABLED
                settings.useExtAudio = !settings.useExtAudio;
                if (settings.useExtAudio)
                {
                    printf("Using I2S Audio\n");
                }
                else
                {
                    printf("Using DVIAudio\n");
                }

#else
                settings.flags.useExtAudio = 0;
#endif
                Frens::savesettings();
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
                Frens::screenMode(-1);
            }
            else if (pushed & INPUT_DOWN)
            {
                Frens::screenMode(+1);
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
static char selectedRom[FF_MAX_LFN];
/// @brief
/// Start emulator. Emulator does not run well in DEBUG mode, lots of red screen flicker. In order to keep it running fast enough, we need to run it in release mode or in
/// RelWithDebugInfo mode.
/// @return
int main()
{

    romName = selectedRom;
    ErrorMessage[0] = selectedRom[0] = 0;

    int fileSize = 0;
    bool isGameGear = false;

    // Set voltage and clock frequency
    vreg_set_voltage(VREG_VOLTAGE_1_20);
    sleep_ms(10);
    set_sys_clock_khz(CPUFreqKHz, true);

    stdio_init_all();
     printf("==========================================================================================\n");
    printf("Pico-InfoSMS+ %s\n", SWVERSION);
    printf("Build date: %s\n", __DATE__);
    printf("Build time: %s\n", __TIME__);
    printf("CPU freq: %d kHz\n", clock_get_hz(clk_sys) / 1000);
    printf("Stack size: %d bytes\n", PICO_STACK_SIZE);
    printf("==========================================================================================\n");
    printf("Starting up...\n");
    printf("Starting Master System Emulator\n");
    printf("CPU freq: %d\n", clock_get_hz(clk_sys));

    // Note:
    //     - When using framebuffer, AUDIOBUFFERSIZE must be increased to 1024
    //     - Top and bottom margins are reset to zero
    isFatalError = !Frens::initAll(selectedRom, CPUFreqKHz, MARGINTOP, MARGINBOTTOM, AUDIOBUFFERSIZE, false, true);
    bool showSplash = true;
    while (true)
    {
        if (strlen(selectedRom) == 0 || reset == true)
        {
            menu("Pico-SMS+", ErrorMessage, isFatalError, showSplash, ".sms .gg", selectedRom, "SMS");
            // returns only when PSRAM is enabled,
            printf("Selected rom from menu: %s\n", selectedRom);
        }
        reset = false;
        fileSize = 0;
        isGameGear = false;
        if (Frens::isPsramEnabled())
        {
            // Detect rom type from memory
            // This is used to determine the rom size and type (SMS or GG)
            detect_rom_type_from_memory(ROM_FILE_ADDR, &fileSize, &isGameGear);
            if (fileSize == 0)
            {
                // No rom loaded, continue to menu
                printf("No rom loaded, continuing to menu\n");
                selectedRom[0] = 0;
                continue;
            }
        }
        else
        {
            FRESULT fr;
            FIL file;
            fr = f_open(&file, selectedRom, FA_READ);
            if (fr != FR_OK)
            {
                snprintf(ErrorMessage, 40, "Cannot open rom %d", fr);
                printf("%s\n", ErrorMessage);
                selectedRom[0] = 0;
                continue;
            }
            fileSize = f_size(&file);
            f_close(&file);
            isGameGear = Frens::cstr_endswith(selectedRom, ".gg");
            printf("Now playing: %s (%d bytes)\n", selectedRom, fileSize);
        }
        if (isGameGear)
        {
            printf("Game Gear rom detected\n");
        }
        else
        {
            printf("Master System rom detected\n");
        }
        load_rom(ROM_FILE_ADDR, fileSize, isGameGear);
        // Initialize all systems and power on
        system_init(SMS_AUD_RATE);
        // load state if any
        // system_load_state();
        system_reset();
        printf("Starting game\n");
        process();
        system_shutdown();
        selectedRom[0] = 0;
        showSplash = false;
    }

    return 0;
}
