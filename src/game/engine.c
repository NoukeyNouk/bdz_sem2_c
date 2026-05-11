#include <stdio.h>
#include "table.h"

#include "status.h"
#include "game.h"

status_t board_init(Board *board);
status_t board_insert(Board *board, size_t key1, size_t key2, GameCell cell);
status_t board_get(Board *board, size_t key1, size_t key2, GameCell cell);
status_t board_delete(Board *board, size_t key1, size_t key2, GameCell cell);

status_t game_loop(size_t x, size_t y, size_t mines) {
    status_t status = SUCCESS;
    Board *board = calloc(1, sizeof(Board));
    if (!board) {
        status = ERR_MEM;
        goto cleanup;
    }

    board->x = x;
    board->y = y;
    board->mines = mines;
    board->data = T_create();
    if (!board->data) {
        status = ERR_MEM;
        goto cleanup;
    }

    status = board_init(board);
    if (status == ERR_MEM) {
        goto cleanup;
    }

cleanup:
    if (!board) {
        return status;
    }
    T_free(board->data);
    free(board);
    return status;
}


status_t board_init(Board *board) {
    if (board->x > 65535 && board->y > 65535) {
        board->x = 65535;
        board->y = 65535;
    }

    size_t counter = 0;
    while (counter < board->mines) {
        size_t x = rand() % board->x;
        size_t y = rand() % board->y;
        GameCell *cell = board_get(board, x, y);
    }

}

status_t board_insert(Board *board, size_t key1, size_t key2, GameCell cell) {
    if (key1 >= 65535 || key2 >= 65535) {
        printf("ERROR: x or y >= 65535\n");
        return ERR_VALUE;
    }
    int key = (key1 << 16) + key2;
    return T_insert(board->data, key, cell);
}

status_t board_get(Board *board, size_t key1, size_t key2) {
    if (key1 >= 65535 || key2 >= 65535) {
        printf("ERROR: x or y >= 65535\n");
        return ERR_VALUE;
    }
    int key = (key1 << 16) + key2;
    return T_get(board->data, key);
}

status_t board_delete(Board *board, size_t key1, size_t key2) {
    if (key1 >= 65535 || key2 >= 65535) {
        printf("ERROR: x or y >= 65535\n");
        return ERR_VALUE;
    }
    int key = (key1 << 16) + key2;
    return T_delete(board->data, key);
}
