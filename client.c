#include "common.h"

// Envia mensagem de fechar conexão para o servidor
int sendExit(char *msg, int s) {
    int msglen = send(s, msg, strlen(msg) + 1, 0);
    if (msglen != strlen(msg) + 1) {
        logexit("send");
        return -1;
    }
    return 1;
}

// Checagem de qual comando foi chamado
int parseCommand(char *cmd){
    char *command = strtok(cmd, " ");
    if (strcmp(command, "select") == 0){
        return 1;
    }
    else if (strcmp(command, "send") == 0){
        return 2;
    }
    else if (strcmp(command, "exit") == 0){
        return 3;
    }
    return -1;
}

int main(int argc, char **argv) {
    if (argc < 3){
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if( 0 != addrparse(argv[1], argv[2], &storage) ){
        usage(argc, argv);
    }

    // Inicializar Socket
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if( s == -1 ){
        logexit("socket");
    }

    // Inicializar Conexao
    struct sockaddr *addr = (struct sockaddr *)(&storage); 
    if( 0 != connect(s, addr, sizeof(storage)) ){
        logexit("connect");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);

    printf("connected to %s \n", addrstr);

    char msg[BUFSZ];
    char cmd[BUFSZ] = "";
    char full[BUFSZ] = "";
    char nome_arquivo[BUFSZ];
    char res[BUFSZ];

    int file_selected = 0;
    int waiting_res = 0;

    while(1) {
        fflush(stdin);
        memset(msg, 0, BUFSZ);
        memset(cmd, 0, BUFSZ);
        memset(full, 0, BUFSZ);
        memset(res, 0, BUFSZ);

        //Leitura das mensagens de comando
        scanf("%[^\n]%*c", msg);
        strcpy(cmd, msg);
        strcpy(full, msg);

        // Quebra da mensagem para analisar elementos
        int cmdType = parseCommand(cmd);
        char *command = strtok(full, "\n");
        char *arquivo[3];
        char *tipo = strtok(command, " ");
        arquivo[0] = tipo;

        // Chamada de ação específica para cada tipo de comando enviado
        switch(cmdType) {
            case 1:
                switch(selectFile(msg, arquivo, nome_arquivo)) {
                    case 1:
                        file_selected = 1;
                        break;
                    case 0:
                        break;
                    case -1:
                        sendExit("exit", s);
                        waiting_res = 1;
                        break;
                }
                break;
            case 2:
                // Permite enviar apenas se há algum arquivo selecionado
                if(file_selected != 1) {
                    printf("no file selected!\n");
                } 
                else {
                    switch(sendFile(msg, s, nome_arquivo)) {
                        case 1:
                            waiting_res = 1;
                            break;
                        case -1:
                            sendExit("exit", s);
                            waiting_res = 1;
                            break;
                    }
                }
                break;
            case 3:
                sendExit("exit", s);
                waiting_res = 1;
                break;
            default:
                sendExit("exit", s);
                waiting_res = 1;
        }
        if(waiting_res == 1) {
            recv(s, res, BUFSZ, 0);
            if(strcmp(res, "Connection closed.") == 0) {
                printf("%s\n", res);
                break;
            } else {
                printf("%s\n", res);
            }
            waiting_res = 0;
        }
    }
    close(s);
    exit(EXIT_SUCCESS);
}