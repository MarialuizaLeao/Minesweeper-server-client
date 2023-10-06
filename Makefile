CC = gcc
CFLAGS = -Wall -g -c -I .
BIN = bin
OBJ = obj
SRC = src

all: $(BIN)/server $(BIN)/client

$(BIN)/server: $(OBJ)/server.o $(OBJ)/common.o | $(BIN)
	$(CC) -Wall -g $(OBJ)/server.o $(OBJ)/common.o -o $(BIN)/server

$(BIN)/client: $(OBJ)/client.o $(OBJ)/common.o | $(BIN)
	$(CC) -Wall -g $(OBJ)/client.o $(OBJ)/common.o -o $(BIN)/client

$(OBJ)/server.o: server.c | $(OBJ)
	$(CC) server.c $(CFLAGS) -o $(OBJ)/server.o

$(OBJ)/client.o: client.c | $(OBJ)
	$(CC) client.c $(CFLAGS) -o $(OBJ)/client.o

$(OBJ)/common.o: common.c | $(OBJ)
	$(CC) common.c $(CFLAGS) -o $(OBJ)/common.o

$(OBJ): 
	mkdir $(OBJ)

$(BIN):
	mkdir $(BIN)

clean:
	rm -rf $(BIN) $(OBJ)