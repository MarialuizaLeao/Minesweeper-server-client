#include "common.h"

struct action actionInit(int type, int coordinates[2], int board[MAX][MAX]){
    struct action action;
    action.type = type;
    action.coordinates[0] = coordinates[0];
    action.coordinates[1] = coordinates[1];
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            action.board[i][j] = board[i][j];
        }
    }
    return action;
}

void logexit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void printBoard(int board[MAX][MAX]){
    for(int i=0; i < MAX; i++){
        for(int j=0; j < MAX; j++){
            if(board[i][j] == EMPTY) printf("0\t\t");
            else if (board[i][j] == BOMB) printf("*\t\t");
            else if(board[i][j] == FLAGGED) printf(">\t\t");
            else if(board[i][j] == HIDDEN) printf("-\t\t");
            else printf("%d\t\t", board[i][j]);
        }
        printf("\n");
    }
}