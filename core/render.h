
#ifndef _RENDER_H_
#define _RENDER_H_

/* Pack RGB data into a 32-bit RGB 5:6:5 format */
#define MAKE_PIXEL(r,g,b)   (r << 16 | g << 8 | b)

/* Used for blanking a line in whole or in part */
#define BACKDROP_COLOR      (0x10 | (vdp.reg[7] & 0x0F))

extern uint8_t sms_cram_expand_table[4];
extern uint8_t gg_cram_expand_table[16];
extern void (*render_bg)(int line);
extern void (*render_obj)(int line);
extern uint8_t *linebuf;
extern uint8_t internal_buffer[0x100];
extern uint32_t pixel[];
extern uint8_t bg_name_dirty[0x200];     
extern uint16_t bg_name_list[0x200];     
extern uint16_t bg_list_index;           
extern uint8_t tms_lookup[16][256][2];
extern uint8_t mc_lookup[16][256][8];
extern uint8_t txt_lookup[256][2];
extern uint8_t bp_expand[256][8];
extern uint8_t lut[0x10000];
extern uint32_t bp_lut[0x20000];

void render_shutdown(void);
void render_init(void);
void render_reset(void);
void render_line(int line);
void render_bg_sms(int line);
void render_obj_sms(int line);
void palette_sync(int index, int force);
void remap_internal_to_host(int line);

#endif /* _RENDER_H_ */
