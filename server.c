#include "server.h"

int main(int argc, char *argv[]){

    initArgs(argc, argv);
    initBoard(answerBoard);
    int sockfd = setSocket();

    while(true){

        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *) &cstorage;
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(sockfd, caddr, &caddrlen);
        if(csock == -1) logexit("accept");
        printf("client connected\n");

        while(true){

            struct action requestFromClient;
            int count = recv(csock, &requestFromClient, sizeof(requestFromClient), 0);
            if(count == 0) break;
            if(count == -1) logexit("recv");

            struct action responseToClient;

            switch(requestFromClient.type){
                case START:
                    resetClientBoard();
                    responseToClient = initAction(STATE, requestFromClient.coordinates, clientBoard);
                    break;
                case REVEAL:
                    responseToClient = changeClientBoardCell(requestFromClient.coordinates, answerBoard[requestFromClient.coordinates[0]][requestFromClient.coordinates[1]], REVEAL);
                    break;
                case FLAG:
                    responseToClient = changeClientBoardCell(requestFromClient.coordinates, FLAGGED, FLAG);
                    break;
                case REMOVE_FLAG:
                    responseToClient = changeClientBoardCell(requestFromClient.coordinates, HIDDEN, REMOVE_FLAG);
                    break;
                case RESET:
                    resetClientBoard();
                    responseToClient = initAction(STATE, requestFromClient.coordinates, clientBoard);
                    printf("starting new game\n");
                    break;
                case EXIT:
                    resetClientBoard();
                    printf("client disconnected\n");
                    break;
            }

            if(responseToClient.type == GAME_OVER || responseToClient.type == WIN) resetClientBoard();
            count = send(csock, &responseToClient, sizeof(responseToClient), 0);
            if(count != sizeof(responseToClient)) logexit("send");
        }
        close(csock);
    }
}

void initArgs(int argc, char *argv[]){
    if(argc != 5 || strcmp(argv[3], "-i") != 0){
        errorHandler(SERVER_USAGE_ERROR);
        exit(1);
    }
    ipVersion = argv[1];
    port = argv[2];
    inputFilePath = argv[4];
}

void initBoard(){
    FILE *fp = fopen(inputFilePath, "r");
    if(fp == NULL){
        printf("Error opening file\n");
        exit(1);
    }
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            fscanf(fp, "%d,", &answerBoard[i][j]);
        }
    }
    fclose(fp);
    printBoard(answerBoard);
}

void resetClientBoard(){
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            clientBoard[i][j] = HIDDEN;
        }
    }
    amountOfNotBombCells = MAX*MAX - AMOUNT_OF_BOMBS;
}

int setSocket(){
    struct sockaddr_storage storage;
    if(server_sockaddr_init(ipVersion, port, &storage)) logexit("server_sockaddr_init");
    // Socket
    int sockfd = socket(storage.ss_family, SOCK_STREAM, 0);
    if(sockfd == -1) logexit("socket");
    // Reuse
    int enable = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0) logexit("setsockopt");
    // Bind
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(bind(sockfd, addr, sizeof(storage)) != 0) logexit("bind");
    // Listen
    if(listen(sockfd, 10) != 0) logexit("listen");
    return sockfd;
}

struct action changeClientBoardCell(int coordinates[2], int newValue, int requestType){
    if(IS_BOMB(newValue)) return initAction(GAME_OVER, coordinates, answerBoard);
    if(GAME_WON) return initAction(WIN, coordinates, answerBoard);
    if(requestType == REVEAL) amountOfNotBombCells--;
    clientBoard[coordinates[0]][coordinates[1]] = newValue;
    return initAction(STATE, coordinates, clientBoard);
}