
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define APP_NAME            "SMS Plus"
#define APP_VERSION         "1.9-git"

#define PALETTE_SIZE        0x20

/* Mask for removing unused pixel data */
#define PIXEL_MASK          0x1F

/* These can be used for 'input.pad[]' */
#define INPUT_UP            0x00000001
#define INPUT_DOWN          0x00000002
#define INPUT_LEFT          0x00000004
#define INPUT_RIGHT         0x00000008
#define INPUT_BUTTON2       0x00000010
#define INPUT_BUTTON1       0x00000020

/* These can be used for 'input.system' */
#define INPUT_START         0x00000001  /* Game Gear only */    
#define INPUT_PAUSE         0x00000002  /* Master System only */
#define INPUT_RESET         0x00000004  /* Master System only */

enum {
    SRAM_SAVE   = 0,
    SRAM_LOAD   = 1
};


/* User input structure */
typedef struct
{
    uint32_t pad[2];
    uint8_t analog[2];
    uint32_t system;
} input_t;

/* Game image structure */
typedef struct
{
    uint8_t *rom;
    uint8_t pages;
    uint32_t crc;
    uint8_t sha1[20];
    uint32_t sram_crc;
    int mapper;
    uint8_t sram[0x8000];
    uint8_t fcr[4];
} cart_t;

/* Bitmap structure */
typedef struct
{
    unsigned char *data;
    int width;
    int height;
    int pitch;
    int depth;
    int granularity;
    struct {
        int x, y, w, h;
        int ox, oy, ow, oh;
        int changed;
    } viewport;        
    struct
    {
        uint8_t color[PALETTE_SIZE][3];
        uint8_t dirty[PALETTE_SIZE];
        uint8_t update;
    }pal;
} bitmap_t;

/* Global variables */
extern bitmap_t bitmap;     /* Display bitmap */
extern cart_t cart;         /* Game cartridge data */
extern input_t input;       /* Controller input */

/* Function prototypes */
void system_frame(int skip_render);
void system_init(void);
void system_shutdown(void);
void system_reset(void);
void system_manage_sram(uint8_t *sram, int slot, int mode);
void system_poweron(void);
void system_poweroff(void);

#endif /* _SYSTEM_H_ */
