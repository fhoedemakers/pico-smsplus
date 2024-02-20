#ifndef _SMSPLUS_H
#define _SMSPLUS_H

typedef struct {
	char gamename[256];
	char sramdir[256];
	char sramfile[516];
	char stdir[256];
} gamedata_t;

typedef struct {
	int video_scale;
	int video_filter;
	int audio_rate;
	int audio_fm;
	int audio_fmtype;
	int misc_region;
	int misc_ffspeed;
} settings_t;

settings_t *smsp_settings_ptr();
void smsp_state(int slot, int mode);

#endif
