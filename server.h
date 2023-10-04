#include "common.h"

#define IS_BOMB(a) a == BOMB
#define GAME_WON (amountOfNotBombCells - 1) == 0

char *ipVersion = "";
char *port = "";
char * inputFilePath = NULL;

int answerBoard[MAX][MAX];
int clientBoard[MAX][MAX];
int amountOfNotBombCells = MAX*MAX - AMOUNT_OF_BOMBS;

void initArgs(int argc, char *argv[]);
void initBoard();
void resetClientBoard();
int setSocket();
struct action changeClientBoardCell(int coordinates[2], int newValue, int requestType);