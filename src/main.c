#pragma output REGISTER_SP = 0xD000

#include "main.h"


struct sp1_Rect fullScreenDef = {0, 0, 32, 24};
static void gameIntroductionScreenDisplay(void);



int main()
{
    uint16_t rndSeed;

    zx_border(INK_BLACK);
    sp1_Initialize( SP1_IFLAG_MAKE_ROTTBL | SP1_IFLAG_OVERWRITE_TILES | SP1_IFLAG_OVERWRITE_DFILE,
                    INK_BLACK | PAPER_WHITE,
                    ' ' );
    sp1_Invalidate(&fullScreenDef);
    gameSpritesInit();
    gameSpritesSetAway();
    while(1)
    {
        gameIntroductionScreenDisplay();

        do {
            rndSeed++;
        } while (0==in_inkey());

        gameScreenLoop(rndSeed);      
    }

    return 0;
}


static void gameIntroductionScreenDisplay(void) {

    sp1_ClearRectInv(fullScreenDef, INK_BLACK | PAPER_WHITE, 32, SP1_RFLAG_TILE | SP1_RFLAG_COLOUR);
    gameSpritesSetAway();
    displayText(&fullScreenDef, "\x14\x42 *** ZX GROCERY 2022 ***", 0, 1);
    displayText(&fullScreenDef, "\x14\x38 Calculate price of products", 0, 3);
    displayText(&fullScreenDef, "\x14\x38 To win - earn 100$!", 0, 4);
    displayText(&fullScreenDef, "\x14\x38 Control Keys", 0, 7);
    displayText(&fullScreenDef, "\x14\x38 o | l - groc. curr. cntrl", 0, 8);
    displayText(&fullScreenDef, "\x14\x38 m - buying from the warehouse", 0, 9);
    displayText(&fullScreenDef, "\x14\x38 q - restart the game", 0, 10);
    displayText(&fullScreenDef, "\x14\x78 Press any key to continue", 0, 13);
    sp1_UpdateNow();
    playIntro();
}
