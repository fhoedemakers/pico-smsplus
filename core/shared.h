#ifndef _SHARED_H_
#define _SHARED_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <signal.h>
#include <math.h>
#include <limits.h>

#ifndef PATH_MAX
#ifdef  MAX_PATH
#define PATH_MAX    MAX_PATH
#else
#define PATH_MAX    1024
#endif
#endif

#ifndef SMS_FILE_ADDR
#define SMS_FILE_ADDR 0x10080000
#endif

#include "macros.h"
#include "z80.h"
#include "sms.h"
#include "pio.h"
#include "memz80.h"
#include "vdp.h"
#include "render.h"
#include "tms.h"
#include "sn76489.h"
#include "emu2413.h"
#include "ym2413.h"
#include "fmintf.h"
#include "stream.h"
#include "sound.h"
#include "system.h"
#include "error.h"

#include "state.h"
#include "loadrom.h"
#include "hash.h"

#endif /* _SHARED_H_ */
