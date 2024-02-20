/*
    memz80.c --
    Z80 port handlers.
*/
#include "shared.h"

void (*cpu_writemem16)(int address, int data);
void (*cpu_writeport16)(uint16_t port, uint8_t data);
uint8_t (*cpu_readport16)(uint16_t port);

/* Pull-up resistors on data bus */
uint8_t data_bus_pullup   = 0x00;
uint8_t data_bus_pulldown = 0x00;

/* Read unmapped memory */
uint8_t z80_read_unmapped(void)
{
    int pc = z80_get_reg(Z80_PC);
    uint8_t data;
    pc = (pc - 1) & 0xFFFF;
    data = cpu_readmap[pc >> 13][pc & 0x03FF];
    return ((data | data_bus_pullup) & ~data_bus_pulldown);
}

void memctrl_w(uint8_t data)
{
    sms.memctrl = data;
}

/*--------------------------------------------------------------------------*/
/* Sega Master System port handlers                                         */
/*--------------------------------------------------------------------------*/

void sms_port_w(uint16_t port, uint8_t data)
{
    port &= 0xFF;

    if(port >= 0xF0)
    {
        switch(port)
        {
            case 0xF0:
                fmunit_write(0, data);
                return;

            case 0xF1:
                fmunit_write(1, data);
                return;

            case 0xF2:
                fmunit_detect_w(data);
                return;
        }
    }

    switch(port & 0xC1)
    {
        case 0x00:
            memctrl_w(data);
            return;

        case 0x01:
            ioctrl_w(data);
            return;

        case 0x40:
        case 0x41:
            psg_write(data);
            return;

        case 0x80:
        case 0x81:
            vdp_write(port, data);
            return;

        case 0xC0:
        case 0xC1:
            return;
    }
}

uint8_t sms_port_r(uint16_t port)
{
    port &= 0xFF;

    if(port == 0xF2) //&& !(sms.memctrl & 4))
        return fmunit_detect_r();

    switch(port & 0xC0)
    {
        case 0x00:
            return z80_read_unmapped();

        case 0x40:
            return vdp_counter_r(port);

        case 0x80:
            return vdp_read(port);

        case 0xC0:
            return input_r(port);
    }

    /* Just to please the compiler */
    return -1;
}



/*--------------------------------------------------------------------------*/
/* Game Gear port handlers                                                  */
/*--------------------------------------------------------------------------*/

void gg_port_w(uint16_t port, uint8_t data)
{
    port &= 0xFF;

    if(port <= 0x06) {
        sio_w(port, data);
        return;
    }

    switch(port & 0xC1)
    {
        case 0x00:
            memctrl_w(data);
            return;

        case 0x01:
            ioctrl_w(data);
            return;

        case 0x40:
        case 0x41:
            psg_write(data);
            return;

        case 0x80:
        case 0x81:
            gg_vdp_write(port, data);
            return;
    }
}


uint8_t gg_port_r(uint16_t port)
{
    port &= 0xFF;

    if(port <= 0x06)
        return sio_r(port);

    switch(port & 0xC0)
    {
        case 0x00:
            return z80_read_unmapped();

        case 0x40:
            return vdp_counter_r(port);

        case 0x80:
            return vdp_read(port);

        case 0xC0:
            switch(port)
            {
                case 0xC0:
                case 0xC1:
                case 0xDC:
                case 0xDD:
                    return input_r(port);
            }
            return z80_read_unmapped();
    }

    /* Just to please the compiler */
    return -1;
}

/*--------------------------------------------------------------------------*/
/* Game Gear (MS) port handlers                                             */
/*--------------------------------------------------------------------------*/

void ggms_port_w(uint16_t port, uint8_t data)
{
    port &= 0xFF;

    switch(port & 0xC1)
    {
        case 0x00:
            memctrl_w(data);
            return;

        case 0x01:
            ioctrl_w(data);
            return;

        case 0x40:
        case 0x41:
            psg_write(data);
            return;

        case 0x80:
        case 0x81:
            gg_vdp_write(port, data);
            return;
    }
}

uint8_t ggms_port_r(uint16_t port)
{
    port &= 0xFF;

    switch(port & 0xC0)
    {
        case 0x00:
            return z80_read_unmapped();

        case 0x40:
            return vdp_counter_r(port);

        case 0x80:
            return vdp_read(port);

        case 0xC0:
            switch(port)
            {
                case 0xC0:
                case 0xC1:
                case 0xDC:
                case 0xDD:
                    return input_r(port);
            }
            return z80_read_unmapped();
    }

    /* Just to please the compiler */
    return -1;
}

/*--------------------------------------------------------------------------*/
/* MegaDrive / Genesis port handlers                                        */
/*--------------------------------------------------------------------------*/

void md_port_w(uint16_t port, uint8_t data)
{
    switch(port & 0xC1)
    {
        case 0x00:
            /* No memory control register */
            return;

        case 0x01:
            ioctrl_w(data);
            return;

        case 0x40:
        case 0x41:
            psg_write(data);
            return;

        case 0x80:
        case 0x81:
            md_vdp_write(port, data);
            return;
    }
}


uint8_t md_port_r(uint16_t port)
{
    switch(port & 0xC0)
    {
        case 0x00:
            return z80_read_unmapped();

        case 0x40:
            return vdp_counter_r(port);

        case 0x80:
            return vdp_read(port);

        case 0xC0:
            switch(port)
            {
                case 0xC0:
                case 0xC1:
                case 0xDC:
                case 0xDD:
                    return input_r(port);
            }
            return z80_read_unmapped();
    }

    /* Just to please the compiler */
    return -1;
}

