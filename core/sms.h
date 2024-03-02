
#ifndef _SMS_H_
#define _SMS_H_

#define TYPE_OVERSEAS   (0)
#define TYPE_DOMESTIC   (1)
enum {
    MAPPER_NONE         = 0,
    MAPPER_SEGA         = 1,
    MAPPER_CODIES       = 2,
    MAPPER_KOREA        = 3,
    MAPPER_KOREA2       = 4
};

enum {
    DISPLAY_NTSC        = 0,
    DISPLAY_PAL         = 1
};

enum {
    FPS_NTSC        = 60,
    FPS_PAL         = 50
};

enum {
    CLOCK_NTSC        = 3579545,
    CLOCK_PAL         = 3579545
};

enum {
    CONSOLE_SMS         = 0x20,
    CONSOLE_SMSJ        = 0x21,
    CONSOLE_SMS2        = 0x22,

    CONSOLE_GG          = 0x40,
    CONSOLE_GGMS        = 0x41,

    CONSOLE_MD          = 0x80,
    CONSOLE_MDPBC       = 0x81,
    CONSOLE_GEN         = 0x82,
    CONSOLE_GENPBC      = 0x83
};
enum {
    TERRITORY_DOMESTIC  = 0,
    TERRITORY_EXPORT    = 1
};
/* SMS context */
typedef struct {
    uint8 *dummy; //JMD: Point this into outher space plz.
    uint8 ram[0x2000];
//    uint8 sram[0x8000];
    uint8 *sram;
    uint8 fcr[4];
    uint8 paused;
    uint8 save;
    uint8 country;
    uint8 port_3F;
    uint8 port_F2;
    uint8 use_fm;
    uint8 irq;
    uint8 psg_mask;
    uint8 territory;
    uint8 display;
} t_sms;

/* Global data */
extern t_sms sms;

/* Function prototypes */
void sms_frame(int skip_render);

void sms_init(void);

void sms_reset(void);

int sms_irq_callback(int param);

void sms_mapper_w(int address, int data);

void cpu_reset(void);

#endif /* _SMS_H_ */
