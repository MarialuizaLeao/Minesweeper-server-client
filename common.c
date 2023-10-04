#include "common.h"

struct action initAction(int type, int coordinates[2], int board[4][4]){
    struct action action;
    action.type = type;
    action.coordinates[0] = coordinates[0];
    action.coordinates[1] = coordinates[1];
    for(int i = 0; i < MAX; i++){
        for(int j = 0; j < MAX; j++){
            action.board[i][j] = board[i][j];
        }
    }
    return action;
}

void errorHandler(char *error){
    printf("%s\n", error);
}

void logexit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage){
    if(addrstr == NULL || portstr == NULL) return -1;
    uint16_t port = (uint16_t) atoi(portstr);
    if(port == 0) return -1;
    port = htons(port); //host to network (litle endian)
    struct in_addr inaddr4; //32 bit IP Address
    if(inet_pton(AF_INET, addrstr, &inaddr4)){
        struct sockaddr_in *addr4 = (struct sockaddr_in *) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }
    struct in6_addr inaddr6; //128 bit IP Address
    if(inet_pton(AF_INET6, addrstr, &inaddr6)){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6 -> sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }
    return -1;
}

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize){
    int version;
    char addrstr[INET6_ADDRSTRLEN+1] = "";
    uint16_t port;
    if(addr->sa_family == AF_INET){
        version = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *) addr;
        if(!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr, INET6_ADDRSTRLEN + 1)) logexit("ntop");
        port = ntohs(addr4->sin_port); //Network to host short
    }
    else if(addr->sa_family == AF_INET6){
        version = 6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) addr;
        if(!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr, INET6_ADDRSTRLEN + 1)) logexit("ntop");
        port = ntohs(addr6->sin6_port); //Network to host short
    }
    else logexit("unknown protocol family");
    if(str) snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port);
}

int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage){
    uint16_t port = (uint16_t) atoi(portstr);
    if(port == 0) return -1;
    port = htons(port);
    memset(storage, 0, sizeof(*storage));
    if(0 == strcmp(proto, "v4")){
        struct sockaddr_in *addr4 = (struct sockaddr_in *) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY; //Qualquer add disponivel no computador
        addr4->sin_port = port;
        return 0;
    }
    else if(0 == strcmp(proto, "v6")){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr = in6addr_any; //Qualquer add disponivel no computador v6
        addr6->sin6_port = port;
        return 0;
    }
    else return -1;
}

void printBoard(int board[MAX][MAX]){
    for(int i=0; i < MAX; i++){
        for(int j=0; j < MAX; j++){
            if(board[i][j] == EMPTY) printf("0\t\t");
            else if (board[i][j] == BOMB) printf("*\t\t");
            else if(board[i][j] == FLAGGED) printf(">\t\t");
            else if(board[i][j] == HIDDEN) printf("-\t\t");
            else printf("%d\t\t", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}