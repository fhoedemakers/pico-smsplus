#include <stdio.h>
#include <string.h>

#include "FrensHelpers.h"
#include <pico.h>
#include <memory>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include <dvi/dvi.h>
#include <util/exclusive_proc.h>
#include <gamepad.h>
#include "hardware/watchdog.h"
#include "RomLister.h"
#include "menu.h"
#include "nespad.h"
#include "wiipad.h"

#include "font_8x8.h"
#define FONT_CHAR_WIDTH 8
#define FONT_CHAR_HEIGHT 8
#define FONT_N_CHARS 95
#define FONT_FIRST_ASCII 32
#define SCREEN_COLS 32
#define SCREEN_ROWS 29

#define STARTROW 2
#define ENDROW 25
#define PAGESIZE (ENDROW - STARTROW + 1)

#define VISIBLEPATHSIZE (SCREEN_COLS - 3)

extern util::ExclusiveProc exclProc_;
extern std::unique_ptr<dvi::DVI> dvi_;
void screenMode(int incr);

// Just for now, reuse the NesPalette from the InfoNes emulator to render colors
#define CC(x) (((x >> 1) & 15) | (((x >> 6) & 15) << 4) | (((x >> 11) & 15) << 8))
const WORD NesPalette[64] = {
    CC(0x39ce), CC(0x1071), CC(0x0015), CC(0x2013), CC(0x440e), CC(0x5402), CC(0x5000), CC(0x3c20),
    CC(0x20a0), CC(0x0100), CC(0x0140), CC(0x00e2), CC(0x0ceb), CC(0x0000), CC(0x0000), CC(0x0000),
    CC(0x5ef7), CC(0x01dd), CC(0x10fd), CC(0x401e), CC(0x5c17), CC(0x700b), CC(0x6ca0), CC(0x6521),
    CC(0x45c0), CC(0x0240), CC(0x02a0), CC(0x0247), CC(0x0211), CC(0x0000), CC(0x0000), CC(0x0000),
    CC(0x7fff), CC(0x1eff), CC(0x2e5f), CC(0x223f), CC(0x79ff), CC(0x7dd6), CC(0x7dcc), CC(0x7e67),
    CC(0x7ae7), CC(0x4342), CC(0x2769), CC(0x2ff3), CC(0x03bb), CC(0x0000), CC(0x0000), CC(0x0000),
    CC(0x7fff), CC(0x579f), CC(0x635f), CC(0x6b3f), CC(0x7f1f), CC(0x7f1b), CC(0x7ef6), CC(0x7f75),
    CC(0x7f94), CC(0x73f4), CC(0x57d7), CC(0x5bf9), CC(0x4ffe), CC(0x0000), CC(0x0000), CC(0x0000)};

#define CBLACK 15
#define CWHITE 48
#define CRED 6
#define CGREEN 10
#define CBLUE 2
#define CLIGHTBLUE 0x2C
#define DEFAULT_FGCOLOR CBLACK // 60
#define DEFAULT_BGCOLOR CWHITE

static int fgcolor = DEFAULT_FGCOLOR;
static int bgcolor = DEFAULT_BGCOLOR;

struct charCell
{
    uint8_t fgcolor : 4;
    uint8_t bgcolor : 4;
    char charvalue;
};

#define SCREENBUFCELLS SCREEN_ROWS *SCREEN_COLS
static charCell screenBuffer[SCREENBUFCELLS];
;

static WORD *WorkLineRom = nullptr;

static constexpr int LEFT = 0x00000004;
static constexpr int RIGHT = 0x00000008;
static constexpr int UP = 0x00000001;
static constexpr int DOWN = 0x00000002;
static constexpr int SELECT = 0x00000002;
static constexpr int START = 0x00000001;
static constexpr int A = 0x00000020;
static constexpr int B = 0x00000010;
// static constexpr int X = 1 << 8;
// static constexpr int Y = 1 << 9;

char getcharslicefrom8x8font(char c, int rowInChar)
{
    sizeof(screenBuffer);
    return font_8x8[(c - FONT_FIRST_ASCII) + (rowInChar)*FONT_N_CHARS];
}

void RomSelect_DrawLine(int line, int selectedRow)
{
    WORD fgcolor, bgcolor;
    memset(WorkLineRom, 0, 640);

    for (auto i = 0; i < SCREEN_COLS; ++i)
    {
        int charIndex = i + line / FONT_CHAR_HEIGHT * SCREEN_COLS;
        int row = charIndex / SCREEN_COLS;
        uint c = screenBuffer[charIndex].charvalue;
        if (row == selectedRow)
        {
            fgcolor = NesPalette[screenBuffer[charIndex].bgcolor];
            bgcolor = NesPalette[screenBuffer[charIndex].fgcolor];
        }
        else
        {
            fgcolor = NesPalette[screenBuffer[charIndex].fgcolor];
            bgcolor = NesPalette[screenBuffer[charIndex].bgcolor];
        }

        int rowInChar = line % FONT_CHAR_HEIGHT;
        char fontSlice = getcharslicefrom8x8font(c, rowInChar); // font_8x8[(c - FONT_FIRST_ASCII) + (rowInChar)*FONT_N_CHARS];
        for (auto bit = 0; bit < 8; bit++)
        {
            if (fontSlice & 1)
            {
                *WorkLineRom = fgcolor;
            }
            else
            {
                *WorkLineRom = bgcolor;
            }
            fontSlice >>= 1;
            WorkLineRom++;
        }
    }
    return;
}

void drawline(int scanline, int selectedRow)
{
    // RomSelect_PreDrawLine(scanline);
    // RomSelect_DrawLine(scanline - 4, selectedRow);
    // InfoNES_PostDrawLine(scanline);

    auto b = dvi_->getLineBuffer();
    WorkLineRom = b->data() + 32;
    RomSelect_DrawLine(scanline - 4, selectedRow);
    dvi_->setLineBuffer(scanline, b);
}

static void putText(int x, int y, const char *text, int fgcolor, int bgcolor)
{

    if (text != nullptr)
    {
        auto index = y * SCREEN_COLS + x;
        auto maxLen = strlen(text);
        if (strlen(text) > SCREEN_COLS - x)
        {
            maxLen = SCREEN_COLS - x;
        }
        while (index < SCREENBUFCELLS && *text && maxLen > 0)
        {
            screenBuffer[index].charvalue = *text++;
            screenBuffer[index].fgcolor = fgcolor;
            screenBuffer[index].bgcolor = bgcolor;
            index++;
            maxLen--;
        }
    }
}

void DrawScreen(int selectedRow)
{
    for (auto line = 4; line < 236; line++)
    {
        drawline(line, selectedRow);
    }
}

void ClearScreen(charCell *screenBuffer, int color)
{
    for (auto i = 0; i < SCREENBUFCELLS; i++)
    {
        screenBuffer[i].bgcolor = color;
        screenBuffer[i].fgcolor = color;
        screenBuffer[i].charvalue = ' ';
    }
}

void displayRoms(Frens::RomLister romlister, int startIndex)
{
    char buffer[ROMLISTER_MAXPATH + 4];
    auto y = STARTROW;
    auto entries = romlister.GetEntries();
    ClearScreen(screenBuffer, bgcolor);
    putText(1, 0, "Choose a rom to play:", fgcolor, bgcolor);
    putText(1, SCREEN_ROWS - 1, "A: Select, B: Back", fgcolor, bgcolor);
    for (auto index = startIndex; index < romlister.Count(); index++)
    {
        if (y <= ENDROW)
        {
            auto info = entries[index];
            if (info.IsDirectory)
            {
                snprintf(buffer, sizeof(buffer), "D %s", info.Path);
            }
            else
            {
                snprintf(buffer, sizeof(buffer), "R %s", info.Path);
            }

            putText(1, y, buffer, fgcolor, bgcolor);
            y++;
        }
    }
}

void DisplayFatalError(char *error)
{
    ClearScreen(screenBuffer, bgcolor);
    putText(0, 0, "Fatal error:", fgcolor, bgcolor);
    putText(1, 3, error, fgcolor, bgcolor);
    while (true)
    {
        auto frameCount = ProcessAfterFrameIsRendered();
        DrawScreen(-1);
    }
}

void DisplayEmulatorErrorMessage(char *error)
{
    DWORD PAD1_Latch, PAD1_Latch2, pdwSystem;
    ClearScreen(screenBuffer, bgcolor);
    putText(0, 0, "Error occured:", fgcolor, bgcolor);
    putText(0, 3, error, fgcolor, bgcolor);
    putText(0, ENDROW, "Press a button to continue.", fgcolor, bgcolor);
    while (true)
    {
        auto frameCount = ProcessAfterFrameIsRendered();
        DrawScreen(-1);
        processinput(&PAD1_Latch, &PAD1_Latch2, &pdwSystem, false);
        if (PAD1_Latch > 0)
        {
            return;
        }
    }
}

void showSplashScreen()
{
    DWORD PAD1_Latch, PAD1_Latch2, pdwSystem;
    char s[SCREEN_COLS];
    ClearScreen(screenBuffer, bgcolor);

    strcpy(s, "Pico-");
    putText(SCREEN_COLS / 2 - (strlen(s) + 4) / 2, 2, s, fgcolor, bgcolor);

    putText((SCREEN_COLS / 2 - (strlen(s)) / 2) + 3, 2, "S", CRED, bgcolor);
    putText((SCREEN_COLS / 2 - (strlen(s)) / 2) + 4, 2, "M", CGREEN, bgcolor);
    putText((SCREEN_COLS / 2 - (strlen(s)) / 2) + 5, 2, "S", CBLUE, bgcolor);
    putText((SCREEN_COLS / 2 - (strlen(s)) / 2) + 6, 2, "+", fgcolor, bgcolor);

    strcpy(s, "Sega Master System");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 4, s, fgcolor, bgcolor);
    strcpy(s, "emulator for RP2040");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 5, s, fgcolor, bgcolor);
   
    strcpy(s, "Pico Port");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 9, s, fgcolor, bgcolor);
    strcpy(s, "@frenskefrens");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 10, s, CLIGHTBLUE, bgcolor);

    strcpy(s, "DVI Support");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 13, s, fgcolor, bgcolor);
    strcpy(s, "@shuichi_takano");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 14, s, CLIGHTBLUE, bgcolor);

    strcpy(s, "(S)NES/WII controller support");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 17, s, fgcolor, bgcolor);

    strcpy(s, "@PaintYourDragon @adafruit");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 18, s, CLIGHTBLUE, bgcolor);

    strcpy(s, "PCB Design");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 21, s, fgcolor, bgcolor);

    strcpy(s, "@johnedgarpark");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 22, s, CLIGHTBLUE, bgcolor);

    strcpy(s, "https://github.com/");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 25, s, CLIGHTBLUE, bgcolor);
    strcpy(s, "fhoedemakers/pico-smsplus");
    putText(1, 26, s, CLIGHTBLUE, bgcolor);
    int startFrame = -1;
    while (true)
    {
        auto frameCount = ProcessAfterFrameIsRendered();
        if (startFrame == -1)
        {
            startFrame = frameCount;
        }
        DrawScreen(-1);
        processinput(&PAD1_Latch, &PAD1_Latch2, &pdwSystem, false);
        if (PAD1_Latch > 0 || (frameCount - startFrame) > 1000)
        {
             return;
        }
        if ((frameCount % 30) == 0)
        {
            for (auto i = 0; i < SCREEN_COLS; i++)
            {
                auto col = rand() % 63;
                putText(i, 0, " ", col, col);
                col = rand() % 63;
                putText(i, SCREEN_ROWS - 1, " ", col, col);
            }
            for (auto i = 1; i < SCREEN_ROWS - 1; i++)
            {
                auto col = rand() % 63;
                putText(0, i, " ", col, col);
                col = rand() % 63;
                putText(SCREEN_COLS - 1, i, " ", col, col);
            }
        }
    }
}

void screenSaver()
{
    DWORD PAD1_Latch, PAD1_Latch2, pdwSystem;
    WORD frameCount;
    while (true)
    {
        frameCount = ProcessAfterFrameIsRendered();
        DrawScreen(-1);
        processinput(&PAD1_Latch, &PAD1_Latch2, &pdwSystem, false);

        if (PAD1_Latch > 0)
        {
            return;
        }
        if ((frameCount % 3) == 0)
        {
            auto color = rand() % 63;
            auto row = rand() % SCREEN_ROWS;
            auto column = rand() % SCREEN_COLS;
            putText(column, row, " ", color, color);
        }
    }
}
void clearinput()
{
    DWORD PAD1_Latch, PAD1_Latch2, pdwSystem;
    while (1)
    {
        ProcessAfterFrameIsRendered();
        DrawScreen(-1);
        processinput(&PAD1_Latch, &PAD1_Latch2, &pdwSystem, true);
        if (PAD1_Latch == 0)
        {
            break;
        }
    }
}
// Global instances of local vars in romselect() some used in Lambda expression later on
static char *selectedRomOrFolder;
static uintptr_t FLASH_ADDRESS;
static bool errorInSavingRom = false;
static char *globalErrorMessage;


//
BYTE dirbuffer[10 * 1024];
void menu(uintptr_t NES_FILE_ADDR, char *errorMessage, bool isFatal, bool reset)
{
    FLASH_ADDRESS = NES_FILE_ADDR;
    int firstVisibleRowINDEX = 0;
    int selectedRow = STARTROW;
    char currentDir[FF_MAX_LFN];
    int totalFrames = -1;

    globalErrorMessage = errorMessage;
    FRESULT fr;
    DWORD PAD1_Latch, PAD1_Latch2, pdwSystem;

    int horzontalScrollIndex = 0;
    printf("Starting Menu\n");
    size_t ramsize;
    // Borrow Emulator RAM buffer for screen.
    // screenBuffer = (charCell *)InfoNes_GetRAM(&ramsize);
    /// size_t chr_size;
    // Borrow ChrBuffer to store directory contents
    // void *buffer = InfoNes_GetChrBuf(&chr_size);
    Frens::RomLister romlister(dirbuffer, sizeof(dirbuffer));
    clearinput();
    if (strlen(errorMessage) > 0)
    {
        if (isFatal) // SD card not working, show error
        {
            DisplayFatalError(errorMessage);
        }
        else
        {
            DisplayEmulatorErrorMessage(errorMessage); // Emulator cannot start, show error
        }
    }
    else
    {
        // Show splash screen, only when not reset from emulation
        if ( reset == false )
        {
            showSplashScreen();
        } else sleep_ms(300);
    }
    romlister.list("/");
    displayRoms(romlister, firstVisibleRowINDEX);
    while (1)
    {

        auto frameCount = ProcessAfterFrameIsRendered();
        auto index = selectedRow - STARTROW + firstVisibleRowINDEX;
        auto entries = romlister.GetEntries();
        selectedRomOrFolder = (romlister.Count() > 0) ? entries[index].Path : nullptr;
        errorInSavingRom = false;
        DrawScreen(selectedRow);
        processinput(&PAD1_Latch, &PAD1_Latch2, &pdwSystem, false);

        if (PAD1_Latch > 0 || pdwSystem > 0)
        {
            totalFrames = frameCount; // Reset screenSaver
            // reset horizontal scroll of highlighted row
            horzontalScrollIndex = 0;
            putText(3, selectedRow, selectedRomOrFolder, fgcolor, bgcolor);

            // if ((PAD1_Latch & Y) == Y)
            // {
            //     fgcolor++;
            //     if (fgcolor > 63)
            //     {
            //         fgcolor = 0;
            //     }
            //     printf("fgColor++ : %02d (%04x)\n", fgcolor, NesPalette[fgcolor]);
            //     displayRoms(romlister, firstVisibleRowINDEX);
            // }
            // else if ((PAD1_Latch & X) == X)
            // {
            //     bgcolor++;
            //     if (bgcolor > 63)
            //     {
            //         bgcolor = 0;
            //     }
            //     printf("bgColor++ : %02d (%04x)\n", bgcolor, NesPalette[bgcolor]);
            //     displayRoms(romlister, firstVisibleRowINDEX);
            // }
            // else
            if ((PAD1_Latch & UP) == UP && selectedRomOrFolder)
            {
                if (selectedRow > STARTROW)
                {
                    selectedRow--;
                }
                else
                {
                    if (firstVisibleRowINDEX > 0)
                    {
                        firstVisibleRowINDEX--;
                        displayRoms(romlister, firstVisibleRowINDEX);
                    }
                }
            }
            else if ((PAD1_Latch & DOWN) == DOWN && selectedRomOrFolder)
            {
                if (selectedRow < ENDROW && (index) < romlister.Count() - 1)
                {
                    selectedRow++;
                }
                else
                {
                    if (index < romlister.Count() - 1)
                    {
                        firstVisibleRowINDEX++;
                        displayRoms(romlister, firstVisibleRowINDEX);
                    }
                }
            }
            else if ((PAD1_Latch & LEFT) == LEFT && selectedRomOrFolder)
            {
                firstVisibleRowINDEX -= PAGESIZE;
                if (firstVisibleRowINDEX < 0)
                {
                    firstVisibleRowINDEX = 0;
                }
                selectedRow = STARTROW;
                displayRoms(romlister, firstVisibleRowINDEX);
            }
            else if ((PAD1_Latch & RIGHT) == RIGHT && selectedRomOrFolder)
            {
                if (firstVisibleRowINDEX + PAGESIZE < romlister.Count())
                {
                    firstVisibleRowINDEX += PAGESIZE;
                }
                selectedRow = STARTROW;
                displayRoms(romlister, firstVisibleRowINDEX);
            }
            else if ((PAD1_Latch & B) == B)
            {
                fr = f_getcwd(currentDir, 40);
                if (fr != FR_OK)
                {
                    printf("Cannot get current dir: %d\n", fr);
                }
                if (strcmp(currentDir, "/") != 0)
                {
                    romlister.list("..");
                    firstVisibleRowINDEX = 0;
                    selectedRow = STARTROW;
                    displayRoms(romlister, firstVisibleRowINDEX);
                }
            }
            else if ((pdwSystem & START) == START && (pdwSystem & SELECT) != SELECT)
            {
                // reboot and start emulator with currently loaded game
                // Create a file /START indicating not to reflash the already flashed game
                // The emulator will delete this file after loading the game
                FRESULT fr;
                FIL fil;
                printf("Creating /START\n");
                fr = f_open(&fil, "/START", FA_CREATE_ALWAYS | FA_WRITE);
                if (fr == FR_OK)
                {
                    auto bytes = f_puts("START", &fil);
                    printf("Wrote %d bytes\n", bytes);
                    fr = f_close(&fil);
                    if (fr != FR_OK)
                    {
                        printf("Cannot close file /START:%d\n", fr);
                    }
                }
                else
                {
                    printf("Cannot create file /START:%d\n", fr);
                }
                break; // reboot
            }
            else if ((PAD1_Latch & A) == A && selectedRomOrFolder)
            {
                if (entries[index].IsDirectory)
                {
                    romlister.list(selectedRomOrFolder);
                    firstVisibleRowINDEX = 0;
                    selectedRow = STARTROW;
                    displayRoms(romlister, firstVisibleRowINDEX);
                }
                else
                {
                    FRESULT fr;
                    FIL fil;
                    char curdir[256];

                    fr = f_getcwd(curdir, sizeof(curdir));
                    printf("Current dir: %s\n", curdir);
                    // Create file containing full path name currently loaded rom
                    // The contents of this file will be used by the emulator to flash and start the correct rom in main.cpp
                    printf("Creating %s\n", ROMINFOFILE);
                    fr = f_open(&fil, ROMINFOFILE, FA_CREATE_ALWAYS | FA_WRITE);
                    if (fr == FR_OK)
                    {
                        for (auto i = 0; i < strlen(curdir); i++)
                        {

                            int x = f_putc(curdir[i], &fil);
                            printf("%c", curdir[i]);
                            if (x < 0)
                            {
                                snprintf(globalErrorMessage, 40, "Error writing file %d", fr);
                                printf("%s\n", globalErrorMessage);
                                errorInSavingRom = true;
                                break;
                            }
                        }
                        f_putc('/', &fil);
                        printf("%c", '/');
                        for (auto i = 0; i < strlen(selectedRomOrFolder); i++)
                        {

                            int x = f_putc(selectedRomOrFolder[i], &fil);
                            printf("%c", selectedRomOrFolder[i]);
                            if (x < 0)
                            {
                                snprintf(globalErrorMessage, 40, "Error writing file %d", fr);
                                printf("%s\n", globalErrorMessage);
                                errorInSavingRom = true;
                                break;
                            }
                        }
                        printf("\n");
                    }
                    else
                    {
                        printf("Cannot create %s:%d\n", ROMINFOFILE, fr);
                        snprintf(globalErrorMessage, 40, "Cannot create %s:%d", ROMINFOFILE, fr);
                        errorInSavingRom = true;
                    }
                    f_close(&fil);
                    // break out of loop and reboot
                    // rom will be flashed and started by main.cpp
                    // Cannot flash here because of lockups (when using wii controller) and sound issues
                    break;
                }
            }
        }
        // scroll selected row horizontally if textsize exceeds rowlength
        if (selectedRomOrFolder)
        {
            if ((frameCount % 30) == 0)
            {
                if (strlen(selectedRomOrFolder + horzontalScrollIndex) > VISIBLEPATHSIZE)
                {
                    horzontalScrollIndex++;
                }
                else
                {
                    horzontalScrollIndex = 0;
                }
                putText(3, selectedRow, selectedRomOrFolder + horzontalScrollIndex, fgcolor, bgcolor);
            }
        }
        if (totalFrames == -1)
        {
            totalFrames = frameCount;
        }
        if ((frameCount - totalFrames) > 800)
        {
            printf("Starting screensaver\n");
            totalFrames = -1;
            screenSaver();
            displayRoms(romlister, firstVisibleRowINDEX);
        }
    } // while 1
      // Wait until user has released all buttons
    clearinput();
    
#if WII_PIN_SDA >= 0 and WII_PIN_SCL >= 0
    wiipad_end();
#endif

    // Don't return from this function call, but reboot in order to get avoid several problems with sound and lockups (WII-pad)
    // After reboot the emulator will and flash start the selected game.
    printf("Rebooting...\n");
    watchdog_enable(100, 1);
    while (1)
        ;
    // Never return
}