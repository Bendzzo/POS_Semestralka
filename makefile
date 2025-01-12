CC = gcc
CFLAGS = -Wall -Wextra -pthread
SRC = src
INCLUDE = include
BIN = bin
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes

all: server client

server: $(SRC)/server.c $(SRC)/game.c $(SRC)/socket.c
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $(BIN)/server $(SRC)/server.c $(SRC)/game.c $(SRC)/socket.c

client: $(SRC)/client.c $(SRC)/game.c $(SRC)/socket.c
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $(BIN)/client $(SRC)/client.c $(SRC)/game.c $(SRC)/socket.c

clean:
	rm -rf $(BIN)/*

mm_server:
	$(VALGRIND) ./bin/server

mm_client:
	$(VALGRIND) ./bin/client


