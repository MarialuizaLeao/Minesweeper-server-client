#ifndef SERVER_H
#define SERVER_H

#include "common.h"

#define IS_BOMB(a) a == BOMB
#define GAME_WON (amountOfNotBombCells - 1) == 0

char *ipVersion = "";
char *port = "";
char * inputFilePath = NULL;

int answerBoard[MAX][MAX];
int clientBoard[MAX][MAX];
int amountOfNotBombCells = MAX*MAX - AMOUNT_OF_BOMBS;

void initArgs(int argc, char *argv[]); // inicialize arguments
void initBoard(); // inicialize board
int serverSockaddrInit(const char *ipProtocol, const char *portstr, struct sockaddr_storage *storage);
int initSocket(); // inicialize socket
void resetClientBoard(); // reset client board
struct action changeClientBoardCell(int coordinates[2], int newValue, int requestType); // change client board cell and return action with the server response to client

#endif