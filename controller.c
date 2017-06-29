//
//  controller.c
//  Raging Clue
//
//  Created by Sean Corkery on 3/3/15.
//  Copyright (c) 2015 MUFFIN. All rights reserved.
//

#include "controller.h"
#include "model.h"
#include "view.h"
#include <stdio.h>
#include <string.h>

FILE *output_file;
FILE *input_file;

void new_game(void);
int get_player_data();
void send_suggestion(int character, int weapon, int room);
void send_accusation(int character, int weapon, int room);
void print_output(void);
int get_player_position(int which);
int take_next_turn(void);
char *get_player_input(void);


void new_game(void) // starts a new game
{
    setup_game();
}

int get_player_data() // retrieves the player number from view
{
    int data = send_player_data();
    return data;
}

void get_eliminated(int type)
{
    send_eliminated(type);
    print_output();
}

void send_comp_turn(int which)
{
    computer_take_turn(which);
    print_output();
}

void send_suggestion(int character, int weapon, int room)
{
    character += 10; // add 10 because character numbers start at 11 in model
    verify_suggestion(character, weapon, room);
    print_output();
}

void send_accusation(int character, int weapon, int room)
{
    character += 10; // add 10 because character numbers start at 11 in model
    verify_accusation(character, weapon, room);
    print_output();
}

void print_output(void) // prints the content of the output file to the screen
{
    char newstring[5000] = "";
    output_file = fopen(OUTPUT_FILE, "r");
    fseek(output_file, 0, SEEK_SET);
    fread(newstring, 5000, 1, output_file);
    fclose(output_file);
    printf("\n%s\n" ,newstring);
    output_file = fopen(OUTPUT_FILE, "w");
    fclose(output_file);
}

int get_player_position(int which) // gets the player's physical position on the board
{
    int position = report_position(which);
    return position;
}

int take_next_turn(void) // takes the next turn and then prints all output from the turn
{
    int turnAction = next_turn(); // calls next turn from model
    print_output();
    return turnAction;
}

char *get_player_input(void) // I don't even know if this is used anywhere.  see if this is used anywhere, if not delete it
{
    char newstring[500];
    input_file = fopen("userin", "r");
    fseek(input_file, 0, SEEK_SET);
    fread(newstring, 500, 1, input_file);
    
    char *returnString[500];
    strcpy(*returnString, newstring);
    
    return *returnString;
}