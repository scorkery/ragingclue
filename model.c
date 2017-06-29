//
//  model.c
//  Raging Clue
//
//  Created by Sean Corkery on 3/3/15.
//  Copyright (c) 2015 MUFFIN. All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "model.h"
#include "controller.h"

/*
 --------------------------------------------------------------------------
 DEFINITIONS
 --------------------------------------------------------------------------
 */

// room types

#define KITCHEN 0
#define LIBRARY 1
#define STUDY 2
#define OSTRICH_ROOM 3

// weapon types

#define HAMMER 4
#define STICK 5
#define PIE 6
#define PANTS 7
#define CHAINSAW 8
#define SHOTGUN 9
#define WEASEL 10

// character types

#define SCARLET 11
#define MOUTON 12
#define PLEB 13
#define WHITNEY 14

// define misc values

#define ELIMINATED -1
#define MAX_SQUARES 24

typedef enum {LOST=0, WON=1} wonlost;

//#define TRUE 1
//#define FALSE 0

typedef enum {FALSE, TRUE} boolean;

// action types

typedef enum {MOVE_ONLY, AT_DOOR, MAKES_SUGGESTION, MAKES_ACCUSATION, DOES_NOTHING} action;

// cards, players, & the murderer

#define MAX_CARDS 15
#define MAX_PLAYERS 4
#define MAX_PLAYER_CARDS 3

typedef struct {
    int character;
    int postion;
    int room;
    int *playerCard[MAX_PLAYER_CARDS];
} player;

typedef struct {
    int *character;
    int *room;
    int *weapon;
} murderer;

// movement

typedef struct {
    int spaceNumber;
    boolean hasDoor;
    int whichDoor;
    char text[10];
} movement_square;

movement_square square[MAX_SQUARES];
movement_square *playerSquare;

// cards & global pointers to cards

int card[MAX_CARDS];
int *current_card;
int *bottom_card;
int *top_card;

player players[MAX_PLAYERS];
murderer the_murderer;

player *current_player; // general use pointer for cycling through loops and such
player *human_player; // the human player
player *active_player; // the player currently taking their turn

int *active_player_room; // room the player is currently in, references a room card in the card array
int *current_playerCard; // current card in the player's hand being referenced

// global variables

int suggest_character;
int suggest_weapon;
int suggest_room;

int turn = 0;

FILE *output_file;
FILE *input_file;



/*
 --------------------------------------------------------------------------
 FUNCTIONS
 --------------------------------------------------------------------------
 */

// prototypes

// setup
void init(void);
void assign_doors(void);
void assign_start_places(void);
void card_assignment(void);
void choose_murderer(void);
int *scan_cards(int *theCard);
void shuffle(void);
void deal_cards(void);

// mechanics
char *format_names(int which, char *dest);
void log_output(char *input);

// gameplay
void game_over(player *thePlayer, int win);
int dice_roll(int sides);
void move_player(player *thePlayer, int spaces);
int check_door(player *thePlayer);
int check_room_elim(int roomNum);
action take_turn(player *thePlayer);
void eliminate_card(int theCard);
void check_suggestion(int character, int weapon, int room, int type);
void make_suggestion(void);


// ======================= SETUP FUNCTIONS =============================


void init(void) // initializes stuff
{
    output_file = fopen(OUTPUT_FILE, "w");
    fclose(output_file);
    current_card = card;
    bottom_card = card;
    top_card = &card[MAX_CARDS];
    
    current_player = players;
    human_player = &players[0];
    active_player = human_player;
    current_playerCard = players[0].playerCard[0];
    
    active_player_room = &card[0]; // might need to change this later
    
    assign_doors();
    
    time_t seconds;
    time(&seconds);
    srandom((unsigned int) seconds);
}

void assign_doors(void) // designates which movement squares lead to doors
{
    // first, initialize hasDoor of all squares to false
    
    for (int i=0; i<MAX_SQUARES; i++) {
        square[i].hasDoor = FALSE;
    }
    
    // now set squares with doors to true and designate where doors lead to
    
    square[1].hasDoor = TRUE;
    square[1].whichDoor = STUDY;
    
    square[5].hasDoor = TRUE;
    square[5].whichDoor = STUDY;
    
    square[7].hasDoor = TRUE;
    square[7].whichDoor = KITCHEN;
    
    square[11].hasDoor = TRUE;
    square[11].whichDoor = KITCHEN;
    
    square[13].hasDoor = TRUE;
    square[13].whichDoor = OSTRICH_ROOM;
    
    square[17].hasDoor = TRUE;
    square[17].whichDoor = OSTRICH_ROOM;
    
    square[19].hasDoor = TRUE;
    square[19].whichDoor = LIBRARY;
    
    square[23].hasDoor = TRUE;
    square[23].whichDoor = LIBRARY;
}

void assign_start_places(void) // randomly determines where all players will start on the board
{
    for (int i=0; i<MAX_PLAYERS; i++) {
        
        // assign all players a starting position based on a random number between 0 and the total number of squares
        
        players[i].postion = random() % MAX_SQUARES - 1;
        
        // check to make sure no two players start on the same square
        
        for (int j=0; j<MAX_PLAYERS; j++) {
            if (players[i].postion == players[j].postion) {
                players[i].postion ++;
            }
        } // end loop
    }// end loop
}

void card_assignment(void) // assigns values to each card corresponding to rooms, characters and weapons
{
    int theCard = 0;
    
    for (current_card = card; current_card < top_card; current_card ++) {
        *current_card = theCard;
        theCard ++;
    }
}

void assign_players(void) // assigns characters to players
{
    int playerCharacter = get_player_data() + 10; // add ten to value because player cards start at 11
    players[0].character = playerCharacter; // assign the player's chosen character to the character slot
    
    for (int i=1; i<MAX_PLAYERS; i++) {
        players[i].character = i + SCARLET;
    }
    
    
    for (int i=1; i<MAX_PLAYERS; i++) {
        if (players[i].character == players[0].character) players[i].character = SCARLET;
    }
}

void choose_murderer(void) // pick out a character, room and weapon for the solution
{
    
    // pick random card numbers for the character, weapon and room respectively
    // must choose number from within ranges appropriate for card types, see constant definitions
    
    int character = (random() % 3) + 11; // characters start at 11, so add 11
    int weapon = (random() % 6) + 4; // weapons start at 4, so add 4
    int room = (random() % 3); // rooms start at 0, so no need to add a value
    
    // assign chosen cards to the murderer
    
    the_murderer.character = &card[character];
    the_murderer.weapon = &card[weapon];
    the_murderer.room = &card[room];
}

int *scan_cards(int *theCard) // checks to see if a card is part of the solution or eliminated
{
    if (theCard == the_murderer.character) return 0;
    else if (theCard == the_murderer.weapon) return 0;
    else if (theCard == the_murderer.room) return 0;
    else if (*theCard == ELIMINATED) return 0;
    
    else return theCard;
}

void shuffle(void)
{
    
}

void deal_cards(void) // deal cards out to player, after the murderer cards have been set
{
    int playerNumber = 0;
    int cardNumber = 0;
    
    for (current_card = card; current_card < top_card; current_card ++) {
        
        // if current card is part of the solution, skip to the next card
        
        if (scan_cards(current_card) != current_card) {
            continue;
        }
        
        // otherwise, add the card to the player's hand
        
        else {
            players[playerNumber].playerCard[cardNumber] = scan_cards(current_card);
        }
        
        // move on to the next player
        
        playerNumber ++;
        
        // if all players have been cycled through, reset playerNumber to 0
        
        if (playerNumber == 4) {
            cardNumber ++;
            playerNumber = 0;
        }
    } // end loop
}


// ======================= MECHANIC FUNCTIONS =============================


char *format_names(int which, char *dest)
{
    if (which == KITCHEN) strcpy(dest, "kitchen");
    else if (which == LIBRARY) strcpy(dest, "library");
    else if (which == STUDY) strcpy(dest, "study");
    else if (which == OSTRICH_ROOM) strcpy(dest, "ostrich room");
    
    else if (which == HAMMER) strcpy(dest, "hammer");
    else if (which == STICK) strcpy(dest, "pointy stick");
    else if (which == PIE) strcpy(dest, "banana cream pie laced with cyanide");
    else if (which == PANTS) strcpy(dest, "pair of pants");
    else if (which == CHAINSAW) strcpy(dest, "chainsaw");
    else if (which == SHOTGUN) strcpy(dest, "shotgun");
    else if (which == WEASEL) strcpy(dest, "vicious attack weasel");
    
    else if (which == SCARLET) strcpy(dest, "Scarlet Pumpernickel");
    else if (which == MOUTON) strcpy(dest, "Colonel Mouton");
    else if (which == PLEB) strcpy(dest, "Professor Pleb");
    else if (which == WHITNEY) strcpy(dest, "Whitney Houston");
    
    return dest;
}

void log_output(char *input)
{
    char output[500];
    strcpy(output, input);
    output_file = fopen(OUTPUT_FILE, "a");
    fprintf(output_file, output);
    fclose(output_file);
}

// ======================= GAMEPLAY FUNCTIONS =============================


void game_over(player *thePlayer, int win) // ends the game when someone guesses correctly
{
    if (thePlayer - 1 == human_player) thePlayer --;
    int *winner = &thePlayer->character;
    char winnerName[50] = "";
    format_names(*winner, winnerName);
    
    if (win == WON) {
        printf("\n*** GAME OVER ***\n\n%s has guessed correctly and won the game.\n'", winnerName);
    }
    else {
        printf("\n*** GAME OVER ***\n\nYour accusation was incorrect, and the remaining guests snicker at your hilarious faux pas.  'Actually', one of the others states haughtily, '");
    }
    
    char murd[50] = "";
    char weap[50] = "";
    char room[50] = "";
    
    format_names(*the_murderer.character, murd);
    format_names(*the_murderer.weapon, weap);
    format_names(*the_murderer.room, room);
    
    printf("The murder was committed by %s in the %s using the %s.", murd, room, weap);
    if (win == LOST) printf("  Even a child could see this.'\n\nThe other guests nod in agreement, as you hang your head in shame.  'Perhaps you should leave the detective work to the real detectives, Columbo!' someone else chimes in, and everyone laughs.\n\nPerhaps indeed.\n\n");
    else if (win == WON && thePlayer != human_player) printf("' proclaims %s, and everyone, yourself included, stands with mouth agape at this person's almost supernatural deductive abilities.\n\n'Man', you think to yourself on the long boat ride home, 'I sure wish I'd thought of that!'\n\n", winnerName);
    else printf("' you proclaim, and everyone applauds your superior deductive powers.\n\nBravo!\n\n");
    
    exit(0);
}

int dice_roll(int sides) // roles a dice of a number of sides specified by user
{
    // dice roll can never be zero, so account for this
    
    int roll = random() % (sides - 1); // subtract 1 from sides, so a 6 sided dice is now random of 0-5
    roll ++; // increment roll, so roll is now a number 1-6 in a 6 sided dice roll
    
    // record the output
    
    char logstring[500];
    sprintf(logstring, "\nRolling a %d sided die.  Result is %d.\n", sides, roll);

    log_output(logstring);
    
    return roll;
}

void move_player(player *thePlayer, int spaces) // moves the player a given number of spaces
{
    int playerPos = thePlayer->postion; // square the player begins at
    int moveTo = playerPos + spaces; // an integer to hold how many spaces total player will be moved
    int lastSquare = MAX_SQUARES - 1;
    int movesToLast = (lastSquare + 1) - playerPos; // how many moves player will make to get to last square
    
    int totalSpaces = spaces; // just used for logging
    
    // if current move will move player past the maximum number of squares, ie MAX_SQUARES=24, PLAYER at 23, move 6
    
    if (moveTo > lastSquare) {
        spaces = spaces - movesToLast; // how many spaces past the max will the movement take
        thePlayer->postion = 0; // move player to square zero
    }
    thePlayer->postion += spaces; // advance player position to int spaces
    
    // format and log the output
    
    char name[50];
    int *playerName;
    playerName = &thePlayer->character;
    format_names(*playerName, name);
    
    char logstring[500];
    sprintf(logstring, "\n%s has moved %d spaces; current position is %d.\n", name, totalSpaces, thePlayer->postion);
    log_output(logstring);
}

int check_door(player *thePlayer) // checks to see if player is standing beside a door
{
    // create a pointer to the player's current square to make things easier
    
    int squareNumber = thePlayer->postion; // what square is the player on
    movement_square *theSquare; // pointer for said square
    theSquare = &square[squareNumber]; // assign the pointer to the current player square
    
    return theSquare->hasDoor;
}

int check_room_elim(int roomNum) // checks to see if a room card has been eliminated already
{
    if (card[roomNum] == ELIMINATED) return TRUE; // card has been eliminated, return true
    else return FALSE; // otherwise, return false
}

action take_turn(player *thePlayer) // everything that happens during a player's turn
{
    action playerAction = DOES_NOTHING; // the action player will be taking this turn, initialize to nothing as default
    
    if (turn == 1) assign_players(); // on the first turn, assign human player to chosen character and the rest to computer
    
    // roll the dice and move the player
    
    int moveSpaces = dice_roll(6);
    move_player(thePlayer, moveSpaces);
    
    // check to see if player is standing in front of a door
    
    if (check_door(thePlayer)) {
        char door[50] = "";
        char name[50] = "";
        char logstring[100] = "";
        
        format_names(thePlayer->character, name);
        format_names(square[thePlayer->postion].whichDoor, door);
        
        sprintf(logstring, "\n%s arrives at the door to the %s.\n", name, door);
        log_output(logstring);
        
        if (thePlayer == human_player) {
            
            // if the player is human, action is at a door
            
            playerAction = AT_DOOR;
        }
        else {
            
            // otherwise, it's a computer player.
            // player will have a 60% chance of choosing to enter the room and make a suggestion
            
            int rand = random() % 100;
            if (rand <= 60) {
                // player enters the room, so log output informing the human player of this
                
                sprintf(logstring, "\n%s enters the %s.\n", name, door);
                log_output(logstring);
                
                // player is in a room ,so make a suggestion
                
                make_suggestion();
                playerAction = MAKES_SUGGESTION;
            }
            else {
                // player decides not to enter the room, so let the user know
                
                sprintf(logstring, "\n%s decides not to enter the %s.\n", name, door);
                log_output(logstring);
            }
        } // end if player is computer
        
    } // end check door
    
    else {
        playerAction = MOVE_ONLY;
    }
    
    return playerAction;
}

void eliminate_card(int theCard) // eliminates a card from suspicion
{
    card[theCard] = ELIMINATED; // sets the card value to -1
    
    char cardName[50];
    format_names(theCard, cardName);
    
    char logstring[500];
    sprintf(logstring, "\n%s has been eliminated from suspicion.\n", cardName);
    log_output(logstring);
}

void check_suggestion(int character, int weapon, int room, int type) // checks the suggestion
{
    player *last_player;
    last_player = &players[MAX_PLAYERS - 1];
    
    int *theCharacter;
    int *theWeapon;
    int *theRoom;
    
    theCharacter = &card[character];
    theWeapon = &card[weapon];
    theRoom = &card[room];
    
    // first, check to see if the suggestion is the correct solution
    
    if (*theCharacter == *the_murderer.character && *theWeapon == *the_murderer.weapon && *theRoom == *the_murderer.room) {
        game_over(active_player, WON);
        return;
    }
    else {
        // player has made an accusation and guessed incorrectly
        if (type == 1) game_over(active_player, LOST);
    }
    
    // log the player's suggestion for display purposes
    
    char player_name[50] = "";
    char sug_char[50] = "";
    char sug_weap[50] = "";
    char sug_room[50] = "";
    char logstring[200] = "";
    player *tempPlayer;
    tempPlayer = active_player;
    
    format_names(tempPlayer->character, player_name);
    format_names(*theCharacter, sug_char);
    format_names(*theWeapon, sug_weap);
    format_names(*theRoom, sug_room);
    
    sprintf(logstring, "\n%s suggests that it was %s in the %s with the %s.\n", player_name, sug_char, sug_room, sug_weap);
    
    log_output(logstring);
    
    // if suggestion != solution, go through all player cards until one of the suggested cards is found, then eliminate the card
    
    for (current_player = &players[0]; current_player <= last_player; current_player ++) {
        for (int i=0; i<MAX_PLAYER_CARDS; i++) {
            if (*current_player->playerCard[i] == character) {
                eliminate_card(character);
                return;
            }
            else if (*current_player->playerCard[i] == weapon) {
                eliminate_card(weapon);
                return;
            }
            else if (*current_player->playerCard[i] == room) {
                eliminate_card(room);
                return;
            }
        } // end for
        
    } // end for
}

void make_suggestion(void) // computer players will use this to make suggestions
{
    // pick out random cards to suggest
    
    int character = (random() % 3) + 11; // characters start at 11, so add 11
    int weapon = (random() % 6) + 4; // weapons start at 4, so add 4
    int room = (random() % 3); // rooms start at 0, so no need to add a value
    
    // check to make sure each card picked hasn't already been eliminated
    
    while (card[character] == ELIMINATED) {
        character = (random() % 3) + 11;
    }
    while (card[weapon] == ELIMINATED) {
        weapon = (random() % 6) + 4;
    }
    while (card[room] == ELIMINATED) {
        room = (random() % 3);
    }
    
    // assign suggestion to temporary variables for use outside function
    
    suggest_character = character;
    suggest_room = room;
    suggest_weapon = weapon;
    
    // check the suggestion
    
    check_suggestion(character, weapon, room, 0);
}


// ======================= TESTING FUNCTIONS =============================



void print_all_cards(void) // prints all cards in their current order
{
    current_card = card;
    for (current_card = card; current_card < top_card; current_card ++) {
        printf("%d\n", *current_card);
    }
}

void print_solution(void) // prints which cards are the murderer cards, use for testing purposes only
{
    for (current_card = card; current_card < top_card; current_card ++) {
        if (current_card == the_murderer.character) printf("\nMurderer: %d", *current_card);
        if (current_card == the_murderer.weapon) printf("\nWeapon: %d", *current_card);
        if (current_card == the_murderer.room) printf("\nRoom: %d\n\n", *current_card);
        printf("\n\n");
    }
}

void print_hands(void) // prints out hands for all players, also for testing purposes
{
    printf("\nPlayer 0: %d, %d, %d", *players[0].playerCard[0], *players[0].playerCard[1], *players[0].playerCard[2]);
    printf("\nPlayer 1: %d, %d, %d", *players[1].playerCard[0], *players[1].playerCard[1], *players[1].playerCard[2]);
    printf("\nPlayer 2: %d, %d, %d", *players[2].playerCard[0], *players[2].playerCard[1], *players[2].playerCard[2]);
    printf("\nPlayer 3: %d, %d, %d", *players[3].playerCard[0], *players[3].playerCard[1], *players[3].playerCard[2]);
}



// ======================= EXPORT FUNCTIONS =============================

// these functions get sent to controller.c

void setup_game(void) // sets up everything necessary for the game to begin
{
    init();
    card_assignment();
    assign_start_places();
    choose_murderer();
    shuffle();
    deal_cards();
}

void send_eliminated(int type) // logs which cards have been eliminated based on screen in view. for use w/suggestions, accusations
{
    // if player is on character screen
    
    if (type == 6 || type == 15) {
        for (int i=SCARLET; i<=WHITNEY; i++) {
            if (card[i] == ELIMINATED) {
                char name[50] = "";
                char logstring[50] = "";
                
                format_names(i, name);
                sprintf(logstring, "\n%s", name);
                log_output(logstring);
            }
        }
    }
    
    // if player is on weapon screen
    
    else if (type == 7 || type == 16) {
        for (int i=HAMMER; i<=WEASEL; i++) {
            if (card[i] == ELIMINATED) {
                char name[50] = "";
                char logstring[50] = "";
                
                format_names(i, name);
                sprintf(logstring, "\n%s", name);
                log_output(logstring);
            }
        }
    }
    
    // if player is on room screen
    
    else if (type == 8 || type == 17) {
        for (int i=KITCHEN; i<=OSTRICH_ROOM; i++) {
            if (card[i] == ELIMINATED) {
                char name[50] = "";
                char logstring[50] = "";
                
                format_names(i, name);
                sprintf(logstring, "\n%s", name);
                log_output(logstring);
            }
        }
    }
}

int verify_suggestion(int character, int weapon, int room)
{
    active_player --; // decrement back to the human player
    check_suggestion(character, weapon, room, 0);
    active_player ++; // put player back the way it was
    return 0;
}

int verify_accusation(int character, int weapon, int room)
{
    check_suggestion(character, weapon, room, 1);
    return 0;
}

int report_position(int which) // reports a given player's position on the board
{
    int position = players[which].postion;
    return position;
}

int next_turn(void) // takes the next turn with the current active player
{
    turn ++;
    
    if (active_player == human_player) printf("\nCurrent player is human.\n");
    else printf("\nCurrent player is a computer.");
    
    action playerAction = take_turn(active_player);
    
    if (playerAction == AT_DOOR) {
        
    }
    else if (playerAction == MOVE_ONLY) {
        if (active_player->character == SCARLET) log_output("\nScarlet Pumpernickel plods down the hallway.");
        else if (active_player->character == MOUTON) log_output("\nColonel Mouton trots down the hallway.");
        else if (active_player->character == PLEB) log_output("\nProfessor Pleb shuffles down the hallway.");
        else if (active_player->character == WHITNEY) log_output("\nWhitney Houston stumbles down the hallway.");
    }
    
    // advance pointer to the next player and end turn
    
    if (active_player < &players[MAX_PLAYERS - 1]) active_player ++;
    else active_player = &players[0];
    
    return playerAction;
}

void computer_take_turn(int which)
{
    char logstring[100];
    sprintf(logstring, "\n\nTurn:  Computer Player %d\n", which +1);
    log_output(logstring);
    next_turn();
}



