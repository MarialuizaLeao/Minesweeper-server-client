#pragma once

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX 4
#define AMOUNT_OF_BOMBS 3
#define BUFSZ 1024

// labels for each type of action
#define ERROR -1
#define START 0 // from client -> server at the beginning of the game
#define REVEAL 1 // from client -> server to reveal a cell
#define FLAG 2 // from client -> server to flag a cell
#define STATE 3 // from server -> client everytime the state of the game changes
#define REMOVE_FLAG 4 // from client -> server to remove a flag
#define RESET 5 // from client to server -> reset the game
#define WIN 6 // from server -> client when the game is won
#define EXIT 7 // from client -> server to exit the game and disconnect
#define GAME_OVER 8 // from server -> client when the game is lost

// labels for each type of cell
#define BOMB -1
#define HIDDEN -2
#define FLAGGED -3
#define EMPTY 0
#define REVEALED(a) (a == 1|| a == 2|| a == 3 || a == 4 || a == 5|| a == 6)

struct action{
  int type;
  int coordinates[2];
  int board[MAX][MAX];
};

struct action actionInit(int type, int coordinates[2], int board[MAX][MAX]);

void logexit(const char *msg);
void printBoard(int board[MAX][MAX]);
