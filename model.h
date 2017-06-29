//
//  model.h
//  Raging Clue
//
//  Created by Sean Corkery on 3/3/15.
//  Copyright (c) 2015 MUFFIN. All rights reserved.
//

#include <stdio.h>
#define OUTPUT_FILE "tempdata"

void setup_game(void);
int next_turn(void);
int report_position(int which);
void send_eliminated(int type);
void computer_take_turn(int which);
int verify_suggestion(int character, int weapon, int room);
int verify_accusation(int character, int weapon, int room);
