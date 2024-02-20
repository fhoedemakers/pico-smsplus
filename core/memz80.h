
#ifndef _MEMZ80_H_
#define _MEMZ80_H_

/* Global data */
extern uint8_t data_bus_pullup;
extern uint8_t data_bus_pulldown;

/* Function prototypes */
uint8_t z80_read_unmapped(void);
void gg_port_w(uint16_t port, uint8_t data);
uint8_t gg_port_r(uint16_t port);
void ggms_port_w(uint16_t port, uint8_t data);
uint8_t ggms_port_r(uint16_t port);
void sms_port_w(uint16_t port, uint8_t data);
uint8_t sms_port_r(uint16_t port);
void md_port_w(uint16_t port, uint8_t data);
uint8_t md_port_r(uint16_t port);

#endif /* _MEMZ80_H_ */
