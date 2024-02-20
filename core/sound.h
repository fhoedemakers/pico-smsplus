#ifndef _SOUND_H_
#define _SOUND_H_
#include <stdint.h>

enum {
    STREAM_PSG_L,               /* PSG left channel */
    STREAM_PSG_R,               /* PSG right channel */
    STREAM_FM_MO,               /* YM2413 melody channel */
    STREAM_FM_RO,               /* YM2413 rhythm channel */
    STREAM_MAX                  /* Total # of sound streams */
};  

/* Sound emulation structure */
typedef struct
{
    void (*mixer_callback)(int16_t **stream, int16_t **output, int length);
    int16_t *output[2];
    int16_t *stream[STREAM_MAX];
    int fm_which;
    int enabled;
    int fps;
    int buffer_size;
    int sample_count;
    int sample_rate;
    int done_so_far;
    uint32_t fm_clock;
    uint32_t psg_clock;
} snd_t;


/* Global data */
extern snd_t snd;

/* Function prototypes */
void psg_write(int data);
void psg_stereo_w(int data);
int fmunit_detect_r(void);
void fmunit_detect_w(int data);
void fmunit_write(int offset, int data);
int sound_init(void);
void sound_shutdown(void);
void sound_reset(void);
void sound_update(int line);
void sound_mixer_callback(int16_t **stream, int16_t **output, int length);

#endif /* _SOUND_H_ */
