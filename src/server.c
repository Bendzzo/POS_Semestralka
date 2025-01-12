#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include "../include/socket.h"
#include "../include/server.h"
#include <ctype.h>

void *handle_client(void *arg);

void start_server(Server *server) {
    int server_socket = passive_socket_init(PORT);
    if (server_socket < 0) {
        //perror("Server initialization failed");
        exit(1);
    }

    printf("Waiting for players...\n");
    for (int i = 0; i < PLAYER_COUNT; i++) {
        server->client_sockets[i] = passive_socket_wait_for_client(server_socket);
        printf("Player %d connected.\n", i + 1);
    }

    pthread_mutex_init(&server->game_mutex, NULL);
    pthread_cond_init(&server->turn_cond, NULL);
    server->turn = 0;

    pthread_t threads[PLAYER_COUNT];
    int player_indices[PLAYER_COUNT];
    void *args[PLAYER_COUNT][2];

    for (int i = 0; i < PLAYER_COUNT; i++) {
        player_indices[i] = i;
        args[i][0] = server;        
        args[i][1] = &player_indices[i]; 
        pthread_create(&threads[i], NULL, handle_client, args[i]);
    }

    for (int i = 0; i < PLAYER_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&server->game_mutex);
    pthread_cond_destroy(&server->turn_cond);
    passive_socket_destroy(server_socket);
}

void *handle_client(void *arg) {
    Server *server = ((Server **)arg)[0];
    int player_id = *(int *)(((void **)arg)[1]);

    char buffer[10];

    send(server->client_sockets[player_id], &server->game, sizeof(Game), 0);

    while (1) {
        pthread_mutex_lock(&server->game_mutex);

        while (server->turn != player_id)
            pthread_cond_wait(&server->turn_cond, &server->game_mutex);

        recv(server->client_sockets[player_id], buffer, sizeof(buffer), 0);
        int row = buffer[0] - '0', col = buffer[1] - '0';

        if (make_move(&server->game, player_id + 1, row, col)) {
            int winner = check_winner(&server->game);

            // Poslanie mapy vsetkym hracom
            for (int i = 0; i < PLAYER_COUNT; i++) {
                send(server->client_sockets[i], &server->game, sizeof(Game), 0);
            }

            if (winner) {
    		char end_msg[BUFFER_SIZE];
    		if (winner == 'T') {
        	snprintf(end_msg, sizeof(end_msg), "=== GAME OVER ===\nIt's a tie!\n");
    		} else {
        	snprintf(end_msg, sizeof(end_msg), "=== GAME OVER ===\nPlayer %d (%c) wins!\n",
                (winner == 'X') ? 1 : 2, winner);
    	     }

    	     	for (int i = 0; i < PLAYER_COUNT; i++) {
        		send(server->client_sockets[i], end_msg, strlen(end_msg), 0);
    	      	}
    		exit(0);
	    }
            // Posun na dalsieho hraca
            server->turn = (server->turn + 1) % PLAYER_COUNT;
            pthread_cond_broadcast(&server->turn_cond);
        }

        pthread_mutex_unlock(&server->game_mutex);
    }
}

int main() {
    Server server;
    server.turn = 0;

    pthread_mutex_init(&server.game_mutex, NULL);
    pthread_cond_init(&server.turn_cond, NULL);

    init_game(&server.game);
    start_server(&server);

    pthread_mutex_destroy(&server.game_mutex);
    pthread_cond_destroy(&server.turn_cond);

    return 0;
}
