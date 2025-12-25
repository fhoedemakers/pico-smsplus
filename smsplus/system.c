/*
    Copyright (C) 1998, 1999, 2000  Charles Mac Donald

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "shared.h"
#include "ff.h"
#include <stdbool.h>

t_bitmap bitmap;
t_cart cart;
t_snd snd;
t_input input;
//OPLL *opll;

struct {
    char reg[64];
} ym2413;

void *frens_f_malloc(size_t size);
void frens_f_free(void *ptr);
void system_init(int rate) {

    // initialize memory
    cachePtr = (int16 *)frens_f_malloc(512 * 4 * sizeof(int16));
    cacheStore = (uint8 *)frens_f_malloc(CACHEDTILES * 64 * sizeof(uint8));
    cacheStoreUsed = (uint8 *)frens_f_malloc(CACHEDTILES * sizeof(uint8));
    if (!cachePtr || !cacheStore || !cacheStoreUsed) {
        printf("Failed to allocate memory for cache\n");
        exit(1);
    }
    memset(cachePtr, 0, 512 * 4 * sizeof(int16));
    memset(cacheStore, 0, CACHEDTILES * 64 * sizeof(uint8));
    memset(cacheStoreUsed, 0, CACHEDTILES * sizeof(uint8));
    /* Initialize the VDP emulation */
    vdp_init();

    /* Initialize the SMS emulation */
    sms_init();

    /* Initialize the look-up tables and related data */
    render_init();

    /* Enable sound emulation if the sample rate was specified */
    audio_init(rate);

    /* Don't save SRAM by default */
    sms.save = 0;

    /* Clear emulated button state */
    __builtin_memset(&input, 0, sizeof(t_input));
}

void audio_init(int rate) {
    /* Clear sound context */
    __builtin_memset(&snd, 0, sizeof(t_snd));

    /* Reset logging data */
    snd.log = 0;
    snd.callback = NULL;

    /* Oops.. sound is disabled */
    if (!rate) return;

    /* Calculate buffer size in samples */
    snd.bufsize = (rate / 60);

    /* Sound output */
    snd.buffer[0] = (signed short int *) frens_f_malloc(snd.bufsize * 2);
    snd.buffer[1] = (signed short int *) frens_f_malloc(snd.bufsize * 2);
    if (!snd.buffer[0] || !snd.buffer[1]) return;
    __builtin_memset(snd.buffer[0], 0, snd.bufsize * 2);
    __builtin_memset(snd.buffer[1], 0, snd.bufsize * 2);

    /* YM2413 sound stream */
//    snd.fm_buffer = (signed short int *)malloc(snd.bufsize * 2);
//    if(!snd.fm_buffer) return;
//    memset(snd.fm_buffer, 0, snd.bufsize * 2);

    /* SN76489 sound stream */
//    snd.psg_buffer[0] = (signed short int *)malloc(snd.bufsize * 2);
//    snd.psg_buffer[1] = (signed short int *)malloc(snd.bufsize * 2);
//    if(!snd.psg_buffer[0] || !snd.psg_buffer[1]) return;
//    memset(snd.psg_buffer[0], 0, snd.bufsize * 2);
//    memset(snd.psg_buffer[1], 0, snd.bufsize * 2);

    /* Set up SN76489 emulation */
    SN76496_init(0, MASTER_CLOCK, 255, rate);

    /* Set up YM2413 emulation */
//    OPLL_init(3579545, rate) ;
//    opll = OPLL_new() ;
//    OPLL_reset(opll) ;
//    OPLL_reset_patch(opll,0) ;            /* if use default voice data. */ 

    /* Inform other functions that we can use sound */
    snd.enabled = 1;
}


void system_shutdown(void) {
     // free memory
     printf("Free memory\n");
    frens_f_free(cachePtr);
    frens_f_free(cacheStore);
    frens_f_free(cacheStoreUsed);
    frens_f_free(snd.buffer[0]);
    frens_f_free(snd.buffer[1]);
    if (snd.enabled) {
//        OPLL_delete(opll);
//        OPLL_close();
    }
}


void system_reset(void) {
   
    
    cpu_reset();
    vdp_reset();
    sms_reset();
    render_reset();
    system_load_sram();
    if (snd.enabled) {
//        OPLL_reset(opll) ;
//        OPLL_reset_patch(opll,0) ;            /* if use default voice data. */ 
    }
}


bool system_save_state(FIL *fd) {
    UINT bw;
    FRESULT fr;
    /* Save VDP context */
    fr = f_write(fd, &vdp, sizeof(t_vdp), &bw);
    if (fr != FR_OK || bw != sizeof(t_vdp)) {
        printf("Error writing VDP context: fr=%d wrote=%u expected=%u\n", fr, bw, (unsigned)sizeof(t_vdp));
        return false;
    }

    /* Save SMS context */
    fr = f_write(fd, &sms, sizeof(t_sms), &bw);
    if (fr != FR_OK || bw != sizeof(t_sms)) {
        printf("Error writing SMS context: fr=%d wrote=%u expected=%u\n", fr, bw, (unsigned)sizeof(t_sms));
        return false;
    }

    /* Save Z80 context */
    fr = f_write(fd, Z80_Context, sizeof(Z80_Regs), &bw);
    if (fr != FR_OK || bw != sizeof(Z80_Regs)) {
        printf("Error writing Z80 regs: fr=%d wrote=%u expected=%u\n", fr, bw, (unsigned)sizeof(Z80_Regs));
        return false;
    }
    fr = f_write(fd, &after_EI, sizeof(int), &bw);
    if (fr != FR_OK || bw != sizeof(int)) {
        printf("Error writing after_EI: fr=%d wrote=%u expected=%u\n", fr, bw, (unsigned)sizeof(int));
        return false;
    }

    /* Save YM2413 registers */
    fr = f_write(fd, &ym2413.reg[0], 0x40, &bw);
    if (fr != FR_OK || bw != 0x40) {
        printf("Error writing YM2413 regs: fr=%d wrote=%u expected=%u\n", fr, bw, (unsigned)0x40);
        return false;
    }

    /* Save SN76489 context */
    fr = f_write(fd, &sn[0], sizeof(t_SN76496), &bw);
    if (fr != FR_OK || bw != sizeof(t_SN76496)) {
        printf("Error writing SN76489 context: fr=%d wrote=%u expected=%u\n", fr, bw, (unsigned)sizeof(t_SN76496));
        return false;
    }

    return true;
}


bool system_load_state(FIL *fd) {
    int i;
    uint8 reg[0x40];
    UINT br;
    FRESULT fr;

    /* Initialize everything */
    // cpu_reset();
    system_reset();

    /* Load VDP context */
    fr = f_read(fd, &vdp, sizeof(t_vdp), &br);
    if (fr != FR_OK || br != sizeof(t_vdp)) {
        printf("Error reading VDP context: fr=%d read=%u expected=%u\n", fr, br, (unsigned)sizeof(t_vdp));
        return false;
    }

    /* Load SMS context */
    fr = f_read(fd, &sms, sizeof(t_sms), &br);
    if (fr != FR_OK || br != sizeof(t_sms)) {
        printf("Error reading SMS context: fr=%d read=%u expected=%u\n", fr, br, (unsigned)sizeof(t_sms));
        return false;
    }

    /* Load Z80 context */
    fr = f_read(fd, Z80_Context, sizeof(Z80_Regs), &br);
    if (fr != FR_OK || br != sizeof(Z80_Regs)) {
        printf("Error reading Z80 regs: fr=%d read=%u expected=%u\n", fr, br, (unsigned)sizeof(Z80_Regs));
        return false;
    }
    fr = f_read(fd, &after_EI, sizeof(int), &br);
    if (fr != FR_OK || br != sizeof(int)) {
        printf("Error reading after_EI: fr=%d read=%u expected=%u\n", fr, br, (unsigned)sizeof(int));
        return false;
    }

    /* Load YM2413 registers */
    fr = f_read(fd, reg, 0x40, &br);
    if (fr != FR_OK || br != 0x40) {
        printf("Error reading YM2413 regs: fr=%d read=%u expected=%u\n", fr, br, (unsigned)0x40);
        return false;
    }

    /* Load SN76489 context */
    fr = f_read(fd, &sn[0], sizeof(t_SN76496), &br);
    if (fr != FR_OK || br != sizeof(t_SN76496)) {
        printf("Error reading SN76489 context: fr=%d read=%u expected=%u\n", fr, br, (unsigned)sizeof(t_SN76496));
        return false;
    }

    /* Restore callbacks */
    z80_set_irq_callback(sms_irq_callback);

    cpu_readmap[0] = cart.rom + 0x0000; /* 0000-3FFF */
    cpu_readmap[1] = cart.rom + 0x2000;
    cpu_readmap[2] = cart.rom + 0x4000; /* 4000-7FFF */
    cpu_readmap[3] = cart.rom + 0x6000;
    cpu_readmap[4] = cart.rom + 0x0000; /* 0000-3FFF */
    cpu_readmap[5] = cart.rom + 0x2000;
    cpu_readmap[6] = sms.ram;
    cpu_readmap[7] = sms.ram;

    cpu_writemap[0] = sms.dummy;
    cpu_writemap[1] = sms.dummy;
    cpu_writemap[2] = sms.dummy;
    cpu_writemap[3] = sms.dummy;
    cpu_writemap[4] = sms.dummy;
    cpu_writemap[5] = sms.dummy;
    cpu_writemap[6] = sms.ram;
    cpu_writemap[7] = sms.ram;

    sms_mapper_w(3, sms.fcr[3]);
    sms_mapper_w(2, sms.fcr[2]);
    sms_mapper_w(1, sms.fcr[1]);
    sms_mapper_w(0, sms.fcr[0]);

    /* Force full pattern cache update */
//    is_vram_dirty = 1;
//    memset(vram_dirty, 1, 0x200);

    /* Restore palette */
    for (i = 0; i < PALETTE_SIZE; i += 1)
        palette_sync(i);

    /* Restore sound state */
    if (snd.enabled) {
#if 0
        /* Clear YM2413 context */
        OPLL_reset(opll) ;
        OPLL_reset_patch(opll,0) ;            /* if use default voice data. */ 

        /* Restore rhythm enable first */
        ym2413_write(0, 0, 0x0E);
        ym2413_write(0, 1, reg[0x0E]);

        /* User instrument settings */
        for(i = 0x00; i <= 0x07; i += 1)
        {
            ym2413_write(0, 0, i);
            ym2413_write(0, 1, reg[i]);
        }

        /* Channel frequency */
        for(i = 0x10; i <= 0x18; i += 1)
        {
            ym2413_write(0, 0, i);
            ym2413_write(0, 1, reg[i]);
        }

        /* Channel frequency + ctrl. */
        for(i = 0x20; i <= 0x28; i += 1)
        {
            ym2413_write(0, 0, i);
            ym2413_write(0, 1, reg[i]);
        }

        /* Instrument and volume settings  */
        for(i = 0x30; i <= 0x38; i += 1)
        {
            ym2413_write(0, 0, i);
            ym2413_write(0, 1, reg[i]);
        }
#endif
    return true;
    }
}

void ym2413_write(int chip, int offset, int data) {
//    static uint8 latch = 0;
//    if(offset & 1)
//        OPLL_writeReg(opll, latch, data);
//    else
//        latch = data;
}




