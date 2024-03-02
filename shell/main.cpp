/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"

#include "shared.h"
#include "smsplus.h"
#define SMS_WIDTH 256
#define SMS_HEIGHT 192
#define SMS_AUD_RATE 44100
#define SMS_FPS 60

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
int main()
{
    // TODO set overclock

    stdio_init_all();
    printf("Staring Master System Emulator\n");
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
	
    while(true) 
    {
        // TODO Process input
        sms_frame(0);
        // TODO Process audio
    }
    return 0;
}