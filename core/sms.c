/*
    sms.c --
    Sega Master System console emulation.
*/
#include "shared.h"

/* SMS context */
sms_t sms;

uint8_t dummy_write[0x400];
uint8_t dummy_read[0x400];

void read_map(uint8_t *src, int offset, int length)
{
    int index;
    int page_shift = 10;
    int page_count = (length >> page_shift) & 0x3F;

    for(index = 0; index < page_count; index++)
    {
        cpu_readmap[(offset >> page_shift) | index] = &src[index << page_shift];
    }
}

void writemem_mapper_none(int offset, int data)
{
    cpu_writemap[offset >> 10][offset & 0x03FF] = data;
}

void writemem_mapper_sega(int offset, int data)
{
    cpu_writemap[offset >> 10][offset & 0x03FF] = data;
    if(offset >= 0xFFFC)
        sms_mapper_w(offset & 3, data);
}

void writemem_mapper_codies(int offset, int data)
{
    switch(offset & 0xC000)
    {
        case 0x0000:
            sms_mapper_w(1, data);
            return;
            
        case 0x4000:
            sms_mapper_w(2, data);
            return;
            
        case 0x8000:
            sms_mapper_w(3, data);
            return;
            
        case 0xC000:
            cpu_writemap[offset >> 10][offset & 0x03FF] = data;
            return;
    }

}

void writemem_mapper_korea(int offset, int data)
{
    int i;
    static const int bank_mask = 0x0F;
    static const int bank_shift = 13;
    static const int page_mask = 0x0F;
    static const int page_shift = 10;
    uint8_t *base = &cart.rom[(data & bank_mask) << bank_shift];

    switch(offset)
    {
        /* 4-bit data written to 0000 maps 8K page to 8000-9FFF */
        case 0x0000:
            for(i = 0; i < 8; i++)
            {
                cpu_readmap[(0x8000 >> page_shift) + i] = \
                    &base[(i & page_mask) << page_shift];
            }
            return;

        /* 4-bit data written to 0001 maps 8K page to A000-BFFF */
        case 0x0001:
            for(i = 0; i < 8; i++)
            {
                cpu_readmap[(0xA000 >> page_shift) + i] = \
                    &base[(i & page_mask) << page_shift];
            }
            return;

        /* 4-bit data written to 0002 maps 8K page to 4000-5FFF */
        case 0x0002:
            for(i = 0; i < 8; i++)
            {
                cpu_readmap[(0x4000 >> page_shift) + i] = \
                    &base[(i & page_mask) << page_shift];
            }
            return;

        /* 4-bit data written to 0003 maps 8K page to 6000-7FFF */
        case 0x0003:
            for(i = 0; i < 8; i++)
            {
                cpu_readmap[(0x6000 >> page_shift) + i] = \
                    &base[(i & page_mask) << page_shift];
            }
            return;
    }

    cpu_writemap[offset >> 10][offset & 0x03FF] = data;
}

void writemem_mapper_korea2(int offset, int data)
{
    int i;
    static const int bank_mask = 0x3F;
    static const int bank_shift = 13;
    static const int page_shift = 10;
    uint8_t *base = &cart.rom[(data & bank_mask) << bank_shift];

    switch(offset)
    {
        case 0x4000: /* 6-bit data written to 4000 maps 8K page to 4000-5FFF */
        case 0x6000: /* 6-bit data written to 6000 maps 8K page to 6000-7FFF */
        case 0x8000: /* 6-bit data written to 8000 maps 8K page to 8000-9FFF */
        case 0xA000: /* 6-bit data written to A000 maps 8K page to A000-BFFF */
            for(i = 0; i < 8; i++)
            {
                cpu_readmap[(offset >> page_shift) + i] = \
                    &base[(i & bank_mask) << page_shift];
            }
            return;
    }

    cpu_writemap[offset >> 10][offset & 0x03FF] = data;
}


void sms_init(void)
{
    z80_init();

    sms_reset();

    /* Default: open bus */
    data_bus_pullup     = 0x00;
    data_bus_pulldown   = 0x00;

    /* Assign mapper */
    switch(cart.mapper)
    {
        case MAPPER_NONE:
            cpu_writemem16 = writemem_mapper_none;
            break;

        case MAPPER_SEGA:
            cpu_writemem16 = writemem_mapper_sega;
            break;

        case MAPPER_KOREA:
            cpu_writemem16 = writemem_mapper_korea;
            break;

        case MAPPER_KOREA2:
            cpu_writemem16 = writemem_mapper_korea2;
            break;

        case MAPPER_CODIES:
            cpu_writemem16 = writemem_mapper_codies;
            break;

        default:
            cpu_writemem16 = writemem_mapper_sega;
            break;
    }

    /* Initialize selected console emulation */
    switch(sms.console)
    {
        case CONSOLE_SMS:
            cpu_writeport16 = sms_port_w;
            cpu_readport16 = sms_port_r;
            break;
  
        case CONSOLE_SMS2:
            cpu_writeport16 = sms_port_w;
            cpu_readport16 = sms_port_r;
            data_bus_pullup = 0xFF;
            break;

        case CONSOLE_GG:
            cpu_writeport16 = gg_port_w;
            cpu_readport16 = gg_port_r;
            data_bus_pullup = 0xFF;
            break;

        case CONSOLE_GGMS:
            cpu_writeport16 = ggms_port_w;
            cpu_readport16 = ggms_port_r;
            data_bus_pullup = 0xFF;
            break;

        case CONSOLE_GEN:
        case CONSOLE_MD:
            cpu_writeport16 = md_port_w;
            cpu_readport16 = md_port_r;
            break;

        case CONSOLE_GENPBC:
        case CONSOLE_MDPBC:
            cpu_writeport16 = md_port_w;
            cpu_readport16 = md_port_r;
            data_bus_pullup = 0xFF;
            break;
    }
}

void sms_shutdown(void)
{
    /* Nothing to do */
}

void sms_reset(void)
{
    int i;

    
    z80_reset(NULL);
    z80_set_irq_callback(sms_irq_callback);

    /* Clear SMS context */
    memset(dummy_write, 0, sizeof(dummy_write));
    memset(dummy_read,  0, sizeof(dummy_read));
    memset(sms.wram,    0, sizeof(sms.wram));
    memset(cart.sram,    0, sizeof(cart.sram));

    sms.paused      = 0x00;
    sms.save        = 0x00;
    sms.fm_detect   = 0x00;
    sms.memctrl     = 0xAB;
    sms.ioctrl      = 0xFF;

    for(i = 0x00; i <= 0x2F; i++)
    {
        cpu_readmap[i]  = &cart.rom[(i & 0x1F) << 10];
        cpu_writemap[i] = dummy_write;
    }

    for(i = 0x30; i <= 0x3F; i++)
    {
        cpu_readmap[i] = &sms.wram[(i & 0x07) << 10];
        cpu_writemap[i] = &sms.wram[(i & 0x07) << 10];
    }

    cart.fcr[0] = 0x00;
    cart.fcr[1] = 0x00;
    cart.fcr[2] = 0x01;
    cart.fcr[3] = 0x00;
}


void sms_mapper_w(int address, int data)
{
    int i;

    /* Calculate ROM page index */
    uint8_t page = (data % cart.pages);

    /* Save frame control register data */
    cart.fcr[address] = data;

    switch(address)
    {
        case 0:
            if(data & 8)
            {
                uint32_t offset = (data & 4) ? 0x4000 : 0x0000;
                sms.save = 1;

                for(i = 0x20; i <= 0x2F; i++)
                {
                    cpu_writemap[i] = cpu_readmap[i]  = &cart.sram[offset + ((i & 0x0F) << 10)];
                }
            }
            else
            {
                for(i = 0x20; i <= 0x2F; i++)
                {          
                    cpu_readmap[i] = &cart.rom[((cart.fcr[3] % cart.pages) << 14) | ((i & 0x0F) << 10)];
                    cpu_writemap[i] = dummy_write;
                }
            }
            break;

        case 1:
            for(i = 0x01; i <= 0x0F; i++)
            {
                cpu_readmap[i] = &cart.rom[(page << 14) | ((i & 0x0F) << 10)];
            }
            break;

        case 2:
            for(i = 0x10; i <= 0x1F; i++)
            {
                cpu_readmap[i] = &cart.rom[(page << 14) | ((i & 0x0F) << 10)];
            }
            break;

        case 3:
            if(!(cart.fcr[0] & 0x08))
            {
                for(i = 0x20; i <= 0x2F; i++)
                {
                    cpu_readmap[i] = &cart.rom[(page << 14) | ((i & 0x0F) << 10)];
                }
            }
            break;
    }
}




int sms_irq_callback(int param)
{
    return 0xFF;
}



