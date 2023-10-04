#include "client.h"

int main(int argc, char **argv){

    initArgs(argc, argv);
    int sockfd = setSocket();

    char input[BUFSZ]; // buffer for input

    while(true){

        scanf("%s", input);
        int cmdType = parseCommand(input);

        int coordnates[2];
        bool canSendRequestToServer = true;

        switch(cmdType){
            case ERROR:
                errorHandler(COMMAND_ERROR);
                canSendRequestToServer = false;
                break;
            default:
                break;
        }

        if(cmdType == REVEAL || cmdType == FLAG || cmdType == REMOVE_FLAG){
            scanf("%d,%d", &coordnates[0], &coordnates[1]);
            if(!validAction(cmdType, coordnates)) canSendRequestToServer = false;
        }

        if(!canSendRequestToServer) continue;

        struct action requestToServer = initAction(cmdType, coordnates, board);
        int count = send(sockfd, &requestToServer, sizeof(requestToServer), 0);
        if(count != sizeof(requestToServer)){
            logexit("send");
        }

        if(cmdType != EXIT){
            struct action responseFromServer;
            count = recv(sockfd, &responseFromServer, sizeof(responseFromServer), 0);
            if(count != sizeof(responseFromServer)){
                    logexit("send");
            }
            if(cmdType == START) printf("game started\n");
            copyBoard(responseFromServer.board);
            if(responseFromServer.type == WIN) {
                printf("YOU WIN!\n");
            } else if(responseFromServer.type == GAME_OVER){
                printf("GAME OVER!\n");
            }
            printBoard(board);
        }
        else{
            close(sockfd);
            break;
        }

    }
}

void initArgs(int argc, char *argv[]){
    if(argc != 3){
        errorHandler(CLIENT_USAGE_ERROR);
        exit(1);
    }
    else{
        ipVersion = argv[1];
        port = argv[2];
    }
}

int setSocket(){
    // inicialize comunication
    struct sockaddr_storage storage;
    if(addrparse(ipVersion, port, &storage) != 0){
        logexit("addrparse");
    }

    // inicialize socket
    int sockfd = socket(storage.ss_family, SOCK_STREAM, 0);
    if(sockfd == -1){
        logexit("socket");
    }

    // inicialize connection
    struct sockaddr *addr = (struct sockaddr *)(&storage); 
    if(connect(sockfd, addr, sizeof(storage)) != 0){
        logexit("connect");
    }
    return sockfd;
}

void copyBoard(int Updatedboard[MAX][MAX]){
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            board[i][j] = Updatedboard[i][j];
        }
    }
}

bool validAction(int type, int coordinates[2]){
    if(!VALID_COORD(coordinates[0], coordinates[1])){
        errorHandler(INVALID_CELL_ERROR);
        return false;
    }
    else if(type == REVEAL && REVEALED(board[coordinates[0]][coordinates[1]])){
        errorHandler(REVEAL_ALREADY_REVEALED_CELL_ERROR);
        return false;
    }
    else if(type == FLAG && board[coordinates[0]][coordinates[1]] == FLAGGED){
        errorHandler(FLAG_ALREADY_FLAGGED_CELL_ERROR);
        return false;
    }
    else if(type == FLAG && REVEALED(board[coordinates[0]][coordinates[1]])){
        errorHandler(FLAG_ALREADY_REVEALED_CELL_ERROR);
        return false;
    }
    return true;
}

// Checagem de qual comando foi chamado
int parseCommand(char *cmd){
    char *command = strtok(cmd, " ");
    if(strcmp(command, "start") == 0){
        return START;
    }
    else if(strcmp(command, "reveal") == 0){
        return REVEAL;
    }
    else if(strcmp(command, "flag") == 0){
        return FLAG;
    }
    else if(strcmp(command, "remove_flag") == 0){
        return REMOVE_FLAG;
    }
    else if(strcmp(command, "reset") == 0){
        return RESET;
    }
    else if(strcmp(command, "exit") == 0){
        return EXIT;
    }
    return ERROR;
}