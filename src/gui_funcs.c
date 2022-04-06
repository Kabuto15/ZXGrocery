#include "gui_funcs.h"

struct sp1_pss ps1;



void displayText(struct sp1_Rect screen, char *text, uint8_t x, uint8_t y)
{
    struct sp1_pss ps0;
    ps0.bounds    = &screen;
    ps0.flags     = SP1_PSSFLAG_INVALIDATE;
    ps0.attr_mask = 0;
    ps0.visit     = 0;

    sp1_SetPrintPos(&ps0, y, x);
    sp1_PrintString(&ps0, (uint8_t *)text);
}


void displayScreen(struct sp1_Rect screen, uint8_t *tiles, uint8_t *pTiles, uint8_t uiTilesLen, uint8_t uiTilesBase, uint8_t uiYStartPos)
{
    uint8_t i;
    uint8_t *pt2 =tiles;
    
    for (i = 0; i < uiTilesLen; i++, pt2 += 8) {
        sp1_TileEntry(uiTilesBase + i, pt2);
    }

    ps1.bounds    = &screen;
    ps1.flags     = SP1_PSSFLAG_INVALIDATE;
    ps1.visit     = 0;

    sp1_SetPrintPos(&ps1, uiYStartPos, 0);
    sp1_PrintString(&ps1, pTiles);
    sp1_UpdateNow();

}


void initialiseColour(unsigned int count, struct sp1_cs *c)
{
    (void)count;

    c->attr_mask = SP1_AMASK_INK;
    c->attr      = INK_BLACK;
}
