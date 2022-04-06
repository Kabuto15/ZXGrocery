#ifndef GAME__H
#define GAME__H

#include "main.h"
#include "sound.h"

#define GOODS_QTY_DEF   5
#define GAME_LEVELS     5
#define DELAY_LONG      2000
#define WINNING_VALUE   100


struct GOODS_WAREHOUSE {
    uint8_t currentPrice[GOODS_QTY_DEF];
    uint8_t currentInventory[GOODS_QTY_DEF];
};

enum NEW_SHOPER_STATUS { NS_IDLE, NS_COMING, NS_JUST_CAME_RAND, NS_WAITING_FOR_BILL, NS_QUIT };

enum GAME_EXIT_STATE { GAME_RUNNING, GAME_OVER };
struct GAME_STATUS {
    uint8_t gameLevel;
    uint8_t currentFunds;
    uint8_t customersNo;
};


void initialiseColour(unsigned int count, struct sp1_cs *c);
void gameScreenLoop(uint16_t rndSeed);
void initGameVariables(uint16_t rndSeed);
void gameSpritesInit(void);
void gameSpritesSetAway(void);
void playIntro(void);

#endif