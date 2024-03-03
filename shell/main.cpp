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
#include "smsplus.h"

#define SMS_WIDTH 256
#define SMS_HEIGHT 192
#define SMS_AUD_RATE 44100
#define SMS_FPS 60


const uint LED_PIN = PICO_DEFAULT_LED_PIN;

#ifndef DVICONFIG
//#define DVICONFIG dviConfig_PicoDVI
#define DVICONFIG dviConfig_PicoDVISock
#endif

#define CC(x) (((x >> 1) & 15) | (((x >> 6) & 15) << 4) | (((x >> 11) & 15) << 8))

namespace
{
    constexpr uint32_t CPUFreqKHz = 252000;

    constexpr dvi::Config dviConfig_PicoDVI = {
        .pinTMDS = {10, 12, 14},
        .pinClock = 8,
        .invert = true,
    };

    constexpr dvi::Config dviConfig_PicoDVISock = {
        .pinTMDS = {12, 18, 16},
        .pinClock = 14,
        .invert = false,
    };

    std::unique_ptr<dvi::DVI> dvi_;

    // Defined static constexpr uintptr_t NES_FILE_ADDR = 0x10080000;

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
// rendering
static uint8_t screenCropX = 0;
static uint16_t screenBufferLine[240];
static uint8_t smsBufferLine[SMS_WIDTH];
static int palette565[32];
static uint8_t sram[0x8000];
static settings_t settings;
static gamedata_t gdata;
extern "C" void in_ram(sms_palette_sync)(int index) {
    palette565[index] = ((bitmap.pal.color[index][0] >> 3) << 11)
                        | ((bitmap.pal.color[index][1] >> 2) << 5)
                        | (bitmap.pal.color[index][2] >> 3);
}

extern "C" void in_ram(sms_render_line)(int line, const uint8_t *buffer) {
    //printf("sms_render_line(%i)\r\n", line);
    for (int i = screenCropX; i < BMP_WIDTH - screenCropX; i++) {
        screenBufferLine[i - screenCropX] = palette565[(buffer[i + BMP_X_OFFSET]) & 31];       
    }

   // s_core->getDisplay()->put(screenBufferLine, BMP_WIDTH - (screenCropX * 2));

#ifdef LINUX
    if (line == BMP_HEIGHT + BMP_Y_OFFSET - 1) {
        s_core->getDisplay()->flip();
    }
#endif
}

void system_load_sram(void) {
    printf("system_load_sram: TODO\n");
  
    // TODO
}

void system_save_sram() {
    printf("system_save_sram: saving sram TODO\n");

  // TODO
}

void system_load_state() {
  //TODO
}

void system_save_state() {
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

int main()
{
    // Set voltage and clock frequency
    vreg_set_voltage(VREG_VOLTAGE_1_20);
    sleep_ms(10);
    set_sys_clock_khz(CPUFreqKHz, true);

    stdio_init_all();
    for (int i=0; i<5; i++) {
        printf("Hello, world! The master system emulator is starting...(%d)\n", i);
        sleep_ms(1000);
    }
    // 
    printf("%x\n", CC(0x7FFF));
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
    //sms.console = CONSOLE_SMS; // For now, we only support SMS
    load_rom();

    fprintf(stdout, "CRC : %08X\n", cart.crc);
	//fprintf(stdout, "SHA1: %s\n", cart.sha1);
	// fprintf(stdout, "SHA1: ");
	// for (int i = 0; i < SHA1_DIGEST_SIZE; i++) {
	// 	fprintf(stdout, "%02X", cart.sha1[i]);
	// }
	// fprintf(stdout, "\n");

    // Set defaults. Is this needed?
	settings.video_scale = 2;
	settings.video_filter = 0;
	settings.audio_rate = 48000;
	settings.audio_fm = 1;
	//settings.audio_fmtype = SND_EMU2413;
	//settings.misc_region = TERRITORY_DOMESTIC;
	settings.misc_ffspeed = 2;

    // TODO
    // Override settings set in the .ini
	// gbIniError err = gb_ini_parse("smsplus.ini", &smsp_ini_handler, &settings);
	// if (err.type != GB_INI_ERROR_NONE) {
	// 	fprintf(stderr, "Error: No smsplus.ini file found.\n");
	// }

    //sms.territory = settings.misc_region;
	//if (sms.console != CONSOLE_GG) { sms.use_fm = settings.audio_fm; }
	
	// Initialize all systems and power on

	system_init(SMS_AUD_RATE);
	uint32_t frame=0;
    while(true) 
    {
        // TODO Process input
        sms_frame(0);
        // TODO Process audio
        printf("Frame %d\n", frame++);
        // gpio_put(LED_PIN, hw_divider_s32_quotient_inlined(dvi_->getFrameCounter(), 60) & 1);
         gpio_put(LED_PIN, hw_divider_s32_quotient_inlined(frame, 60) & 1);
    }
    return 0;
}