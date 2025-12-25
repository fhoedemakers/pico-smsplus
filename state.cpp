#include "state.h"
#include "FrensHelpers.h"
#include "ff.h"
#include "smsplus/shared.h"

int Emulator_SaveState(const char* path)
{
    FIL fil;
    FRESULT fr = f_open(&fil, path, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        printf("Cannot open save state file: %d (%s)\n", fr, path);
        return -1;
    }

    bool ok = system_save_state(&fil);
    FRESULT frc = f_close(&fil);

    if (!ok) {
        printf("system_save_state failed (%s)\n", path);
        return -1;
    }
    if (frc != FR_OK) {
        printf("Error closing save state file: %d (%s)\n", frc, path);
        return -1;
    }
    return 0;
}
int Emulator_LoadState(const char* path)
{
    FIL fil;
    FRESULT fr = f_open(&fil, path, FA_READ);
    if (fr != FR_OK) {
        printf("Cannot open load state file: %d (%s)\n", fr, path);
        return -1;
    }
    system_shutdown(); // shutdown before loading state
    system_init(SMS_AUD_RATE); // re-init system
    //system_reset(); // reset system
    bool ok = system_load_state(&fil);
    FRESULT frc = f_close(&fil);

    if (!ok) {
        printf("system_load_state failed (%s)\n", path);
        return -1;
    }
    if (frc != FR_OK) {
        printf("Error closing load state file: %d (%s)\n", frc, path);
        return -1;
    }
    return 0;
}