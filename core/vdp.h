#ifndef _VDP_H_
#define _VDP_H_

/*
    vdp1

    mode 4 when m4 set and m1 reset

    vdp2

    mode 4 when m4 set and m2,m1 != 1,0


*/

/* Display timing (NTSC) */

#define MASTER_CLOCK        3579545
#define LINES_PER_FRAME     262
#define FRAMES_PER_SECOND   60
#define CYCLES_PER_LINE     ((MASTER_CLOCK / FRAMES_PER_SECOND) / LINES_PER_FRAME)

/* VDP context */
typedef struct
{
    uint8_t vram[0x4000];
    uint8_t cram[0x40]; 
    uint8_t reg[0x10];
    uint8_t status;     
    uint8_t latch;      
    uint8_t pending;    
    uint8_t buffer;     
    uint8_t code;       
    uint16_t addr;
    int pn, ct, pg, sa, sg;
    int ntab;        
    int satb;
    int line;
    int left;
    uint8_t height;
    uint8_t extended;
    uint8_t mode;
    uint8_t vint_pending;
    uint8_t hint_pending;
    uint16_t cram_latch;
    uint8_t bd;

    void (*vram_write)(uint16_t offset, uint8_t data);
    uint8_t (*vram_read)(uint16_t offset, uint8_t data);

} vdp_t;

/* Global data */
extern vdp_t vdp;

/* Function prototypes */
void vdp_init(void);
void vdp_shutdown(void);
void vdp_reset(void);
uint8_t vdp_counter_r(int offset);
uint8_t vdp_read(int offset);
void vdp_write(int offset, uint8_t data);
void gg_vdp_write(int offset, uint8_t data);
void md_vdp_write(int offset, uint8_t data);
void tms_write(int offset, int data);
void viewport_check(void);

#endif /* _VDP_H_ */

