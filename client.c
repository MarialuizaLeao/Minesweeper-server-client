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
        struct action requestToServer;

        // Chamada de ação específica para cada tipo de comando enviado
        switch(cmdType){
            case START:
                requestToServer = initAction(START, coordnates, board);
                printf("game started\n");
                break;
            case REVEAL:
                scanf("%d,%d", &coordnates[0], &coordnates[1]);
                requestToServer = initAction(REVEAL, coordnates, board);
                if(!validAction(requestToServer)){
                    canSendRequestToServer = false;
                }
                break;
            case FLAG:
                scanf("%d,%d", &coordnates[0], &coordnates[1]);
                requestToServer = initAction(FLAG, coordnates, board);
                if(!validAction(requestToServer)){
                    canSendRequestToServer = false;
                }
                break;
            case REMOVE_FLAG:
                scanf("%d,%d", &coordnates[0], &coordnates[1]);
                requestToServer = initAction(REMOVE_FLAG, coordnates, board);
                if(!validAction(requestToServer)){
                    canSendRequestToServer = false;
                }
                break;
            case RESET:
                requestToServer = initAction(RESET, coordnates, board);
                break;
            case EXIT:
                requestToServer = initAction(EXIT, coordnates, board);
                break;
            case ERROR:
                errorHandler(COMMAND_ERROR);
                canSendRequestToServer = false;
                break;
        }
        if(canSendRequestToServer){
            int count = send(sockfd, &requestToServer, sizeof(requestToServer), 0);
            if(cmdType == EXIT){
                close(sockfd);
                break;
            }
            if(count != sizeof(requestToServer)){
                logexit("send");
            }

            struct action responseFromServer;
            count = recv(sockfd, &responseFromServer, sizeof(responseFromServer), 0);
            if(count != sizeof(responseFromServer)){
                    logexit("send");
            }
            
            copyBoard(responseFromServer.board);

            if(responseFromServer.type == WIN) {
                printf("YOU WIN!\n");
            } else if(responseFromServer.type == GAME_OVER){
                printf("GAME OVER!\n");
            }
            printBoard(board);  
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

bool validCell(struct action action){
    if(action.coordinates[0] < 0 || action.coordinates[0] >= MAX || action.coordinates[1] < 0 || action.coordinates[1] >= MAX){
        errorHandler(INVALID_CELL_ERROR);
        return false;
    }
    return true;
}

bool validAction(struct action action){
    if(!VALID_COORD(action.coordinates[0], action.coordinates[1])){
        errorHandler(INVALID_CELL_ERROR);
        return false;
    }
    else if(action.type == REVEAL && REVEALED(action.board[action.coordinates[0]][action.coordinates[1]])){
        errorHandler(REVEAL_ALREADY_REVEALED_CELL_ERROR);
        return false;
    }
    else if(action.type == FLAG && action.board[action.coordinates[0]][action.coordinates[1]] == FLAGGED){
        errorHandler(FLAG_ALREADY_FLAGGED_CELL_ERROR);
        return false;
    }
    else if(action.type == FLAG && REVEALED(action.board[action.coordinates[0]][action.coordinates[1]])){
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