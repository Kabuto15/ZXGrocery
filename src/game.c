#include "game.h"
#include "gui_funcs.h"
#include "graphics_raw_data.h"
#include "sound/bit.h"
#include "main.h"




extern uint8_t circle_masked_left[];
extern uint8_t circle_masked_right[];
extern uint8_t runner_f1[];


//struct sp1_ss *spriceCurrDistr;
struct sp1_ss *spriceCurrGrocery;
struct sp1_ss *animatedSprite;

const char *goodsNames[GOODS_QTY_DEF] = {
    "Bananas",
    "Apples",
    "Oranges",
    "Potatos",
    "Tomatos"
};

const uint8_t goodsScreenPos[GOODS_QTY_DEF] = {
    4,6,8,10,12
};

const uint8_t currsorSelPos[GOODS_QTY_DEF] = {
    32,48,64,80,96
};

const uint8_t gameLevelCustomersNo[GAME_LEVELS] = {
    5,10,15,20,25
};

const uint8_t gameLevelCustomersAppearance[GAME_LEVELS] = {
    8,6,4,3,2
};

const uint8_t gameLevelDistributorPriceChange[GAME_LEVELS] = {
    0,0,1,1,1
};

const uint8_t levelRndProdType[] = { 2, 3, 4, 2, 3};
const uint8_t levelRndProdAmount[] = {1, 1, 1, 3, 4};


struct GOODS_WAREHOUSE distributor;
struct GOODS_WAREHOUSE groceryShopPlayer;
struct GAME_STATUS gameStatus;
enum GAME_EXIT_STATE gameExitStatus;
enum NEW_SHOPER_STATUS eNewShoper;
uint8_t newShoperProductsType[GOODS_QTY_DEF];
uint8_t newShoperProductsAmount[GOODS_QTY_DEF];
uint8_t newShoperBill;
uint8_t newShoperNoOfProducts;
uint8_t newShoperCountDown;
uint16_t newShoperGameTick;
uint8_t animation_offset;
uint8_t animation_x;
uint8_t textBuffer[25];


static void displayNewShoperRequest(void);
static uint8_t newShoperGiveBillNumber(void);
static void displayCurrentWarehouse(void);
static void displayBottomScreen(void);
static void newShoperRandomProducts(uint8_t rndSeek);
static void changeDistributorPrices(void);
static void buyFromDistributor(uint8_t itemPos);
static enum GAME_EXIT_STATE newShoperStateMachine(uint8_t rndSeek);
static enum GAME_EXIT_STATE finalizeNewShoperRequest(void);
static enum GAME_EXIT_STATE applyGroceryPenalty(uint8_t uiPenalty);
static void displayTopScreen(void);
static void playPositive(void);
static void playNegative(void);


void gameSpritesInit(void)
{
    spriceCurrGrocery = sp1_CreateSpr(SP1_DRAW_MASK2LB, SP1_TYPE_2BYTE, 3, (int)circle_masked_right, 0);
    sp1_AddColSpr(spriceCurrGrocery, SP1_DRAW_MASK2RB,  SP1_TYPE_2BYTE, 0, 0);
    sp1_IterateSprChar(spriceCurrGrocery, initialiseColour);

    /* distributor coursor */
    /*
    spriceCurrDistr = sp1_CreateSpr(SP1_DRAW_MASK2LB, SP1_TYPE_2BYTE, 3, (int)circle_masked_left, 0);
    sp1_AddColSpr(spriceCurrDistr, SP1_DRAW_MASK2RB,  SP1_TYPE_2BYTE, 0, 0);
    sp1_IterateSprChar(spriceCurrDistr, initialiseColour);
    */

    animatedSprite = sp1_CreateSpr(SP1_DRAW_LOAD1LB, SP1_TYPE_1BYTE, 2, (int)runner_f1, 0);
    sp1_AddColSpr(animatedSprite, SP1_DRAW_LOAD1RB, SP1_TYPE_1BYTE, 0, 0);
    sp1_IterateSprChar(animatedSprite, initialiseColour);
  
}

void gameSpritesSetAway(void)
{
    sp1_MoveSprPix(spriceCurrGrocery, &fullScreenDef, 0, 300, 0);
    //sp1_MoveSprPix(spriceCurrDistr, &fullScreenDef, 0, 300, 0);
}

void gameScreenLoop(uint16_t rndSeed) {
    uint8_t uiDistributorCurrPos;
    uint8_t uiGroceryCurrPos;
    uint16_t uiCustomerGameTick = 0;
    uint16_t mod_value = 0;
    enum GAME_EXIT_STATE eExitState = GAME_RUNNING;

    uiDistributorCurrPos = 0;
    uiGroceryCurrPos = 0;
    playPositive();
    initGameVariables(rndSeed);
    sp1_ClearRectInv(fullScreenDef, INK_BLACK | PAPER_WHITE, 32, SP1_RFLAG_TILE | SP1_RFLAG_COLOUR);
    displayBottomScreen(); 
    displayTopScreen();
      
    sp1_MoveSprPix(spriceCurrGrocery, &fullScreenDef, 0, 230, currsorSelPos[uiDistributorCurrPos]);
    //sp1_MoveSprPix(spriceCurrDistr, &fullScreenDef, 0, 16, currsorSelPos[uiGroceryCurrPos]);
    sp1_UpdateNow();

    displayCurrentWarehouse();
    animation_x=16;
    animation_offset=16;

    while(GAME_OVER!=eExitState)
    {
        uiCustomerGameTick++;
        rndSeed++;

        if (NS_IDLE == eNewShoper)
        {
            animation_offset = 16;
            mod_value = gameLevelCustomersAppearance[gameStatus.gameLevel]*100;
            if (uiCustomerGameTick%mod_value == 0) // new customer coming
                eNewShoper = NS_COMING;

        }
        else if (NS_COMING == eNewShoper)
        {
            animatedSprite->frame = (void*)animation_offset;
            sp1_MoveSprPix(animatedSprite, &fullScreenDef, 0, animation_x++, 160);
            if (0 == uiCustomerGameTick % 3) bit_beep(10, 300);
            animation_offset+=16;
            if( animation_offset == 128 )
                animation_offset = 0;
            
            if (animation_x == 90) {
                eNewShoper = NS_JUST_CAME_RAND;
                newShoperCountDown = 60;
                newShoperGameTick = 0;
                uiCustomerGameTick = 0;
                sp1_MoveSprPix(animatedSprite, &fullScreenDef, 0, 0, 200); // hide
            }
        }

        /* distributor - disabled, to be developed in the future -> 2032! */
        /*
        if (in_key_pressed(IN_KEY_SCANCODE_w)) {
             if (uiDistributorCurrPos > 0)
                 uiDistributorCurrPos -= 1;
             sp1_MoveSprPix(spriceCurrDistr, &fullScreenDef, 0, 16, currsorSelPos[uiDistributorCurrPos]);
             z80_delay_ms(50);
        }
        else if (in_key_pressed(IN_KEY_SCANCODE_s)) {
            if (uiDistributorCurrPos < GOODS_QTY_DEF-1)
                 uiDistributorCurrPos += 1;
             sp1_MoveSprPix(spriceCurrDistr, &fullScreenDef, 0, 16, currsorSelPos[uiDistributorCurrPos]);
             z80_delay_ms(50);
        }
        else if (in_key_pressed(IN_KEY_SCANCODE_e)) {
            if (distributor.currentPrice[uiDistributorCurrPos]<9)
                distributor.currentPrice[uiDistributorCurrPos] += 1;
            displayCurrentWarehouse();
        }
        else if (in_key_pressed(IN_KEY_SCANCODE_d)) {
            if (distributor.currentPrice[uiDistributorCurrPos]>1)
                distributor.currentPrice[uiDistributorCurrPos] -= 1;
            displayCurrentWarehouse();
        }
        */


        /* grocery seller menu */
        if (in_key_pressed(IN_KEY_SCANCODE_o)) {
             if (uiGroceryCurrPos > 0)
                 uiGroceryCurrPos -= 1;
             sp1_MoveSprPix(spriceCurrGrocery, &fullScreenDef, 0, 230, currsorSelPos[uiGroceryCurrPos]);
             z80_delay_ms(50);
        }
        else if (in_key_pressed(IN_KEY_SCANCODE_l)) {
            if (uiGroceryCurrPos < GOODS_QTY_DEF-1)
                 uiGroceryCurrPos += 1;
             sp1_MoveSprPix(spriceCurrGrocery, &fullScreenDef, 0, 230, currsorSelPos[uiGroceryCurrPos]);
             z80_delay_ms(50);
        } else if (in_key_pressed(IN_KEY_SCANCODE_u)) {
            if (groceryShopPlayer.currentPrice[uiGroceryCurrPos]<9)
                groceryShopPlayer.currentPrice[uiGroceryCurrPos] += 1;
            displayCurrentWarehouse();
        }
        else if (in_key_pressed(IN_KEY_SCANCODE_j)) {
            if (groceryShopPlayer.currentPrice[uiGroceryCurrPos]>1)
                groceryShopPlayer.currentPrice[uiGroceryCurrPos] -= 1;
            displayCurrentWarehouse();
        }


        /* buying from distributor */
        if (in_key_pressed(IN_KEY_SCANCODE_m)) {
             buyFromDistributor(uiGroceryCurrPos);
        }

         /* exit */
        if (in_key_pressed(IN_KEY_SCANCODE_q)) {
            eExitState = GAME_OVER;
            break;
        }


        eExitState = newShoperStateMachine((uint8_t)uiCustomerGameTick&0xFF);
        z80_delay_ms(50);
        sp1_UpdateNow();    
    }
}




void displayTopScreen(void)
{
    displayScreen(fullScreenDef, zx_grocery_top_tiles, zx_grocery_top_ptiles, ZX_GROCERY_TOP_TILES_LEN, ZX_GROCERY_TOP_TILES_BASE, 0);
}

void displayBottomScreen(void)
{
    displayScreen(fullScreenDef, zx_grocery_bottom_tiles, zx_grocery_bottom_ptiles, ZX_GROCERY_BOTTOM_TILES_LEN, ZX_GROCERY_BOTTOM_TILES_BASE, 16);
    displayText(fullScreenDef, "\x14\x44 CKL 2022", 0, 23);
}


static enum GAME_EXIT_STATE newShoperStateMachine(uint8_t rndSeek)
{
    uint8_t i,k;
    uint8_t toPayGiven;
    enum GAME_EXIT_STATE eExitState = GAME_RUNNING;

    switch(eNewShoper){

        case NS_JUST_CAME_RAND:
            newShoperBill = 0;
            newShoperNoOfProducts = levelRndProdType[gameStatus.gameLevel];
            newShoperRandomProducts(rndSeek);            
            displayNewShoperRequest();
            eNewShoper = NS_WAITING_FOR_BILL;
            break;

        case NS_WAITING_FOR_BILL:
            toPayGiven = newShoperGiveBillNumber();

            if (toPayGiven == newShoperBill)
            {
                    displayText(fullScreenDef, "\x14\x44  Good! ", 24, 21);
                    sp1_UpdateNow();
                    playPositive();
                    eExitState = finalizeNewShoperRequest();
            }
            else
            {
                displayText(fullScreenDef, "\x14\xBAWrong -10$", 17, 21);
                sprintf(textBuffer, "\x14\xBACorr: %d$",newShoperBill);
                displayText(fullScreenDef, textBuffer, 17, 22);
                sp1_UpdateNow();
                playNegative();
                eExitState = applyGroceryPenalty(10);
                z80_delay_ms(2000);
                if (GAME_OVER == eExitState) break;
                displayCurrentWarehouse();
            }
            eNewShoper = NS_QUIT;
            break;


        case NS_QUIT:
            if (gameStatus.gameLevel<GAME_LEVELS-1)
            {
                if (gameStatus.customersNo>gameLevelCustomersNo[gameStatus.gameLevel]) gameStatus.gameLevel++;
            }

            gameStatus.customersNo++;
            if (1 == gameLevelDistributorPriceChange[gameStatus.gameLevel]) 
            {
                changeDistributorPrices();
                bit_beep(50, 1000);
                bit_beep(40,1200);
                bit_beep(30,1300);
            }
            displayBottomScreen();
            displayTopScreen();
            displayCurrentWarehouse();
            sp1_MoveSprPix(animatedSprite, &fullScreenDef, 0, 0, 190); // hide
            animation_x = 16;
            eNewShoper = NS_IDLE;
            break;

        default:
        break;
    }

    return eExitState;
}



static void newShoperRandomProducts(uint8_t rndSeek)
{
    uint8_t i, k;
    uint8_t rndNumber;
    bool diffProd;

    newShoperBill = 0;
    for (i=0; i<levelRndProdType[gameStatus.gameLevel];i++)
    {
        do 
        {
            diffProd = true;
            rndNumber = (rand()+rndSeek) % GOODS_QTY_DEF;
            newShoperProductsType[i] = rndNumber;
            if (i>0)
            {
                for(k=0; k<i; k++)
                {
                    if (newShoperProductsType[i] == newShoperProductsType[k]) 
                    {
                        diffProd = false;
                    } 
                }
            }
        } while (false == diffProd);


        rndNumber = ((rand()+rndSeek) % levelRndProdAmount[gameStatus.gameLevel])+1;
        newShoperProductsAmount[i] = rndNumber;

        if (groceryShopPlayer.currentInventory[newShoperProductsType[i]]>=newShoperProductsAmount[i])
        {
            newShoperBill += groceryShopPlayer.currentPrice[newShoperProductsType[i]] * newShoperProductsAmount[i];
        }
        else
        {
            newShoperBill += groceryShopPlayer.currentPrice[newShoperProductsType[i]] * groceryShopPlayer.currentInventory[newShoperProductsType[i]];

        }

        /* DEBUG */
        /*
        sprintf(textBuffer, "\x14\x78 %d, %d", newShoperProductsType[0], newShoperProductsAmount[0]);
        displayText(fullScreenDef, textBuffer, 1, 15);

        sprintf(textBuffer, "\x14\x78 %d, %d", newShoperProductsType[1], newShoperProductsAmount[1]);
        displayText(fullScreenDef, textBuffer, 1, 16);

        sprintf(textBuffer, "\x14\x78 %d", newShoperBill);
        displayText(fullScreenDef, textBuffer, 1, 17);
        */

    }
}


enum GAME_EXIT_STATE applyGroceryPenalty(uint8_t uiPenalty)
{
    if (gameStatus.currentFunds>=uiPenalty)
    {
        gameStatus.currentFunds -= 10;    
    }
    else
    {
        gameStatus.currentFunds = 0;
    }

    if (0 == gameStatus.currentFunds)
    {
        sp1_ClearRectInv(fullScreenDef,  BRIGHT | INK_BLACK | PAPER_BLACK, 32, SP1_RFLAG_TILE | SP1_RFLAG_COLOUR);
        displayText(fullScreenDef, "\x14\xCE GAME OVER !!!", 9, 11); 
        displayText(fullScreenDef, "\x14\xCE NO FUNDS ", 11, 12);   
        sp1_UpdateNow();
        bit_beep(500, 50);
        in_wait_key();
        return GAME_OVER;
    }
}
                

static void buyFromDistributor(uint8_t itemPos)
{
    if (distributor.currentPrice[itemPos]<=gameStatus.currentFunds) 
    {
        gameStatus.currentFunds-=distributor.currentPrice[itemPos];
        groceryShopPlayer.currentInventory[itemPos]+=1;

        displayCurrentWarehouse();
        bit_beep(40, 1000);
        bit_beep(25, 800);
        bit_beep(40, 1200);
    }
    else
    {
        /* not enough funds */
        displayText(fullScreenDef, "\x14\xCENot enough funds", 15, 21);
        sp1_UpdateNow();
        bit_beep(200, 50);
        z80_delay_ms(1500);
        displayText(fullScreenDef, "\x14\x38                ", 15, 21);
    }
}
             

static void displayNewShoperRequest(void)
{
    uint8_t productCnt;
    uint8_t productAmount;
    uint8_t productTypeIndex;

    displayText(fullScreenDef, "\x14\xCEShoper buying", 18, 16);
    bit_beep(10, 500);
    sp1_UpdateNow();
    z80_delay_ms(DELAY_LONG);
    for(productCnt=0; productCnt<newShoperNoOfProducts; productCnt++)
    {
        productTypeIndex = newShoperProductsType[productCnt];
        sprintf(textBuffer, "\x14\x38-%dkg/ %s", newShoperProductsAmount[productCnt], goodsNames[productTypeIndex]);
        displayText(fullScreenDef, textBuffer, 18, 17+productCnt);
        sp1_UpdateNow();
        bit_beep(10, 500);
        z80_delay_ms(DELAY_LONG);
    }
    bit_beep(10, 500);
    sp1_UpdateNow();
 
}



static enum GAME_EXIT_STATE  finalizeNewShoperRequest(void)
{
    uint8_t productCnt;

    for(productCnt=0; productCnt<newShoperNoOfProducts; productCnt++)
    {
        if (newShoperProductsAmount[productCnt] <= groceryShopPlayer.currentInventory[newShoperProductsType[productCnt]] )
        {
            groceryShopPlayer.currentInventory[newShoperProductsType[productCnt]] -= newShoperProductsAmount[productCnt];
            gameStatus.currentFunds += groceryShopPlayer.currentPrice[newShoperProductsType[productCnt]] * newShoperProductsAmount[productCnt];
        }
        else
        {
            groceryShopPlayer.currentInventory[newShoperProductsType[productCnt]] = 0;  
            gameStatus.currentFunds += groceryShopPlayer.currentPrice[newShoperProductsType[productCnt]] \
                                        * groceryShopPlayer.currentInventory[newShoperProductsType[productCnt]]; 
        } 
    }

    if (gameStatus.currentFunds>=WINNING_VALUE)
    {
        /* winning */
        sp1_ClearRectInv(fullScreenDef, BRIGHT | INK_BLACK | PAPER_BLACK, 32, SP1_RFLAG_TILE | SP1_RFLAG_COLOUR);
        displayText(fullScreenDef, "\x14\xCE YOU WIN!!!", 13, 12); 
        displayText(fullScreenDef, "\x14\xCE CONGRATS  ", 13, 13);   
        sp1_UpdateNow();
        bit_beep(50, 500);
        bit_beep(100, 700);
        in_wait_key();
        return GAME_OVER;
    }
    displayCurrentWarehouse();
    sp1_UpdateNow();

    return GAME_RUNNING;
}


static uint8_t newShoperGiveBillNumber(void)
{
    uint8_t escapeFlag = 0;
    uint8_t digitPos = 0;
    uint8_t enteredNumber = 0;

    while(1 != escapeFlag)
    {
       newShoperGameTick++;
        if (0 == newShoperGameTick % 40)
        {
            if (newShoperCountDown>0)
                newShoperCountDown--;

            if (0 == newShoperCountDown) return 255;
        }

        if (in_key_pressed(IN_KEY_SCANCODE_ENTER)) {
            escapeFlag = 1;
        }

        if (in_key_pressed(IN_KEY_SCANCODE_0 | 0x8000))
        {
            digitPos--;    
            if (0 == digitPos) {
                enteredNumber =0;
            }
            else
            {
                enteredNumber = (enteredNumber - enteredNumber % 10)/10;       
            }
            bit_beep(50, 100);
            z80_delay_ms(250);   
        }
        else if (digitPos<2)
        {
            if (in_key_pressed(IN_KEY_SCANCODE_0)) {
                enteredNumber = enteredNumber * digitPos * 10 + 0;
                digitPos++;
                bit_beep(50, 400);
                z80_delay_ms(250);
            } else if (in_key_pressed(IN_KEY_SCANCODE_1)) {
                enteredNumber = enteredNumber * digitPos * 10 + 1;
                digitPos++;
                bit_beep(50, 400);
                z80_delay_ms(250);
            } else if (in_key_pressed(IN_KEY_SCANCODE_2)) {
                enteredNumber = enteredNumber * digitPos * 10 + 2;
                digitPos++;
                bit_beep(50, 400);
                z80_delay_ms(250);
            } else if (in_key_pressed(IN_KEY_SCANCODE_3)) {
                enteredNumber = enteredNumber * digitPos * 10 + 3;
                digitPos++;
                bit_beep(50, 400);
                z80_delay_ms(250);
            } else if (in_key_pressed(IN_KEY_SCANCODE_4)) {
                enteredNumber = enteredNumber * digitPos * 10 + 4;
                digitPos++;
                bit_beep(50, 400);
                z80_delay_ms(250);
            } else if (in_key_pressed(IN_KEY_SCANCODE_5)) {
                enteredNumber = enteredNumber * digitPos * 10 + 5;
                digitPos++;
                bit_beep(50, 400);
                z80_delay_ms(250);
            } else if (in_key_pressed(IN_KEY_SCANCODE_6)) {
                enteredNumber = enteredNumber * digitPos * 10 + 6;
                digitPos++;
                bit_beep(50, 400);
                z80_delay_ms(250);
            } else if (in_key_pressed(IN_KEY_SCANCODE_7)) {
                enteredNumber = enteredNumber * digitPos * 10 + 7;
                digitPos++;
                bit_beep(50, 400);
                z80_delay_ms(250);
            } else if (in_key_pressed(IN_KEY_SCANCODE_8)) {
                enteredNumber = enteredNumber * digitPos * 10 + 8;
                digitPos++;
                bit_beep(50, 400);
                z80_delay_ms(250);
            } else if (in_key_pressed(IN_KEY_SCANCODE_9)) {
                enteredNumber = enteredNumber * digitPos * 10 + 9;
                digitPos++;
                bit_beep(50, 400);
                z80_delay_ms(250);
            } 
        }

        sprintf(textBuffer, "\x14\x3A %d ",newShoperCountDown);
        displayText(fullScreenDef, textBuffer, 27, 21);
        displayText(fullScreenDef, "\x14\x38To pay:", 17, 21);
        sprintf(textBuffer, "\x14\x38%d ",enteredNumber);
        displayText(fullScreenDef, textBuffer, 24, 21);

        sp1_UpdateNow();
        z80_delay_ms(25);
    }
    bit_beep(30, 50);
    z80_delay_ms(50);
    return enteredNumber;
}



static void displayCurrentWarehouse(void)
{
    uint8_t goodsCnt;

    for(goodsCnt=0; goodsCnt<GOODS_QTY_DEF; goodsCnt++)
    {
        displayText(fullScreenDef, "\x14\x0F Cash:", 16, 0);
        sprintf(textBuffer, "\x14\x0C%d$  ",gameStatus.currentFunds);
        displayText(fullScreenDef, textBuffer, 22, 0);

        sprintf(textBuffer, "\x14\x38%s",goodsNames[goodsCnt]);
        displayText(fullScreenDef, textBuffer, 3, goodsScreenPos[goodsCnt]);

        sprintf(textBuffer, "\x14\x78%d$ ", distributor.currentPrice[goodsCnt]);
        displayText(fullScreenDef, textBuffer, 12, goodsScreenPos[goodsCnt]);

        sprintf(textBuffer, "\x14\x78%d$ ", groceryShopPlayer.currentPrice[goodsCnt]);
        displayText(fullScreenDef, textBuffer, 20, goodsScreenPos[goodsCnt]);

        sprintf(textBuffer, "\x14\x78%dkg ", groceryShopPlayer.currentInventory[goodsCnt]);
        displayText(fullScreenDef, textBuffer, 24, goodsScreenPos[goodsCnt]);

        sprintf(textBuffer, "\x14\x78 Cust: %d  Lvl: %d", gameStatus.customersNo, gameStatus.gameLevel);
        displayText(fullScreenDef, textBuffer, 14, 23);
    }
    sp1_UpdateNow();
}



static void changeDistributorPrices(void)
{
    uint8_t goodsCnt;
    uint8_t delta;

    for(goodsCnt=0; goodsCnt<GOODS_QTY_DEF; goodsCnt++)
    {
        delta = rand() % 5 + 1; /* change to DEFINE needed */

        if (delta<2)
        {
            if (distributor.currentPrice[goodsCnt] - delta >0)
                distributor.currentPrice[goodsCnt] -= delta;
        }
        else if (delta >2)
        {
             if (distributor.currentPrice[goodsCnt] + delta <=5)
                distributor.currentPrice[goodsCnt] += delta;
        }
    }
}

void initGameVariables(uint16_t rndSeed)
{
    uint8_t goodsCnt;
    uint8_t price;

    gameStatus.currentFunds = 30;
    gameStatus.gameLevel = 0;
    gameStatus.customersNo = 0;
    newShoperGameTick = 0;

    for(goodsCnt=0; goodsCnt<GOODS_QTY_DEF; goodsCnt++)
    {
        price = (rand()+rndSeed) % 5 + 1; /* change to DEFINE needed */
        distributor.currentPrice[goodsCnt] = price;
        groceryShopPlayer.currentPrice[goodsCnt] = price + 1;
        groceryShopPlayer.currentInventory[goodsCnt] = 0;
    }

    eNewShoper = NS_IDLE;
}




void playIntro(void)
{
    uint8_t i,k;

    for(k=0; k<6; k++)
        for (i=0; i<3; i++)
        {
            bit_beep(50, i*100+k*100);
            bit_beep(10, 3200-i*100);
            bit_beep(50, 1600-k*300);
        }
}


void playPositive(void)
{
    uint8_t i;

    for (i=0; i<4; i++)
    {
        bit_beep(50, i*100);
        bit_beep(10, 3200-i*100);
        bit_beep(50, 1600);
    }
    bit_beep(100, 1200);
}


void playNegative(void)
{
    uint8_t i;

    for (i=0; i<3; i++)
    {
        bit_beep(80, 400 - i*50);
        bit_beep(150, 100);
    }
    bit_beep(100, 50);
}
