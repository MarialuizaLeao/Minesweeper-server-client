#include "common.h"

#define MAX 4
#define BUFSZ 1024

int answerBoard[MAX][MAX];
char *ipVersion = "";
char *port = "";
char *inputFilePath = "";

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
    printBoard(&action);

     struct sockaddr_storage storage;
    if(server_sockaddr_init(ipVersion, port, &storage)){
        logexit("server_sockaddr_init");
    }

    // Socket
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if( s == -1 ){
        logexit("socket");
    }

    // Reuse
    int enable = 1;
    if( 0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))){
        logexit("setsockopt");
    }

    // Bind
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if( 0 != bind(s, addr, sizeof(storage)) ){
        logexit("bind");
    }

    // Listen
    if( 0 != listen(s, 10) ){
        logexit("listen");
    }

    // Accept
    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);
    int csock;

    while(1){
        csock = accept(s, caddr, &caddrlen);
        if(csock == -1){
            logexit("accept");
        }
        addrtostr(caddr, addrstr, BUFSZ);
        printf("connection from %s\n", addrstr);

        // Receive
        uint8_t buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        int count = recv(csock, buf, BUFSZ-1, 0);
        printf("received %d bytes\n", count);
        if( count == -1 ){
            logexit("recv");
        }
        printf("received message: %s\n", buf);

        // Send
        char *msg = "Hello, friend!";
        count = send(csock, msg, strlen(msg)+1, 0);
        printf("sent %d bytes\n", count);
        if( count != strlen(msg)+1 ){
            logexit("send");
        }

        // Close
        close(csock);
    }
}