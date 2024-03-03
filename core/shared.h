#ifndef _SHARED_H_
#define _SHARED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
// FH Enable little Endian
#define LSB_FIRST
// FH End
#define VERSION     "0.9.4a"


#ifndef SMS_FILE_ADDR
#define SMS_FILE_ADDR 0x10080000
#endif

#ifdef _MSC_VER
#include <stdio.h>
#include <string.h>
#ifndef __inline__
#define __inline__ __inline
#endif
#ifndef strcasecmp
#define strcasecmp stricmp
#endif
#endif

#ifndef in_ram
#ifdef PICO_BOARD
#include <pico/platform.h>
#define in_ram __not_in_flash_func
#else
#define in_ram
#endif
#endif

#include "types.h"
#include "z80.h"
#include "sms.h"
#include "vdp.h"
#include "render.h"
#include "sn76496.h"
#include "system.h"
#include "loadrom.h"
#include "hash.h"

char unalChar(const char *adr);

#ifdef __cplusplus
}
#endif

#endif /* _SHARED_H_ */
