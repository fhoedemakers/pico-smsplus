#ifndef MENU_H
#define MENU_H
#define ROMSELECT
#define ROMINFOFILE "/currentloadedrom.txt"
#define SWVERSION "VX.X"

#if PICO_RP2350
#if __riscv
#define PICOHWNAME_ "rp2350-riscv"
#else
#define PICOHWNAME_ "rp2350-arm"
#endif
#else
#define PICOHWNAME_ "rp2040"
#endif

void processinput(DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem, bool ignorepushed, char *gamepadType );
void menu(uintptr_t NES_FILE_ADDR, char *errorMessage, bool isFatalError, bool reset);
char getcharslicefrom8x8font(char c, int rowInChar);
int ProcessAfterFrameIsRendered();
#endif