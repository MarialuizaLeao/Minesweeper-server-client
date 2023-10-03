#include "common.h"
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

int answerBoard[MAX][MAX];
char *ipVersion = "";
char *port = "";
char *inputFilePath = "";

struct action{
    int type;
    int coordinates[2];
    int board[4][4];
};

void printBoard(struct action *action){
    for(int i=0; i < MAX; i++){
        for(int j=0; j < MAX; j++){
            if(action->board[i][j] == -1)
                printf(" ");
            else if (action->board[i][j] == -2){
                printf("-\t\t");
            }
            else if(action->board[i][j] == -3){
                printf(">\t\t");
            }
            else{
                printf("%d\t\t", action->board[i][j]);
            }

        }
        printf("\n");
    }
    printf("\n");
}

void initArgs(int argc, char *argv[]){
    if(argc != 5 || strcmp(argv[3], "-i") != 0){
        printf("Usage: ./server -t <ipVersion> -p <port>\n");
        exit(1);
    }
    else{
        ipVersion = argv[1];
        port = argv[2];
        inputFilePath = argv[4];
    }
}

void initBoard(struct action *action){
    FILE *fp = fopen(inputFilePath, "r");
    if(fp == NULL){
        printf("Error opening file\n");
        exit(1);
    }
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            fscanf(fp, "%d,", &action->board[i][j]);
        }
    }
    fclose(fp);
}

void resetBoard(struct action *action){
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            action->board[i][j] = -2;
        }
    }
}

void updateBoard(struct action *action, int newValue){
    action->board[action->coordinates[0]][action->coordinates[1]] = newValue;
}

bool win(struct action *action){
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            if(action->board[i][j] != answerBoard[i][j] && action->board[i][j] == -1){
                return false;
            }
        }
    }
    return true;
}

bool gameOver(struct action *action){
    return !(action->board[action->coordinates[0]][action->coordinates[1]] == -1);
}

int main(int argc, char *argv[]){
    initArgs(argc, argv);

    struct action action;
    initBoard(&action);

    
    printf("ipVersion: %s, port: %s, inputFilePath: %s\n", ipVersion, port, inputFilePath);
    initBoard(&action);
    printBoard(&action);
}