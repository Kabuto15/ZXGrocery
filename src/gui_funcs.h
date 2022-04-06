#ifndef GUI_FUNCS__H
#define GUI_FUNCS__H

#include "main.h"


void displayText(struct sp1_Rect screen, char *text, uint8_t x, uint8_t y);
void displayScreen(struct sp1_Rect screen, uint8_t *tiles, uint8_t *pTiles, uint8_t uiTilesLen, uint8_t uiTilesBase, uint8_t uiYStartPos);

#endif