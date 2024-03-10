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

const uint LED_PIN = PICO_DEFAULT_LED_PIN;

#ifndef DVICONFIG
// #define DVICONFIG dviConfig_PicoDVI
// #define DVICONFIG dviConfig_PicoDVISock
#define DVICONFIG dviConfig_PimoroniDemoDVSock
#endif

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

    std::unique_ptr<dvi::DVI> dvi_;

    util::ExclusiveProc exclProc_;

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


extern "C" void in_ram(sms_palette_sync)(int index)
{
    int r, g, b;
    r = bitmap.pal.color[index][0];
    g = bitmap.pal.color[index][1];
    b = bitmap.pal.color[index][2];
    // Render using the correct rgb color values
    // See https://segaretro.org/Palette
    switch (r)
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
    switch (g)
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
    switch (b)
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
    palette565[index] = MAKE_PIXEL(r, g, b);
}
#define SCANLINEOFFSET 25
extern "C" void in_ram(sms_render_line)(int line, const uint8_t *buffer)
{
    // screen line 0 - 3 do not use
    // screen Line 4 - 235 are the visible screen
    // SMS renders 192 lines

    line += SCANLINEOFFSET;
    if ( line < 4 ) return;
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

#ifdef LINUX
    if (line == BMP_HEIGHT + BMP_Y_OFFSET - 1)
    {
        s_core->getDisplay()->flip();
    }
#endif
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

void in_ram(process)(void)
{
    // TODO
#if !defined(NDEBUG)
    printf("process()\n");
#endif
    uint32_t frame = 0;
    while (true)
    {
        // TODO Process input
        sms_frame(0);

        // TODO Process audio
        // printf("Frame %d\n", frame++);
        gpio_put(LED_PIN, hw_divider_s32_quotient_inlined(dvi_->getFrameCounter(), 60) & 1);
        // gpio_put(LED_PIN, hw_divider_s32_quotient_inlined(frame++, 60) & 1);
    }
}

/// @brief 
/// Start emulator. Emulator does not run well in DEBUG mode, lots of red screen flicker. In order to keep it running fast enough, we need to run it in release mode or in
/// RelWithDebugInfo mode.
/// @return 
int main()
{
    // Set voltage and clock frequency
    vreg_set_voltage(VREG_VOLTAGE_1_20);
    sleep_ms(10);
    set_sys_clock_khz(CPUFreqKHz, true);

    stdio_init_all();
    for (int i = 0; i < 5; i++)
    {
        printf("Hello, world! The master system emulator is starting...(%d)\n", i);
        sleep_ms(1000);
    }
    printf("Starting Master System Emulator\n");
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    //
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
    // TODO usb initialise tusb_init();

    // TODO flash ROM
    // TODO Setup DV
    //
    // TODO smsp_gamedata_set(argv[1]);
    //
    // Check the type of ROM
    // sms.console = strcmp(strrchr(argv[1], '.'), ".gg") ? CONSOLE_SMS : CONSOLE_GG;
    // sms.console = CONSOLE_SMS; // For now, we only support SMS
    load_rom();

    // fprintf(stdout, "CRC : %08X\n", cart.crc);
    //  fprintf(stdout, "SHA1: %s\n", cart.sha1);
    //   fprintf(stdout, "SHA1: ");
    //   for (int i = 0; i < SHA1_DIGEST_SIZE; i++) {
    //   	fprintf(stdout, "%02X", cart.sha1[i]);
    //   }
    //   fprintf(stdout, "\n");

    // Set defaults. Is this needed?
    // settings.video_scale = 2;
    // settings.video_filter = 0;
    // settings.audio_rate = 48000;
    // settings.audio_fm = 1;
    // settings.audio_fmtype = SND_EMU2413;
    // settings.misc_region = TERRITORY_DOMESTIC;
    // settings.misc_ffspeed = 2;

    // TODO
    // Override settings set in the .ini
    // gbIniError err = gb_ini_parse("smsplus.ini", &smsp_ini_handler, &settings);
    // if (err.type != GB_INI_ERROR_NONE) {
    // 	fprintf(stderr, "Error: No smsplus.ini file found.\n");
    // }

    // sms.territory = settings.misc_region;
    // if (sms.console != CONSOLE_GG) { sms.use_fm = settings.audio_fm; }

    // Initialize all systems and power on

    // init sms
    system_init(SMS_AUD_RATE);

    // load state if any
    // system_load_state();

    // initialize

    system_reset();
    process();
    return 0;
}