#include "common.h"

int board[MAX][MAX];

void initArgs(int argc, char *argv[]){
    if(argc != 3){
        printf("Usage: ./client <ipVersion> <port>\n");
        exit(1);
    }
    else{
        ipVersion = argv[1];
        port = argv[2];
    }
}

bool validCell(struct action action){
    if(action.coordinates[0] < 0 || action.coordinates[0] >= MAX || action.coordinates[1] < 0 || action.coordinates[1] >= MAX){
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

int main(int argc, char **argv) {
    initArgs(argc, argv);
    //Reset board

    struct sockaddr_storage storage;
    if(0 != addrparse(ipVersion, port, &storage)){
        logexit("addrparse");
    }

    // Inicializar Socket
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s == -1){
        logexit("socket");
    }

    // Inicializar Conexao
    struct sockaddr *addr = (struct sockaddr *)(&storage); 
    if(0 != connect(s, addr, sizeof(storage))){
        logexit("connect");
    }

    char msg[BUFSZ];

    bool gameStared = false;

    while(1){
        fflush(stdin);
        memset(msg, 0, BUFSZ);

        //Leitura das mensagens de comando
        scanf("%s", msg);

        // Quebra da mensagem para analisar elementos
        int cmdType = parseCommand(msg);
        
        int coordnates[2];

        // Chamada de ação específica para cada tipo de comando enviado
        switch(cmdType) {
            case ERROR:
                printf("error: command not found\n");
                break;
            case START:
                gameStared = true;
                struct action *request = initAction(START, coordnates, board);
                int count = sendAction(request, s);
                if(count != sizeof(request)){
                    logexit("send");
                }
                struct action *response;
                count = recv(s, &response, sizeof(response), 0);
                break;
            case REVEAL:
                scanf("%d,%d", &coordnates[0], &coordnates[1]);
                struct action *request = initAction(REVEAL, coordnates, board);
                if(validCell(*request) && board[request->coordinates[0]][request->coordinates[1]] < 0){
                    int count = sendAction(request, s);
                    if(count != sizeof(request)){
                        logexit("send");
                    }
                    struct action *response;
                    count = recv(s, &response, sizeof(response), 0);
                }
                else{
                    printf("error: invalid cell\n");
                }
                break;
            case FLAG:
                scanf("%d,%d", &coordnates[0], &coordnates[1]);
                struct action *request = initAction(FLAG, coordnates, board);
                if(validCell(*request) && board[request->coordinates[0]][request->coordinates[1]] < 0 && board[request->coordinates[0]][request->coordinates[1]] != -2){
                    int count = sendAction(request, s);
                    if(count != sizeof(request)){
                        logexit("send");
                    }
                    struct action *response;
                    count = recv(s, &response, sizeof(response), 0);
                }
                else{
                    printf("error: invalid cell\n");
                }
                break;
            case REMOVE_FLAG:
                scanf("%d,%d", &coordnates[0], &coordnates[1]);
                struct action *request = initAction(REMOVE_FLAG, coordnates, board);
                if(validCell(*request)){
                    int count = sendAction(request, s);
                    if(count != sizeof(request)){
                        logexit("send");
                    }
                    struct action *response;
                    count = recv(s, &response, sizeof(response), 0);
                }
                else{
                    printf("error: invalid cell\n");
                }
                break;
            case RESET:
                struct action *request = initAction(RESET, coordnates, board);
                int count = sendAction(request, s);
                if(count != sizeof(request)){
                    logexit("send");
                }
                struct action *response;
                count = recv(s, &response, sizeof(response), 0);
                break;
            case EXIT:
                printf("exiting...\n");
                close(s);
                break;
            default:
                break;
        }
    exit(EXIT_SUCCESS);
}