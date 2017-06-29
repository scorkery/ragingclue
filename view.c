//
//  view.c
//  Raging Clue
//
//  Created by Sean Corkery on 3/3/15.
//  Copyright (c) 2015 MUFFIN. All rights reserved.
//

#include "view.h"
#include "controller.h"
#include <string.h>
#include <stdlib.h>

#define BOARD_WIDTH 30
#define BOARD_HEIGHT 23


/*
 --------------------------------------------------------------------------
 DEFINITIONS
 --------------------------------------------------------------------------
 */


typedef enum {BLACK=30, RED=31, GREEN=32, YELLOW=33, BLUE=34, MAGENTA=35, CYAN=36, WHITE=37} foreground_t;
typedef enum {BLACK_B=40, RED_B=41, GREEN_B=42, YELLOW_B=43, BLUE_B=44, MAGENTA_B=45, CYAN_B=46, WHITE_B=47} background_t;
typedef enum {OFF=0, BOLD=1, UNDERSCORE=4, BLINK=5, REVERSE_VID=7, CONCEALED=8} attribute_t;

typedef struct {
    char content;
} pixel_t;

typedef struct {
    int number;
    int width;
    int height;
    int x;
    int y;
} square_t;

pixel_t view[BOARD_WIDTH][BOARD_HEIGHT]; // x, y

character player;

pixel_t *current_view; // pointer to be used when determing player locations

int screen = 0; // keeps track of what screen the user is currently on
int option_select = 0; // global int to keep track of option numbers when they need to be passed between functions
int prev_screen = 0; // keeps track of the player's previous screen when necessary
int turn_action = 0; // keeps track of the action performed on the active player's last turn

int player_position = 0; // keeps track of where the player is physically on the board

// variables for the computer players

int player_1_position = 0;
int player_2_position = 0;
int player_3_position = 0;
int computer_turn = 0; // keeps track of which computer player is currently taking their turn

// variables to be used in making suggestions / accusations

int sug_character;
int sug_weapon;
int sug_room;

/*
 --------------------------------------------------------------------------
 FUNCTIONS
 --------------------------------------------------------------------------
 */

// prototypes

void draw_board(void);
void draw_screen(int whichScreen);

void clear_screen(void);
int send_player_data();
void help(int whichScreen);
void quit(void);
void reset_game(void);
void introduction(void);
void print_backstory(character theCharacter);
void print_player_name(void);
void print_room_info(void);
void computer_player_turn(int which);
void show_eliminated (int type);
void chk_suggestion(int character, int weapon, int room);
void chk_accusation(int character, int weapon, int room);
void begin_player_turn(void);
void options_list(void);
int read_response(char response[100]);
int user_prompt(void);
void game_start();



// ======================= GRAPHICS FUNCTIONS =============================

void init_view(void)
{
    for (int x=1, y=0; y<23; x++) {
        if (x == 29) {
            view[x][y].content = ' ';
            y++;
            x=0;
        }
        else view[x][y].content = ' ';
    }
}

void get_positions(void)
{
    player_position = get_player_position(0);
    player_1_position = get_player_position(1);
    player_2_position = get_player_position(2);
    player_3_position = get_player_position(3);
}

void update_positions(int thePostion, char firstInitial, char secondInitial)
{
    // set content of views at center of board square thePostion to the characters initials
    // again, a little complicated, but trust me that this works and makes sense
    
    switch (thePostion) {
        case 0:
            view[15][2].content = firstInitial;
            view[16][2].content = secondInitial;
            break;
            
        case 1:
            view[19][2].content = firstInitial;
            view[20][2].content = secondInitial;
            break;
            
        case 2:
            view[23][2].content = firstInitial;
            view[24][2].content = secondInitial;
            break;
            
        case 3:
            view[27][2].content = firstInitial;
            view[28][2].content = secondInitial;
            break;
            
        case 4:
            view[27][5].content = firstInitial;
            view[28][5].content = secondInitial;
            break;
        
        case 5:
            view[27][8].content = firstInitial;
            view[28][8].content = secondInitial;
            break;
        
        case 6:
            view[27][11].content = firstInitial;
            view[28][11].content = secondInitial;
            break;
        
        case 7:
            view[27][14].content = firstInitial;
            view[28][14].content = secondInitial;
            break;
        
        case 8:
            view[27][17].content = firstInitial;
            view[28][17].content = secondInitial;
            break;
        
        case 9:
            view[27][20].content = firstInitial;
            view[28][20].content = secondInitial;
            break;
        
        case 10:
            view[23][20].content = firstInitial;
            view[24][20].content = secondInitial;
            break;
        
        case 11:
            view[19][20].content = firstInitial;
            view[20][20].content = secondInitial;
            break;
        
        case 12:
            view[15][20].content = firstInitial;
            view[16][20].content = secondInitial;
            break;
        
        case 13:
            view[11][20].content = firstInitial;
            view[12][20].content = secondInitial;
            break;
        
        case 14:
            view[7][20].content = firstInitial;
            view[8][20].content = secondInitial;
            break;
        
        case 15:
            view[3][20].content = firstInitial;
            view[4][20].content = secondInitial;
            break;
        
        case 16:
            view[3][17].content = firstInitial;
            view[4][17].content = secondInitial;
            break;
            
        case 17:
            view[3][14].content = firstInitial;
            view[4][14].content = secondInitial;
            break;
            
        case 18:
            view[3][11].content = firstInitial;
            view[4][11].content = secondInitial;
            break;
            
        case 19:
            view[3][8].content = firstInitial;
            view[4][8].content = secondInitial;
            break;
            
        case 20:
            view[3][5].content = firstInitial;
            view[4][5].content = secondInitial;
            break;
            
        case 21:
            view[3][2].content = firstInitial;
            view[4][2].content = secondInitial;
            break;
            
        case 22:
            view[7][2].content = firstInitial;
            view[8][2].content = secondInitial;
            break;
            
        case 23:
            view[11][2].content = firstInitial;
            view[12][2].content = secondInitial;
            break;
            
            
        default:
            break;
    }
}

void update_view(void)
{
    // set all view contents to blank
    
    for (int x=1, y=0; y<23; x++) {
        if (x == 29) {
            view[x][y].content = ' ';
            y++;
            x=0;
        }
        else view[x][y].content = ' ';
    }
    
    // print the names of the rooms
    
    // library
    view[7][5].content = 'L';
    view[8][5].content = 'I';
    view[9][5].content = 'B';
    view[10][5].content = 'R';
    view[11][5].content = 'A';
    view[12][5].content = 'R';
    view[13][5].content = 'Y';
    
    // study
    view[17][5].content = 'S';
    view[18][5].content = 'T';
    view[19][5].content = 'U';
    view[20][5].content = 'D';
    view[21][5].content = 'Y';
    
    // ostrich room
    view[7][12].content = 'O';
    view[8][12].content = 'S';
    view[9][12].content = 'T';
    view[10][12].content = 'R';
    view[11][12].content = 'I';
    view[12][12].content = 'C';
    view[13][12].content = 'H';
    
    view[7][13].content = 'R';
    view[8][13].content = 'O';
    view[9][13].content = 'O';
    view[10][13].content = 'M';
    
    // kitchen
    view[17][12].content = 'K';
    view[18][12].content = 'I';
    view[19][12].content = 'T';
    view[20][12].content = 'C';
    view[21][12].content = 'H';
    view[22][12].content = 'E';
    view[23][12].content = 'N';
    
    // get the player positions
    
    get_positions();
    
    // create some empty chars to hold the initials that will be shown on the board
    
    char human_initial_1;
    char human_initial_2;
    char c1_initial_1;
    char c1_initial_2;
    char c2_initial_1;
    char c2_initial_2;
    char c3_initial_1;
    char c3_initial_2;
    
    // assign the human player the proper initials based on their earlier selection
    
    if (player == 1) {
        human_initial_1 = 'S';
        human_initial_2 = 'P';
    }
    else if (player == 2) {
        human_initial_1 = 'C';
        human_initial_2 = 'M';
    }
    else if (player == 3) {
        human_initial_1 = 'P';
        human_initial_2 = 'P';
    }
    else if (player == 4) {
        human_initial_1 = 'W';
        human_initial_2 = 'H';
    }
    else {
        human_initial_1 = ' ';
        human_initial_2 = ' ';
    }
    
    // assign the computer players their initials based on whoever the human didn't pick
    
    if (human_initial_1 == 'S') { // human player is scarlet pumpernickel
        c1_initial_1 = 'C';
        c1_initial_2 = 'M';
        
        c2_initial_1 = 'P';
        c2_initial_2 = 'P';
        
        c3_initial_1 = 'W';
        c3_initial_2 = 'H';
    }
    else if (human_initial_1 == 'C') { // human player is colonel mouton
        c1_initial_1 = 'S';
        c1_initial_2 = 'P';
        
        c2_initial_1 = 'P';
        c2_initial_2 = 'P';
        
        c3_initial_1 = 'W';
        c3_initial_2 = 'H';
    }
    else if (human_initial_1 == 'P') { // human player is professor pleb
        c1_initial_1 = 'C';
        c1_initial_2 = 'M';
        
        c2_initial_1 = 'S';
        c2_initial_2 = 'P';
        
        c3_initial_1 = 'W';
        c3_initial_2 = 'H';
    }
    else if (human_initial_1 == 'W') { // human player is whitney houston
        c1_initial_1 = 'C';
        c1_initial_2 = 'M';
        
        c2_initial_1 = 'P';
        c2_initial_2 = 'P';
        
        c3_initial_1 = 'S';
        c3_initial_2 = 'P';
    }
    else {
        c1_initial_1 = ' ';
        c1_initial_2 = ' ';
        c2_initial_1 = ' ';
        c2_initial_2 = ' ';
        c3_initial_1 = ' ';
        c3_initial_2 = ' ';
    }
    
    // print each player's initials at his/her current board position
    
    update_positions(player_position, human_initial_1, human_initial_2);
    update_positions(player_1_position, c1_initial_1, c1_initial_2);
    update_positions(player_2_position, c2_initial_1, c2_initial_2);
    update_positions(player_3_position, c3_initial_1, c3_initial_2);
}

void draw_board(void)
{
    // please don't call me out on how difficult this part is to follow
    // there is probably an easier way to do this but this is how I did it
    
    // it was complicated to do but I organized it as simply as I could
    // just trust me that it draws the board perfectly and makes sense
    
    for (int x=1, y=0; y<BOARD_HEIGHT; x++) {
        
        // ---------------- borders ------------------
        
        // green border
        if (x==1 || x==30 || y==0 || y==22) printf("%c[%d;%d;%dm  ", 0x1B, OFF, YELLOW, GREEN_B);
        
        // inner green border
        else if (y==4 && ((x>5 && x<10) || (x>13 && x<18) || (x>21 && x<26))) {
            printf("%c[%d;%d;%dm  ", 0x1B, OFF, YELLOW, GREEN_B);
        }
        else if (y==18 && ((x>5 && x<10) || (x>13 && x<18) || (x>21 && x<26))) {
            printf("%c[%d;%d;%dm  ", 0x1B, OFF, YELLOW, GREEN_B);
        }
        
        else if (x==6 && ((y>4 && y<7) || (y>9 && y<13) || (y>15 && y<18))) {
            printf("%c[%d;%d;%dm  ", 0x1B, OFF, YELLOW, GREEN_B);
        }
        else if (x==25 && ((y>4 && y<7) || (y>9 && y<13) || (y>15 && y<18))) {
            printf("%c[%d;%d;%dm  ", 0x1B, OFF, YELLOW, GREEN_B);
        }
        
        else if ((x==15 || x==16) && y>4 && y<18) {
            printf("%c[%d;%d;%dm  ", 0x1B, OFF, YELLOW, GREEN_B);
        }
        
        else if (y==11 && x>6 && x<25) {
            printf("%c[%d;%d;%dm  ", 0x1B, OFF, YELLOW, GREEN_B);
        }
        
        
        //-------------------- doors ----------------------
        
        else if ((y==4 && x>9 && x<14) || (y==4 && x>17 && x<22)) printf("%c[%d;%d;%dm  ", 0x1B, OFF, WHITE, BLACK_B);
        else if ((y==18 && x>9 && x<14) || (y==18 && x>17 && x<22)) printf("%c[%d;%d;%dm  ", 0x1B, OFF, WHITE, BLACK_B);
        
        else if ((x==6 && y>6 && y<10) || (x==6 && y>12 && y<16)) printf("%c[%d;%d;%dm  ", 0x1B, OFF, WHITE, BLACK_B);
        else if ((x==25 && y>6 && y<10) || (x==25 && y>12 && y<16)) printf("%c[%d;%d;%dm  ", 0x1B, OFF, WHITE, BLACK_B);
        
        //------------ top row of squares -------------
        
        // draw cyan squares
        else if (x>1 && x<6 && y>0 && y<4) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, CYAN_B, view[x][y].content); // 21
        else if (x>9 && x<14 && y>0 && y<4) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, CYAN_B, view[x][y].content); // 23
        else if (x>17 && x<22 && y>0 && y<4) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, CYAN_B, view[x][y].content); // 1
        else if (x>25 && x<30 && y>0 && y<4) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, CYAN_B, view[x][y].content); // 3
        
        // draw blue squares
        else if (x>5 && x<10 && y>0 && y<4) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, BLUE_B, view[x][y].content); // 22
        else if (x>13 && x<18 && y>0 && y<4) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, BLUE_B, view[x][y].content); // 0
        else if (x>21 && x<26 && y>0 && y<4) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, BLUE_B, view[x][y].content); // 2
        
        //------------ bottom row of squares -------------
        
        // draw cyan squares
        else if (x>1 && x<6 && y>18 && y<22) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, CYAN_B, view[x][y].content); // 15
        else if (x>9 && x<14 && y>18 && y<22) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, CYAN_B, view[x][y].content); // 13
        else if (x>17 && x<22 && y>18 && y<22) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, CYAN_B, view[x][y].content); // 11
        else if (x>25 && x<30 && y>18 && y<22) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, CYAN_B, view[x][y].content); // 9
        
        // draw blue squares
        else if (x>5 && x<10 && y>18 && y<22) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, BLUE_B, view[x][y].content); // 14
        else if (x>13 && x<18 && y>18 && y<22) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, BLUE_B, view[x][y].content); // 12
        else if (x>21 && x<26 && y>18 && y<22) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, BLUE_B, view[x][y].content); // 10
        
        //------------ left column of squares ------------
        
        // draw cyan squares
        else if (x>1 && x<6 && y>6 && y<10) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, CYAN_B, view[x][y].content); // 19
        else if (x>1 && x<6 && y>12 && y<16) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, CYAN_B, view[x][y].content); // 17
        
        // draw blue squares
        else if (x>1 && x<6 && y>3 && y<7) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, BLUE_B, view[x][y].content); // 20
        else if (x>1 && x<6 && y>9 && y<13) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, BLUE_B, view[x][y].content); // 18
        else if (x>1 && x<6 && y>15 && y<19) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, BLUE_B, view[x][y].content); // 16
        
        //------------ right column of squares ------------
        
        // draw cyan squares
        else if (x>25 && x<30 && y>6 && y<10) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, CYAN_B, view[x][y].content); // 5
        else if (x>25 && x<30 && y>12 && y<16) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, CYAN_B, view[x][y].content); // 7
        
        // draw blue squares
        else if (x>25 && x<30 && y>3 && y<7) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, BLUE_B, view[x][y].content); // 4
        else if (x>25 && x<30 && y>9 && y<13) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, BLUE_B, view[x][y].content); // 6
        else if (x>25 && x<30 && y>15 && y<19) printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLACK, BLUE_B, view[x][y].content); // 8
        
        // remaining spaces are all room squares, so draw them yellow
        else printf("%c[%d;%d;%dm%c ", 0x1B, OFF, BLUE, YELLOW_B, view[x][y].content);
        
        if (x == BOARD_WIDTH && y <= BOARD_HEIGHT) { // new line, so return colors to default and print a return
            printf("%c[%d;%d;%dm\n", 0x1B, OFF, GREEN, BLACK_B);
            x = 0;
            y++;
        }
    }
    printf("%c[%d;%d;%dm", 0x1B, OFF, GREEN, BLACK_B);
}

void draw_screen(int whichScreen)
{
    switch (screen) {
        case 0:
            introduction();
            break;
            
        case 1:
            print_backstory(option_select);
            break;
            
        case 3:
            begin_player_turn();
            break;
            
        case 5:
            print_room_info();
            break;
            
        case 9:
            help(prev_screen);
            break;
            
        case 10:
            show_eliminated(prev_screen);
            break;
            
        case 13:
            chk_suggestion(sug_character, sug_weapon, sug_room);
            break;
            
        case 14:
            chk_accusation(sug_character, sug_weapon, sug_room);
            break;
            
        case 18:
            computer_player_turn(computer_turn);
            break;
            
        default:
            break;
    }
    
    printf("\n\n");
    
    update_view();
    draw_board();
}


// ======================== INTERFACE FUNCTIONS =============================


void clear_screen(void)
{
    for (int i=0; i<100; i++) {
        printf("\n");
    }
}

void help(int whichScreen)
{
    
    switch (whichScreen) {
        case 0:
            printf("\n\nINTRODUCTION SCREEN:\n\nThis screen is where you select which character you would like ");
            printf("to play the game as.  Enter the number corresponding to your chosen character to view their ");
            printf("profile.");
            break;
            
        default:
            break;
    }
    printf("\n\nGENERAL INFORMATION:\n\nEnter a single digit corresponding to the options given on the screen to select");
    printf(" a given option.  At any time you may type 'HELP' to reach this screen or 'QUIT' to end the program.");
}

void reset_game(void)
{
    player = 0;
    screen = 0;
}

void quit(void)
{
    clear_screen();
    printf("\nThanks for playing, see you next time!\n\n");
    exit(0);
}

void introduction(void)
{
    printf("\n\nWelcome to Raging Clue, the game that is exactly like regular Clue except it's less fun.");
    printf("\n\nHere is the story:\n\nRock musician Styop Quoons has invited four complete strangers to hang out ");
    printf("at his fabulous rock-star mansion, which for some reason only has four rooms and a single connecting hallway.");
    printf("  It looks like it's going to be the party of the century, but then --  OH NO!!!  A murder has occurred!");
    printf("  And who is the victim you might ask?  Why, it is none other than Quoons himself.\n\n");
    printf("The mansion is located on a deserted island in the middle of the South Pacific, whose only current inhabitants");
    printf(" are Quoons himself and his four guests.  Therefore, we can only assume that one of his guests is A BIG MURDERER!!");
    printf("\n\nIt is up to you to investigate the mansion and determine who is the culprit.  You must ascertain");
    printf(" not only who done it, but what they done it with, and what room they done it in.");
    printf("\n\nLet's begin by choosing a character to play.");
}

void print_backstory(character theCharacter)
{
    switch (theCharacter) {
        case SCARLET:
            printf("\n\nScarlet Pumpernickel\n\nAge: 29\t\tOccupation: Baker\n\n");
            printf("------------------------------------------------------\n\n");
            printf("Scarlet Pumpernickel is a beautiful young woman with a troubled past.  At least, she was");
            printf(", until a freak baking accident involving radioactive yeast occurred.  ");
            printf("Now, she looks exactly as she did before, except that she is made entirely out of bread.  ");
            printf("As you can imagine, being made out of bread is unpleasant for her; it impedes socializing, and");
            printf(" birds always try to take a bite out of her whenever she steps outside.  A laboratory in Switzerland");
            printf(" has developed an experimental new drug designed to transform bread into human flesh, and they are");
            printf(" conducting a clinical trial.  However, there is a buy-in:  ONE HUNDRED MILLION DOLLARS.");
            printf("\n\nScarlet would probably do just about anything to get into that trial, perhaps even kill a man");
            printf(" whose net worth is EXACTLY one hundred million dollars...");
            break;
            
        case MOUTON:
            printf("\n\nColonel Mouton\n\nAge: 10\t\tOccupation: Sheep\n\n");
            printf("------------------------------------------------------\n\n");
            printf("Colonel Mouton is a sheep.  I don't mean that he is a person who tends to allow himself to be led");
            printf(" around by others; I mean that he is literally a sheep.  Because of that, you might think that he is");
            printf(" incapable of murder, on account of his lack of opposable thumbs, lack of motive and the fact that HE IS ");
            printf("A SHEEP FOR CRYING OUT LOUD.\n\nHowever, it's always the one you least suspect in these stories...");
            break;
            
        case PLEB:
            printf("\n\nProfessor Pleb\n\nAge: 77\t\tOccupation: Head of Sociology Dept.\n\n");
            printf("------------------------------------------------------\n\n");
            printf("Professor Pleb has spent the majority of his life failing at life.  When he was 12 years old he became");
            printf(" enamoured by the discipline of sociology, which, as it turns out, was probably the worst thing he ");
            printf("could possibly have become enamoured by.  He hit the books as hard as he possibly could, determined to ");
            printf("grow up and become the head of the sociology department, a goal which he accomplished by the age of 32.");
            printf("  Unfortunately, he learned soon afterward that the head of the sociology department only makes about ");
            printf("$12.00 an hour teaching sociology to sociology students.  To make matters worse, his students are ");
            printf("constantly trying to undermine him and get him fired in order to steal his job; because, as it turns out,");
            printf(" head of the sociology department is pretty much the only job a person studying sociology can hope to ");
            printf("obtain, and there is only room in the budget for one.");
            printf("\n\nOne of his brighter students is closing in fast, and the Professor knows he doesn't have");
            printf(" much time left before his position is usurped.  He needs a retirement fund, and quick...");
            break;
            
        case WHITNEY:
            printf("\n\nWhitney Houston\n\nAge: 55\t\tOccupation: Waitress\n\n");
            printf("------------------------------------------------------\n\n");
            printf("Whitney Houston used to be a marginally talented pop singer, until she developed her coke problem.");
            printf("  Is that an inappropriate thing to put in this game?  Of course not, because Coca Cola ");
            printf("is one of America's most beloved soft drinks, and I pity the fool who is offended by responsible adults");
            printf(" enjoying it in reasonable quantities.  However, some people get hooked, ");
            printf("and Whitney Houston is unfortunately one of those people.");
            printf("\n\nWhitney Houston has a Coca Cola problem, and she'll stop at nothing to get her next can...");
            break;
            
        default:
            break;
    }
}

void print_room_info(void)
{
    // player is in the Library
    
    if (player_position == 19 || player_position == 23) {
        printf("\n\nThe LIBRARY is a large, musty-smelling room packed wall to wall with shelves of books.");
        printf("  Material ranges from priceless first editions of timeless classics to leatherbound encyclopedias ");
        printf("covering seemingly every subject currently known to mankind.\n\nLol jk, it's mostly DVDs and comic books.");
    }
    
    // player is in the study
    
    else if (player_position == 1 || player_position == 5) {
        printf("\n\nThe room that Styop Quoons laughably refers to as the STUDY is really just a small, windowless room ");
        printf("containing a desk, a computer, and a mattress on the floor.\n\nThe room smells...lived in.");
    }
    
    // player is in the ostrich room
    
    else if (player_position == 13 || player_position == 17) {
        printf("\n\nThe OSTRICH ROOM is full of ostriches.  There are entirely too many of them.  'Why are there so many ");
        printf("ostriches in this room?', wonders ");
        print_player_name();
        
        char gender[4] = "";
        if (player == SCARLET || player == WHITNEY) strcpy(gender, "she");
        else strcpy(gender, "he");
        
        printf(".  \n\n'In any case,' %s mutters, 'This is a terrible vacation.'", gender);
    }
    
    // player is in the kitchen
    
    else if (player_position == 7 || player_position == 11) {
        printf("\n\nThe KITCHEN is cramped, dirty, and filled with smells so foul that ");
        print_player_name();
        printf(" can't even guess as to their cause.  The origin, however, is apparent enough;");
        printf(" the sink is filled with dishes soaked in a gelatinous brown ooze that looks like it used to be water.");
        printf("\n\nA disturbingly large rat eyes ");
        print_player_name();
        printf(" warily from its perch on top of the refrigerator.");
    }
}

void computer_player_turn(int which)
{
    send_comp_turn(which);
}

void show_eliminated(int type)
{
    get_eliminated(type);
}

void chk_suggestion(int character, int weapon, int room)
{
    send_suggestion(character, weapon, room);
}

void chk_accusation(int character, int weapon, int room)
{
    send_accusation(character, weapon, room);
}

void begin_player_turn(void)
{
    turn_action = take_next_turn();
    if (turn_action == 1) screen = 4;
    else screen = 3;
}

void print_player_name(void)
{
    switch (player) {
        case SCARLET:
            printf("Scarlet Pumpernickel");
            break;
            
        case MOUTON:
            printf("Colonel Mouton");
            break;
        
        case PLEB:
            printf("Professor Pleb");
            break;
            
        case WHITNEY:
            printf("Whitney Houston");
            break;
            
        default:
            break;
    }
}

char *format_room(int which, char *dest)
{
    if (which == KITCHEN) strcpy(dest, "kitchen");
    else if (which == LIBRARY) strcpy(dest, "library");
    else if (which == STUDY) strcpy(dest, "study");
    else if (which == OSTRICH_ROOM) strcpy(dest, "ostrich room");

    return dest;
}

char *format_character(int which, char *dest)
{
    if (which == SCARLET) strcpy(dest, "Scarlet Pumpernickel");
    else if (which == MOUTON) strcpy(dest, "Colonel Mouton");
    else if (which == PLEB) strcpy(dest, "Professor Pleb");
    else if (which == WHITNEY) strcpy(dest, "Whitney Houston");
    
    return dest;
}

char *format_weapon(int which, char *dest)
{
    if (which == HAMMER) strcpy(dest, "hammer");
    else if (which == STICK) strcpy(dest, "pointy stick");
    else if (which == PIE) strcpy(dest, "banana cream pie laced with cyanide");
    else if (which == PANTS) strcpy(dest, "pair of pants");
    else if (which == CHAINSAW) strcpy(dest, "chainsaw");
    else if (which == SHOTGUN) strcpy(dest, "shotgun");
    else if (which == WEASEL) strcpy(dest, "vicious attack weasel");
    
    return dest;
}

void options_list(void)
{
    switch (screen) {
        case 0: // intro screen
            printf("\n\n1) Scarlet Pumpernickel\n2) Colonel Mouton\n3) Professor Pleb\n4) Whitney Houston");
            break;
           
        case 1: // character info screen
            printf("\n\nChoose this character?\n1) Yes, play as this person\n2) No, select someone else");
            break;
            
        case 2: // begin turn screen
            printf("\n\nTurn: ");
            print_player_name();
            printf("\n\n1) Roll Dice\n2) Exit");
            break;
            
        case 3: // end turn screen
            printf("\n\n1) End Turn\n2) Exit\n");
            break;
            
        case 4: // player is standing at a door
            printf("\n\n1) Enter Room\n2) End Turn\n3) Exit");
            break;
            
        case 5: // player is in a room
            printf("\n\n1) Make Suggestion\n2) Make Accusation\n3) End Turn\n4) Exit");
            break;
            
        case 6: // player is making a suggestion, murderer
            printf("\n\nWho do you think the murderer was?");
            printf("\n\n1) Scarlet Pumpernickel\n2) Colonel Mouton\n3) Professor Pleb\n4) Whitney Houston\n5) Show Eliminated");
            break;
            
        case 7: // player is making a suggestion, weapon
            printf("\n\nWhat weapon do you think they used?");
            printf("\n\n1) Hammer\n2) Pointy Stick\n3) Banana Cream Pie Laced With Cyanide\n4) Pair of Pants\n5) Chainsaw");
            printf("\n6) Shotgun\n7) Vicious Attack Weasel\n8) Show Eliminated");
            break;
            
        case 8: // player is making a suggestion, room
            printf("\n\nWhat room do you think the murder happened in?");
            printf("\n\n1) Library\n2) Study\n3) Ostrich Room\n4) Kitchen\n5) Show Eliminated");
            break;
            
        case 9: // player is on the help screen
            printf("\n\n1) Return");
            break;
            
        case 10: // player is on the show eliminated screen
            printf("\n\n1) Return");
            break;
            
        case 11: // player is on the confirm suggestion screen
            {
                char name[50] = "";
                char weap[50] = "";
                char roo[50] = "";
                format_room(sug_room, roo);
                format_weapon(sug_weapon, weap);
                format_character(sug_character, name);
            
                printf("\n\nYou are suggesting that it was %s in the %s with the %s.  Is this correct?", name, roo, weap);
                printf("\n\n1) Yes, confirm\n2) No, change suggestion");
            }
            break;
            
        case 12: // player is on the confirm accusation screen
            {
                char name[50] = "";
                char weap[50] = "";
                char roo[50] = "";
                format_room(sug_room, roo);
                format_weapon(sug_weapon, weap);
                format_character(sug_character, name);
            
                printf("\n\nYou are about to make an accusation.  Whether you are right or wrong, the game will be over.");
                printf("  Before confirming, please make sure you are ABSOLUTELY CERTAIN about the facts of the case.");
                printf("\n\nYou are accusing %s of murdering Styop Quoons in the %s using the %s.  Is this correct?", name, roo, weap);
                printf("\n\n1) Yes, confirm\n2) No, change accusation\n3) Cancel accusation");
            }
            break;
            
        case 13: // player is on the end suggestion screen
            printf("\n\n1) End Turn\n2) Exit");
            break;
            
        case 14: // player is on the end accusation screen
            break;
            
        case 15: // player is making an accusation, murderer
            printf("\n\nWho do want to accuse?");
            printf("\n\n1) Scarlet Pumpernickel\n2) Colonel Mouton\n3) Professor Pleb\n4) Whitney Houston\n5) Show Eliminated");
            break;
            
        case 16: // player is making an accusation, weapon
            printf("\n\nWhat weapon did they use?");
            printf("\n\n1) Hammer\n2) Pointy Stick\n3) Banana Cream Pie Laced With Cyanide\n4) Pair of Pants\n5) Chainsaw");
            printf("\n6) Shotgun\n7) Vicious Attack Weasel\n8) Show Eliminated");
            break;
            
        case 17: // player is making an accusation, room
            printf("\n\nIn what room did they do the dastardly deed?");
            printf("\n\n1) Library\n2) Study\n3) Ostrich Room\n4) Kitchen\n5) Show Eliminated");
            break;
            
        case 18: // computer players' turn screen
            printf("\n\n1) Next Turn\n2) Exit");
            
        default:
            break;
    }
    printf("\n\nType 'HELP' for assistance.\nType 'QUIT' to end program.");
}

int read_response(char response[100])
{
    // remove the extra /n char at the end of response, thanks to user Tim Čas of stackoverflow.com for this fix
    
    response[strcspn(response, "\n")] = 0;
    
    // redirect player to appropriate place based on their current screen
    
    if (strchr(response, 'HELP')) {
        if (screen == 9) return 1;
        else {
            prev_screen = screen;
            screen = 9; // send player to help screen
            return 1; // continue loop
        }
    }
    else if (strchr(response, 'QUIT')) {
        quit();
    }
    
    switch (screen) {
        case 0: // introduction screen
            if (strchr(response, '1')) {
                option_select = 1;
                screen = 1; // send player to character info screen
                return  1; // return 1, loop continues
            }
            else if (strchr(response, '2')) {
                option_select = 2;
                screen = 1; // send player to character info screen
                return 1;
            }
            else if (strchr(response, '3')) {
                option_select = 3;
                screen = 1;
                return 1;
            }
            else if (strchr(response, '4')) {
                option_select = 4;
                screen = 1; // send player to character info screen
                return 1;
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;
            
        case 1: // character select screen
            if (strchr(response, '1')) {
                player = option_select;
                screen = 2; // move on to the game
                return  1; // return 1, loop continues
            }
            else if (strchr(response, '2')) {
                reset_game();
                return 1;
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;
            
        case 2: // begin turn screen
            if (strchr(response, '1')) {
                screen = 3;
                return 1;
            }
            else if (strchr(response, '2')) {
                reset_game();
                return 1;
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;
            
        case 3: // end turn screen
            if (strchr(response, '1')) {
                screen = 18; // player chose to end turn, so move on to computer players' turns
                return 1; // return 1, loop continues
            }
            else if (strchr(response, '2')) {
                reset_game(); // player chose to exit, so return to main menu
                return 1; // return 1, loop continues
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;
            
        case 4: // player is standing at a door
            if (strchr(response, '1')) {
                screen = 5; // player chose to enter, so advance the screen to the room screen
                return 1;
            }
            else if (strchr(response, '2')) {
                screen = 18; // player chose not to enter, so end the turn
                return 1;
            }
            else if (strchr(response, '3')) {
                reset_game(); // player chose to exit, so return to main menu
                return 1; // return 1, loop continues
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;
            
        case 5: // player is in a room
            if (strchr(response, '1')) {
                screen = 6; // player is making a suggestion, so go to first suggestion screen
                return 1;
            }
            else if (strchr(response, '2')) {
                // make accusation
                screen = 15; // player is making an accusation so go to first accusation screen
                return 1;
            }
            else if (strchr(response, '3')) {
                screen = 18; // player chose not to do anything, so end the turn
                return 1; // return 1, loop continues
            }
            else if (strchr(response, '4')) {
                reset_game(); // player chose to exit, so return to main menu
                return 1; // return 1, loop continues
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;
            
        case 6: // suggestion screen, choose murderer
            if (strchr(response, '1')) {
                // scarlet
                sug_character = SCARLET;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '2')) {
                // mouton
                sug_character = MOUTON;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '3')) {
                // pleb
                sug_character = PLEB;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '4')) {
                // whitney
                sug_character = WHITNEY;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '5')) {
                // show eliminated
                prev_screen = screen;
                screen = 10; // go to show eliminated screen
                return 1; // return 1, game continues
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            
            break;
            
        case 7: // suggestion screen, choose weapon
            if (strchr(response, '1')) {
                // hammer
                sug_weapon = HAMMER;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '2')) {
                // pointy stick
                sug_weapon = STICK;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '3')) {
                // pie
                sug_weapon = PIE;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '4')) {
                // pair of pants
                sug_weapon = PANTS;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '5')) {
                // chainsaw
                sug_weapon = CHAINSAW;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '6')) {
                // shotgun
                sug_weapon = SHOTGUN;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '7')) {
                // attack weasel
                sug_weapon = WEASEL;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '8')) {
                // show eliminated
                prev_screen = screen;
                screen = 10;
                return 1; // return 1, game continues
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            
            break;
            
        case 8: // suggestion screen, choose room
            if (strchr(response, '1')) {
                // library
                sug_room = LIBRARY;
                screen = 11;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '2')) {
                // study
                sug_room = STUDY;
                screen = 11;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '3')) {
                // ostrich room
                sug_room = OSTRICH_ROOM;
                screen = 11;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '4')) {
                // kitchen
                sug_room = KITCHEN;
                screen = 11;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '5')) {
                // show eliminated
                prev_screen = screen;
                screen = 10;
                return 1; // return 1, game continues
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;

        
        case 9:  // help screen
            if (strchr(response, '1')) {
                screen = prev_screen;
                return 1;
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;
            
        case 10:  // show eliminated screen
            if (strchr(response, '1')) {
                screen = prev_screen;
                return 1;
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;
            
        case 11: // confirm suggestion screen
            if (strchr(response, '1')) {
                screen = 13; // confirmed, end suggestion
                return 1; // return 1, game continues
            }
            else if (strchr(response, '2')) {
                screen = 6; // player chose no, redo suggestion
                return 1; // return 1, game continues
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;
            
        case 12: // confirm accusation screen
            if (strchr(response, '1')) {
                screen = 14; // confirmed, end accusation
                return 1; // return 1, game continues
            }
            else if (strchr(response, '2')) {
                screen = 15; // player chose no, redo accusation
                return 1; // return 1, game continues
            }
            else if (strchr(response, '3')) {
                screen = 5; // player chose to cancel, so go back to the room screen
                return 1;
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;
            
        case 13: // end suggestion screen
            if (strchr(response, '1')) {
                screen = 18; // player chose to end turn, so go back to begin turn screen for now
                return 1; // return 1, loop continues
            }
            else if (strchr(response, '2')) {
                reset_game(); // player chose to exit, so return to main menu
                return 1; // return 1, loop continues
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;
            
        case 14: // end accusation screen
            if (strchr(response, '1')) {
                return 1;
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;
            
        case 15: // accusation screen, choose murderer
            if (strchr(response, '1')) {
                // scarlet
                sug_character = SCARLET;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '2')) {
                // mouton
                sug_character = MOUTON;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '3')) {
                // pleb
                sug_character = PLEB;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '4')) {
                // whitney
                sug_character = WHITNEY;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '5')) {
                // show eliminated
                prev_screen = screen;
                screen = 10; // go to show eliminated screen
                return 1; // return 1, game continues
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            
            break;
            
        case 16: // accusation screen, choose weapon
            if (strchr(response, '1')) {
                // hammer
                sug_weapon = HAMMER;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '2')) {
                // pointy stick
                sug_weapon = STICK;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '3')) {
                // pie
                sug_weapon = PIE;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '4')) {
                // pair of pants
                sug_weapon = PANTS;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '5')) {
                // chainsaw
                sug_weapon = CHAINSAW;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '6')) {
                // shotgun
                sug_weapon = SHOTGUN;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '7')) {
                // attack weasel
                sug_weapon = WEASEL;
                screen ++;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '8')) {
                // show eliminated
                prev_screen = screen;
                screen = 10;
                return 1; // return 1, game continues
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            
            break;
            
        case 17: // accusation screen, choose room
            if (strchr(response, '1')) {
                // library
                sug_room = LIBRARY;
                screen = 12;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '2')) {
                // study
                sug_room = STUDY;
                screen = 12;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '3')) {
                // ostrich room
                sug_room = OSTRICH_ROOM;
                screen = 12;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '4')) {
                // kitchen
                sug_room = KITCHEN;
                screen = 12;
                return 1; // return 1, game continues
            }
            else if (strchr(response, '5')) {
                // show eliminated
                prev_screen = screen;
                screen = 10;
                return 1; // return 1, game continues
            }
            else {
                printf("\nYou don't need to use the phrase '%s' in this game.\n", response);
                printf("PRESS ENTER TO CONTINUE.");
                fgets(response, 100, stdin);
                return 1; // return 1, loop continues to run
            }
            break;
            
        case 18: // computer players' turns
            if (strchr(response, '1')) {
                // player chose to end turn
                
                if (computer_turn < 2) computer_turn ++;
                else {
                    computer_turn = 0;
                    screen = 2;
                }
                return 1; // return 1, loop continues
            }
            else if (strchr(response, '2')) {
                reset_game(); // player chose to exit, so return to main menu
                return 1; // return 1, loop continues
            }
            
        default:
            break;
    }
    return 0;
}

int user_prompt(void)
{    char response[100];
    int running;
    
    options_list();
        
    printf("\n\nEnter your response:\n");
    fgets(response, 100, stdin);
        
    running = read_response(response);
    return running;
}


// ======================= EXPORT FUNCTIONS =============================


int send_player_data(void)
{
    int playerName = player;
    return playerName;
}

void game_start(void)
{
    init_view();
    draw_screen(screen);
    
    while (user_prompt() != 0) {
        clear_screen();
        draw_screen(screen);
    }
}


