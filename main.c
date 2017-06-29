//
//  main.c
//  Raging Clue
//
//  Created by Sean Corkery on 3/3/15.
//  Copyright (c) 2015 MUFFIN. All rights reserved.
//

#include <stdio.h>
#include "view.h"
#include "controller.h"

int main(int argc, const char * argv[]) {
    new_game();
    game_start();
    return 0;
}
