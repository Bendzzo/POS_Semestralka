#include <stdio.h>
#include <string.h>
#include "../include/game.h"

void init_game(Game *game) {
    memset(game->board, '.', sizeof(game->board));
    game->current_player = 1;
}

int make_move(Game *game, int player, int row, int col) {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE || game->board[row][col] != '.')
        return 0;
    game->board[row][col] = (player == 1) ? 'X' : 'O';
    return 1;
}

int check_winner(Game *game) {
    // Skontroluj riadky a stĺpce
    for (int i = 0; i < SIZE; i++) {
        if (game->board[i][0] != '.' && game->board[i][0] == game->board[i][1] && game->board[i][1] == game->board[i][2])
            return game->board[i][0];  // Výhra v riadku
        if (game->board[0][i] != '.' && game->board[0][i] == game->board[1][i] && game->board[1][i] == game->board[2][i])
            return game->board[0][i];  // Výhra v stĺpci
    }

    // Skontroluj diagonály
    if (game->board[0][0] != '.' && game->board[0][0] == game->board[1][1] && game->board[1][1] == game->board[2][2]) {
        return game->board[0][0];  // Výhra na diagonále "\"
    }
    if (game->board[0][2] != '.' && game->board[0][2] == game->board[1][1] && game->board[1][1] == game->board[2][0]) {
        return game->board[0][2];  // Výhra na diagonále "/"
    }

    // Skontroluj remízu
    int full = 1;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (game->board[i][j] == '.') {
                full = 0;  // Ak je voľné pole, hra ešte neskončila
            }
        }
    }

    if (full) return 'T';  // Remíza

    return 0;  // Hra pokračuje
}




void print_board(Game *game) {
    printf("\n  A B C\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", i + 1);
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", game->board[i][j]);
        }
        printf("\n");
    }
}









