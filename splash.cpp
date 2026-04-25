#include "menu.h"
#include "FrensHelpers.h"
#include <cstring>

// called by menu.cpp
// shows emulator specific splash screen
static int fgcolorSplash = DEFAULT_FGCOLOR;
static int bgcolorSplash =DEFAULT_BGCOLOR;
void splash()
{
    char s[SCREEN_COLS + 1];
    ClearScreen(bgcolorSplash);

    strcpy(s, "Pico-");
    putText(SCREEN_COLS / 2 - (strlen(s) + 4) / 2, 2, s, fgcolorSplash, bgcolorSplash);

    putText((SCREEN_COLS / 2 - (strlen(s)) / 2) + 3, 2, "S", CRED, bgcolorSplash);
    putText((SCREEN_COLS / 2 - (strlen(s)) / 2) + 4, 2, "M", CGREEN, bgcolorSplash);
    putText((SCREEN_COLS / 2 - (strlen(s)) / 2) + 5, 2, "S", CBLUE, bgcolorSplash);
    putText((SCREEN_COLS / 2 - (strlen(s)) / 2) + 6, 2, "+", fgcolorSplash, bgcolorSplash);

    strcpy(s, "Sega Master System/Game Gear");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 4, s, fgcolorSplash, bgcolorSplash);

    strcpy(s, "emulator for RP2040/RP2350");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 5, s, fgcolorSplash, bgcolorSplash);
   
    strcpy(s, "Pico Port");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 7, s, fgcolorSplash, bgcolorSplash);
    strcpy(s, "@frenskefrens");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 8, s, CBLUE, bgcolorSplash);
#if !HSTX
    strcpy(s, "DVI Support");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 11, s, fgcolorSplash, bgcolorSplash);
    strcpy(s, "@shuichi_takano");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 12, s, CBLUE, bgcolorSplash);
#else
    strcpy(s, "HSTX video driver _ I2S audio___");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 11, s, fgcolorSplash, bgcolorSplash);
    strcpy(s, "__@fliperama86____@frenskefrens__");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 12, s, CBLUE, bgcolorSplash);
#endif
    strcpy(s, "(S)NES/WII controller support");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 15, s, fgcolorSplash, bgcolorSplash);

    strcpy(s, "@PaintYourDragon @adafruit");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 16, s, CBLUE, bgcolorSplash);

    strcpy(s, "PCB Design");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 19, s, fgcolorSplash, bgcolorSplash);

    strcpy(s, "@johnedgarpark DynaMight");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 20, s, CBLUE, bgcolorSplash);

    strcpy(s, "3D Case design,artwork & metadata");
    putText(2, 22, s, fgcolorSplash, bgcolorSplash);
    strcpy(s, "DynaMight");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 23, s, CBLUE, bgcolorSplash);

    strcpy(s, "https://github.com/");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 25, s, CBLUE, bgcolorSplash);
    strcpy(s, "fhoedemakers/pico-smsplus");
    putText(SCREEN_COLS / 2 - strlen(s) / 2, 26, s, CBLUE, bgcolorSplash);
}