//
//  view.h
//  Raging Clue
//
//  Created by Sean Corkery on 3/3/15.
//  Copyright (c) 2015 MUFFIN. All rights reserved.
//

#ifndef __Raging_Clue__view__
#define __Raging_Clue__view__

#include <stdio.h>

#endif /* defined(__Raging_Clue__view__) */


typedef enum {SCARLET=1, MOUTON=2, PLEB=3, WHITNEY=4} character;
typedef enum {HAMMER=4, STICK=5, PIE=6, PANTS=7, CHAINSAW=8, SHOTGUN=9, WEASEL=10} weapon;
typedef enum {KITCHEN=0, LIBRARY=1, STUDY=2, OSTRICH_ROOM=3} room;

void computer_player_turn(int which);
int send_player_data(void);
void game_start(void);
void introduction(void);