#include "common.h"

int answerBoard[MAX][MAX];
int clientBoard[MAX][MAX];

void printBoard(int board[MAX][MAX]){
    for(int i=0; i < MAX; i++){
        for(int j=0; j < MAX; j++){
            if(board[i][j] == EMPTY)
                printf("0\t\t");
            else if (board[i][j] == BOMB){
                printf("*\t\t");
            }
            else if(board[i][j] == FLAGGED){
                printf(">\t\t");
            }
            else if(board[i][j] == HIDDEN){
                printf("-\t\t");
            }
            else{
                printf("%d\t\t", board[i][j]);
            }

        }
        printf("\n");
    }
    printf("\n");
}

void initArgs(int argc, char *argv[]){
    if(argc != 5 || strcmp(argv[3], "-i") != 0){
        printf("Usage: ./server <ipVersion> <port> -i <inputFilePath>\n");
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
            clientBoard[i][j] = -2;
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
            if((clientBoard[i][j] != answerBoard[i][j] && clientBoard[i][j] != FLAGGED) || (clientBoard[i][j] == FLAGGED && answerBoard[i][j] != BOMB)){
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

bool checkNewState(struct action request){
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

        char addrstr[BUFSZ];
        addrtostr(addr, addrstr, BUFSZ);
        printf("bound to %s, waiting connections\n", addrstr);

        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *) &cstorage;
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
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
            struct action *response;
            switch(request.type){
            case START:
                resetBoard();
                int coordinates[2] = {0,0};
                response = initAction(STATE, coordinates, clientBoard);
                break;
            case REVEAL:
                if(checkNewState(request) == STATE){
                    revealCell(request.coordinates);
                    response = initAction(STATE, request.coordinates, clientBoard);
                }
                else if(checkNewState(request) == GAME_OVER){
                    response = initAction(GAME_OVER, request.coordinates, answerBoard);
                }
                else if(checkNewState(request) == WIN){
                    response = initAction(WIN, request.coordinates, answerBoard);
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
            default:
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