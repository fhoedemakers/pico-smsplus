/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"

#include "shared.h"
#include "smsplus.h"

static settings_t settings;
static gamedata_t gdata;

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
    sms.console = CONSOLE_SMS; // For now, we only support SMS
    load_rom();

    fprintf(stdout, "CRC : %08X\n", cart.crc);
	//fprintf(stdout, "SHA1: %s\n", cart.sha1);
	fprintf(stdout, "SHA1: ");
	for (int i = 0; i < SHA1_DIGEST_SIZE; i++) {
		fprintf(stdout, "%02X", cart.sha1[i]);
	}
	fprintf(stdout, "\n");

    // Set defaults. Is this needed?
	settings.video_scale = 2;
	settings.video_filter = 0;
	settings.audio_rate = 48000;
	settings.audio_fm = 1;
	settings.audio_fmtype = SND_EMU2413;
	settings.misc_region = TERRITORY_DOMESTIC;
	settings.misc_ffspeed = 2;

    // TODO
    // Override settings set in the .ini
	// gbIniError err = gb_ini_parse("smsplus.ini", &smsp_ini_handler, &settings);
	// if (err.type != GB_INI_ERROR_NONE) {
	// 	fprintf(stderr, "Error: No smsplus.ini file found.\n");
	// }

    sms.territory = settings.misc_region;
	if (sms.console != CONSOLE_GG) { sms.use_fm = settings.audio_fm; }
	
	// Initialize all systems and power on
	system_init();
	system_poweron();
    // while(true) {
    //     system_frame(0);
    // }
    return 0;
}