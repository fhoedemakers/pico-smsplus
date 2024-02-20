
#ifndef _FMINTF_H_
#define _FMINTF_H_

#include <stdint.h>

enum {
    SND_EMU2413,        /* Mitsutaka Okazaki's YM2413 emulator */
    SND_YM2413          /* Jarek Burczynski's YM2413 emulator */
};

typedef struct {
    uint8_t latch;
    uint8_t reg[0x40];
} FM_Context;

/* Function prototypes */
void FM_Init(void);
void FM_Shutdown(void);
void FM_Reset(void);
void FM_Update(int16_t **buffer, int length);
void FM_Write(int offset, int data);
void FM_GetContext(uint8_t *data);
void FM_SetContext(uint8_t *data);
int FM_GetContextSize(void);
uint8_t *FM_GetContextPtr(void);
void FM_WriteReg(int reg, int data);

#endif /* _FMINTF_H_ */
