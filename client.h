#ifndef CLIENT_H
#define CLIENT_H


#include "common.h"

int board[MAX][MAX];

char *ip = ""; // ip address
char *port = ""; // port

#define VALID_COORD(x,y) (x >= 0 && x < MAX && y >= 0 && y < MAX)

void initArgs(int argc, char *argv[]); // inicialize arguments
int clientSockaddrInit(const char *ip, const char *portstr, struct sockaddr_storage *storage); // inicialize address
int socketInit(); // inicialize socket
int parseCommand(char *cmd); // check which command was called
void copyBoard(int Updatedboard[MAX][MAX]); // copy board from server to local variable
bool validAction(int type, int coordinates[2]); // check if action is valid

#endif