#pragma once

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX 4
#define BUFSZ 1024

#define START 0 // Cliente -> Servidor
#define REVEAL 1 // Cliente -> Servidor
#define FLAG 2 // Cliente -> Servidor
#define STATE 3 // Servidor -> Cliente
#define REMOVE_FLAG 4 // Cliente -> Servidor
#define RESET 5 // Cliente -> Servidor
#define WIN 6 // Servidor -> Cliente
#define EXIT 7 // Cliente -> Servidor 
#define GAME_OVER 8 // Servidor -> Cliente

struct action{
  int type;
  int coordinates[2];
  int board[4][4];
};

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);
int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);
int server_sockaddr_init(const char *protocol_version, const char *portstr, struct sockaddr_storage *storage);
void logexit(const char *msg);
void printBoard(struct action *action);