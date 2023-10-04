#include "common.h"

char *ipVersion = "";
char *port = "";
char * inputFilePath = NULL;

int answerBoard[MAX][MAX];
int clientBoard[MAX][MAX];

void initArgs(int argc, char *argv[]){
    if(argc != 5 || strcmp(argv[3], "-i") != 0){
        errorHandler(SERVER_USAGE_ERROR);
        exit(1);
    }
    else{
        ipVersion = argv[1];
        port = argv[2];
        inputFilePath = argv[4];
    }
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
}

void resetBoard(){
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            clientBoard[i][j] = HIDDEN;
        }
    }
}

void revealCell(int coordinates[2]){
    clientBoard[coordinates[0]][coordinates[1]] = answerBoard[coordinates[0]][coordinates[1]];
}

void flagCell(int coordinates[2]){
    clientBoard[coordinates[0]][coordinates[1]] = FLAGGED;
}

void removeFlag(int coordinates[2]){
    clientBoard[coordinates[0]][coordinates[1]] = HIDDEN;
}

bool win(struct action request){
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            if(!(request.coordinates[0] == i && request.coordinates[1] == j) && (clientBoard[i][j] != answerBoard[i][j] && answerBoard[i][j] != BOMB)){
                return false;
            }
        }
    }
    return true;
}

// check if action will reveal a bomb
bool revealBomb(struct action request){
    if(answerBoard[request.coordinates[0]][request.coordinates[1]] == BOMB){
        return true;
    }
    return false;
}

int checkNewState(struct action request){
    if(revealBomb(request)){
        return GAME_OVER;
    }
    else if(win(request)){
        return WIN;
    }
    else{
        return STATE;
    }
}

int main(int argc, char *argv[]){
    initArgs(argc, argv);

    initBoard(answerBoard);
    printBoard(answerBoard);

    struct sockaddr_storage storage;
    if(server_sockaddr_init(ipVersion, port, &storage)){
        logexit("server_sockaddr_init");
    }

    // Socket
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s == -1){
        logexit("socket");
    }

    // Reuse
    int enable = 1;
    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0){
        logexit("setsockopt");
    }

    // Bind
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(bind(s, addr, sizeof(storage)) != 0){
        logexit("bind");
    }

    // Listen
    if(listen(s, 10) != 0){
        logexit("listen");
    }

    while(true){
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *) &cstorage;
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        printf("client connected\n");
        if(csock == -1){
            logexit("accept");
        }
        while(true){
            struct action request;
            int count = recv(csock, &request, sizeof(request), 0);

            if(count == 0){
                break;
            } else if(count == -1){
                logexit("recv");
            }

            struct action response;

            switch(request.type){
                case START:
                    resetBoard();
                    response = initAction(STATE, request.coordinates, clientBoard);
                    break;
                case REVEAL:
                    revealCell(request.coordinates);
                    switch(checkNewState(request)){
                        case STATE:
                            revealCell(request.coordinates);
                            response = initAction(STATE, request.coordinates, clientBoard);
                            break;
                        case GAME_OVER:
                            response = initAction(GAME_OVER, request.coordinates, answerBoard);
                            resetBoard();
                            break;
                        case WIN:
                            response = initAction(WIN, request.coordinates, answerBoard);
                            resetBoard();
                            break;
                    }
                    break;
                case FLAG:
                    flagCell(request.coordinates);
                    response = initAction(STATE, request.coordinates, clientBoard);
                    break;
                case REMOVE_FLAG:
                    removeFlag(request.coordinates);
                    response = initAction(STATE, request.coordinates, clientBoard);
                    break;
                case RESET:
                    resetBoard();
                    response = initAction(STATE, request.coordinates, clientBoard);
                    printf("starting new game\n");
                    break;
                case EXIT:
                    resetBoard();
                    printf("client disconnected\n");
                    break;
                }

                count = send(csock, &response, sizeof(response), 0);
                if(count != sizeof(response)){
                    logexit("send");
                }
                
        }
        // Close
        close(csock);
    }
}