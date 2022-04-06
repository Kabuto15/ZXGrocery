#ifndef MAIN__H
#define MAIN__H

#include <z80.h>
#include <arch/zx.h>
#include <arch/zx/sp1.h>
#include <stdio.h>
#include <stdlib.h>
#include <input.h>
#include "game.h"
#include "gui_funcs.h"


#define uint8_t unsigned char
#define uint16_t unsigned int
typedef enum { false, true } bool;

extern struct sp1_Rect fullScreenDef;


#endif