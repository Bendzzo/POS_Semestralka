#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include "game.h"

#define PORT 55055
#define BUFFER_SIZE 256

typedef struct {
    int client_sockets[PLAYER_COUNT];
    Game game;
    pthread_mutex_t game_mutex;
    pthread_cond_t turn_cond;
    int turn;
} Server;

void start_server(Server *server);
void *handle_client(void *arg);

#endif

