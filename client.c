#include "common.h"

int board[MAX][MAX];

char *ipVersion = "";
char *port = "";

#define VALID_COORD(x,y) (x >= 0 && x < MAX && y >= 0 && y < MAX)

void initArgs(int argc, char *argv[]); // inicialize arguments
void copyBoard(int Updatedboard[MAX][MAX]); // copy board from server to local variable
bool validAction(struct action action); // check if action is valid
int parseCommand(char *cmd); // check which command was called

int main(int argc, char **argv){
    initArgs(argc, argv);

    // inicialize comunication
    struct sockaddr_storage storage;
    if(addrparse(ipVersion, port, &storage) != 0){
        logexit("addrparse");
    }

    // inicialize socket
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s == -1){
        logexit("socket");
    }

    // inicialize connection
    struct sockaddr *addr = (struct sockaddr *)(&storage); 
    if(connect(s, addr, sizeof(storage)) != 0){
        logexit("connect");
    }

    char input[BUFSZ]; // buffer for input

    while(true){

        scanf("%s", input);
        int cmdType = parseCommand(input);
        int coordnates[2];
        bool canSendRequest = true;
        struct action request;

        // Chamada de ação específica para cada tipo de comando enviado
        switch(cmdType){
            case START:
                request = initAction(START, coordnates, board);
                printf("game started\n");
                break;
            case REVEAL:
                scanf("%d,%d", &coordnates[0], &coordnates[1]);
                request = initAction(REVEAL, coordnates, board);
                if(!validAction(request)){
                    canSendRequest = false;
                }
                break;
            case FLAG:
                scanf("%d,%d", &coordnates[0], &coordnates[1]);
                request = initAction(FLAG, coordnates, board);
                if(!validAction(request)){
                    canSendRequest = false;
                }
                break;
            case REMOVE_FLAG:
                scanf("%d,%d", &coordnates[0], &coordnates[1]);
                request = initAction(REMOVE_FLAG, coordnates, board);
                if(!validAction(request)){
                    canSendRequest = false;
                }
                break;
            case RESET:
                request = initAction(RESET, coordnates, board);
                break;
            case EXIT:
                request = initAction(EXIT, coordnates, board);
                break;
            case ERROR:
                errorHandler(COMMAND_ERROR);
                canSendRequest = false;
                break;
        }
        if(canSendRequest){
            int count = send(s, &request, sizeof(request), 0);
            if(cmdType == EXIT){
                close(s);
                break;
            }
            if(count != sizeof(request)){
                logexit("send");
            }

            struct action response;
            count = recv(s, &response, sizeof(response), 0);
            
            copyBoard(response.board);

            if(response.type == WIN) {
                printf("YOU WIN!\n");
            } else if(response.type == GAME_OVER){
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