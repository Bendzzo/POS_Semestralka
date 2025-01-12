#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>
#include <stdbool.h>

#define PORT 55055
#define BUFFER_SIZE 256

typedef struct {
    int socket_fd;
    pthread_t input_thread;
    pthread_t display_thread;
} Client;

void start_client(Client *client, const char *server_ip);
void *handle_input(void *arg);
void *handle_display(void *arg);

#endif

