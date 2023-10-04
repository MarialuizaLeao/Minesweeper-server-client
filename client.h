#include "common.h"

int board[MAX][MAX];

char *ipVersion = "";
char *port = "";

#define VALID_COORD(x,y) (x >= 0 && x < MAX && y >= 0 && y < MAX)

void initArgs(int argc, char *argv[]); // inicialize arguments
void copyBoard(int Updatedboard[MAX][MAX]); // copy board from server to local variable
bool validAction(struct action action); // check if action is valid
int parseCommand(char *cmd); // check which command was called
int setSocket();