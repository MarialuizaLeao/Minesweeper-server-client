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
#define BUFSZ 1024

#define ERROR -1
#define START 0 // from client to server at the beginning of the game
#define REVEAL 1 // from client to server to reveal a cell
#define FLAG 2 // from client to server to flag a cell
#define STATE 3 // from server to client everytime the state of the game changes
#define REMOVE_FLAG 4 // from client to server to remove a flag
#define RESET 5 // from client to server to reset the game
#define WIN 6 // from server to client when the game is won
#define EXIT 7 // from client to server to exit the game and disconnect
#define GAME_OVER 8 // from server to client when the game is lost

#define BOMB -1
#define HIDDEN -2
#define FLAGGED -3
#define EMPTY 0
#define REVEALED(a) (a == 1|| a == 2|| a == 3 || a == 4 || a == 5|| a == 6)

#define COMMAND_ERROR "error: command not found"
#define INVALID_CELL_ERROR "error: invalid cell"
#define REVEAL_ALREADY_REVEALED_CELL_ERROR "error: cell already revealed"
#define FLAG_ALREADY_FLAGGED_CELL_ERROR "error: cell already has a flag"
#define FLAG_ALREADY_REVEALED_CELL_ERROR "error: cannot insert flag in revealed cell"
#define CLIENT_USAGE_ERROR "Usage: ./client <ipVersion> <port>"
#define SERVER_USAGE_ERROR "Usage: ./server <ipVersion> <port> -i <inputFilePath>"

struct action{
  int type;
  int coordinates[2];
  int board[4][4];
};

struct action initAction(int type, int coordinates[2], int board[4][4]);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);
int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);
int server_sockaddr_init(const char *protocol_version, const char *portstr, struct sockaddr_storage *storage);
void logexit(const char *msg);

void printBoard(int board[MAX][MAX]);
void errorHandler(char *error);
