#include "server.h"

int main(int argc, char *argv[]){
    initArgs(argc, argv); // initialize ipVersion, port and inputFilePath
    initBoard(answerBoard); // initialize answerBoard with values read from file
    int sockfd = initSocket(); // initialize server's socket
    while(true){
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *) &cstorage;
        socklen_t caddrlen = sizeof(cstorage);
        int csock = accept(sockfd, caddr, &caddrlen); // complete connection between server and client
        if(csock == -1) logexit("accept");
        printf("client connected\n");

        while(true){
            struct action requestFromClient;
            int count = recv(csock, &requestFromClient, sizeof(requestFromClient), 0);
            if(count == 0) break; // didn't receive a message from a client
            if(count == -1) logexit("recv");

            struct action responseToClient;

            switch(requestFromClient.type){
                case START:
                    // hide all the cells from clientBoard and return a STATE response
                    resetClientBoard();
                    responseToClient = actionInit(STATE, requestFromClient.coordinates, clientBoard);
                    break;
                case REVEAL:
                    // reveal cell from clientBoard at the desired coordnates and return a STATE, WIN or GAME_OVER response
                    responseToClient = changeClientBoardCell(requestFromClient.coordinates, answerBoard[requestFromClient.coordinates[0]][requestFromClient.coordinates[1]], REVEAL);
                    break;
                case FLAG:
                    // flag cell from clientBoard at the desired coordnates and return a STATE response
                    responseToClient = changeClientBoardCell(requestFromClient.coordinates, FLAGGED, FLAG);
                    break;
                case REMOVE_FLAG:
                    // remove flag from clientBoard at the desired coordnates and return a STATE response
                    responseToClient = changeClientBoardCell(requestFromClient.coordinates, HIDDEN, REMOVE_FLAG);
                    break;
                case RESET:
                    // hide all cells from clientBoard and return STATE response
                    resetClientBoard();
                    responseToClient = actionInit(STATE, requestFromClient.coordinates, clientBoard);
                    printf("starting new game\n");
                    break;
                case EXIT:
                    // hide all cells from clientBoard and don`t return any response
                    resetClientBoard();
                    printf("client disconnected\n");
                    break;
            }


            if(responseToClient.type == GAME_OVER || responseToClient.type == WIN) resetClientBoard(); // the game was finished
            count = send(csock, &responseToClient, sizeof(responseToClient), 0); // send response to client
            if(count != sizeof(responseToClient)) logexit("send");
        }
    }
    close(sockfd); // close server
}

void initArgs(int argc, char *argv[]){
    if(argc != 5 || strcmp(argv[3], "-i") != 0){
        logexit("Usage: ./server <ipVersion> <port> -i <inputFilePath>");
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
    // hide all cells
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            clientBoard[i][j] = HIDDEN;
        }
    }
    amountOfNotBombCells = MAX*MAX - AMOUNT_OF_BOMBS; // reset the amount of hidden cells
}

int serverSockaddrInit(const char *ipProtocol, const char *portstr, struct sockaddr_storage *storage){
    uint16_t port = (uint16_t) atoi(portstr);
    if(port == 0) return -1;
    port = htons(port);

    memset(storage, 0, sizeof(*storage));

    if(0 == strcmp(ipProtocol, "v4")){
        struct sockaddr_in *addr4 = (struct sockaddr_in *) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr.s_addr = htonl(INADDR_ANY); //Qualquer add disponivel no computador
        return 0;
    }
    else if(0 == strcmp(ipProtocol, "v6")){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        addr6->sin6_addr = in6addr_any; //Qualquer add disponivel no computador v6
        return 0;
    }
    else return -1;
}

int initSocket(){
    // initialize address
    struct sockaddr_storage storage;
    if(serverSockaddrInit(ipVersion, port, &storage)) logexit("serverSockaddrInitt");
    // initialize socket
    int sockfd = socket(storage.ss_family, SOCK_STREAM, 0);
    if(sockfd == -1) logexit("socket");
    // reuse address
    int enable = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0) logexit("setsockopt");
    // bind to address
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(bind(sockfd, addr, sizeof(storage)) != 0) logexit("bind");
    // listen for connections
    if(listen(sockfd, 10) != 0) logexit("listen");
    return sockfd;
}

struct action changeClientBoardCell(int coordinates[2], int newValue, int requestType){
    if(IS_BOMB(newValue)) return actionInit(GAME_OVER, coordinates, answerBoard); // a bomb was revealed -> GAME_OVER response
    if(GAME_WON) return actionInit(WIN, coordinates, answerBoard); // if all not bombs cells were revealed -> WIN response
    if(requestType == REVEAL) amountOfNotBombCells--; // reveal another not bomb cell
    clientBoard[coordinates[0]][coordinates[1]] = newValue; 
    return actionInit(STATE, coordinates, clientBoard); // a single cell was changed -> STATE response
}