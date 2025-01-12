#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <signal.h>
#include "../include/socket.h"
#include "../include/client.h"
#include "../include/game.h"

void *handle_input(void *arg);
void *handle_display(void *arg);
pid_t server_pid = -1;  // PID servera pre možné ukonèenie

void start_client(Client *client, const char *server_ip) {
    client->socket_fd = connect_to_server(server_ip, PORT);
    if (client->socket_fd < 0) {
        perror("Failed to connect");
        if (server_pid > 0) {
            kill(server_pid, SIGTERM); // Ak sa nepodarí pripoji, vypneme server
        }
        exit(1);
    }

    pthread_create(&client->display_thread, NULL, handle_display, client);
    pthread_create(&client->input_thread, NULL, handle_input, client);

    pthread_join(client->display_thread, NULL);
    pthread_join(client->input_thread, NULL);
}

void menu() {
    printf("Choose how you want to start the game:\n");
    printf("1. Create server\n");
    printf("2. Connect to server\n");
    printf("Your choice: ");
}

int main() {
    int choice;

    menu();
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input. Exiting.\n");
        return 1;
    }
    getchar();  // Ošetrenie newline znaku

    if (choice == 1) {
        server_pid = fork(); 
        if (server_pid == 0) {
            // Spustíme server v subprocess-e
            execl("./bin/server", "./bin/server", NULL);
            perror("Failed to start server");
            exit(1);
        } else if (server_pid < 0) {
            perror("Failed to fork");
            exit(1);
        } else {
            printf("Server was created. Waiting for it to start...\n");
            sleep(1); 
        }
    } else if (choice != 2) {
        printf("Invalid choice. Exiting.\n");
        exit(0);
    } else {
    Client client;
    start_client(&client, "localhost");

    
    if (server_pid > 0) {
        printf("Shutting down server...\n");
        kill(server_pid, SIGTERM);
    }

    close(client.socket_fd);
  }

    
    
    return 0;
}

void *handle_input(void *arg) {
    Client *client = (Client *)arg;
    char move[10];

    while (1) {
        fgets(move, sizeof(move), stdin);
        if (strlen(move) < 2) {
            printf("Invalid input. Use format: 1A\n");
            continue;
        }

        int row = move[0] - '1';
        int col = move[1] - 'A';

        if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
            printf("Invalid move. Row must be 1-3 and column A-C.\n");
            continue;
        }

        char formatted_move[3];
        snprintf(formatted_move, sizeof(formatted_move), "%d%d", row, col);

        send(client->socket_fd, formatted_move, strlen(formatted_move), 0);
    }
}

void *handle_display(void *arg) {
    Client *client = (Client *)arg;
    Game game;
    char buffer[BUFFER_SIZE];

    while (1) {
        int bytes_received = recv(client->socket_fd, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf("\nServer disconnected.\n");
            if (server_pid > 0) {
                printf("Shutting down server...\n");
                kill(server_pid, SIGTERM);
            }
            exit(1);
        }

        if (strstr(buffer, "=== GAME OVER ===") != NULL || strstr(buffer, "Player ") != NULL) {
            printf("%s\n", buffer);
            if (server_pid > 0) {
                printf("Shutting down server...\n");
                kill(server_pid, SIGTERM);
            }
            exit(0);
        }

        if (bytes_received == sizeof(Game)) {
            memcpy(&game, buffer, sizeof(Game));
            printf("\n=== Game Board ===\n");
            print_board(&game);
          }
    }
}
