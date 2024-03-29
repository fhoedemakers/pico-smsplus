
#ifndef _SMS_H_
#define _SMS_H_

#define TYPE_OVERSEAS   (0)
#define TYPE_DOMESTIC   (1)

#define SMS_WIDTH 256
#define SMS_HEIGHT 192
#define SMS_AUD_RATE 44100
#define SMS_FPS 60
// rendering
static uint8_t screenCropX = 0;
static uint16_t screenBufferLine[256];
static uint8_t smsBufferLine[SMS_WIDTH];
static int palette565[32];
static uint8_t sram[0x8000];
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
