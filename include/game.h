#ifndef GAME_H
#define GAME_H

#define SIZE 3
#define PLAYER_COUNT 2

typedef struct {
    char board[SIZE][SIZE];
    int current_player;
} Game;

void init_game(Game *game);
int make_move(Game *game, int player, int row, int col);
int check_winner(Game *game);
void print_board(Game *game);

#endif

