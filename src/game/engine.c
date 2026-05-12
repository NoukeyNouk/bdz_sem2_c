#include <stdio.h>
#include "table.h"

#include "status.h"
#include "game.h"

status_t board_init(Board *board);
status_t board_insert(Board *board, size_t key1, size_t key2, GameCell cell);
GameCell *board_get(Board *board, size_t key1, size_t key2);
status_t board_delete(Board *board, size_t key1, size_t key2);
void board_print(Board *board);
int board_count_mines(Board *board, size_t x, size_t y);
int open_near_cells(Board *board, size_t x, size_t y);

status_t game_loop(size_t x, size_t y, size_t mines) {
    status_t status = SUCCESS;
    GameCell *cell = NULL;
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
    if (status == ERR_MEM || status == ERR_VALUE) {
        goto cleanup;
    }

    int lose = 0;
    size_t opened_cells = 0;
    while (!lose && opened_cells < board->x * board->y - board->mines) {
        board_print(board);
        printf("opened: %d\n", opened_cells);
        scanf("%llu", &x);
        scanf("%llu", &y);
        
        char command[10];
        scanf("%5s", command);

        cell = board_get(board, x, y);
        if (!cell) {
            int mines_cnt = board_count_mines(board, x, y);
            board_insert(board, x, y, (GameCell){
                .is_mine = 0,
                .mines_around = mines_cnt, // TODO
                .state = HIDDEN});
            cell = board_get(board, x, y);
        }

        switch (command[0]) {
            case 'o':
                if (cell->state == HIDDEN) {
                    cell->state = OPENED;
                    if (cell->is_mine) {
                        lose = 1;
                    }
                    else {
                        opened_cells++;
                        if (cell->mines_around == 0) {
                            opened_cells += open_near_cells(board, x, y);
                        }
                    }
                }
                break;
            case 'f':
                if (cell->state == HIDDEN) {
                    cell->state = FLAGGED;
                }
                break;
            case 'u':
                if (cell->state == FLAGGED) {
                    cell->state = HIDDEN;
                }
                break;
        }
    }
    if (lose) {
        printf("U LOOOSE!!\n\n");
    }
    else {
        printf("U WIN!!!!!!\n\n");
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
        if (!cell) {
            board_insert(board, x, y, (GameCell){
                .is_mine = 1,
                .mines_around = 0,
                .state = HIDDEN});

            cell = board_get(board, x, y);
            if (!cell) {
                return ERR_MEM;
            }
            counter++;
        }
    }

    return SUCCESS;
}

status_t board_insert(Board *board, size_t key1, size_t key2, GameCell cell) {
    if (key1 >= 65535 || key2 >= 65535) {
        printf("ERROR: x or y >= 65535\n");
        return ERR_VALUE;
    }
    int key = (key1 << 16) + key2;
    return T_insert(board->data, key, cell);
}

GameCell *board_get(Board *board, size_t key1, size_t key2) {
    if (key1 >= 65535 || key2 >= 65535) {
        printf("ERROR: x or y >= 65535\n");
        return NULL;
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

void board_print(Board *board) {
    for (size_t i = 0; i < board->x; ++i) {
        for (size_t j = 0; j < board->y; ++j) {
            GameCell *cell = board_get(board, i, j);
            if (!cell || cell->state == HIDDEN) {
                printf(".");
            }
            else if (cell->state == OPENED && !cell->is_mine) {
                printf("%d", cell->mines_around);
            }
            else if (cell->state == FLAGGED) {
                printf("F");
            }
            else {
                printf("B");
            }
        }
        printf("\n");
    }
    printf("\n");

    // for (size_t i = 0; i < board->x; ++i) {
    //     for (size_t j = 0; j < board->y; ++j) {
    //         GameCell *cell = board_get(board, i, j);
    //         if (!cell || !cell->is_mine) {
    //             printf(".");
    //         }
    //         else {
    //             printf("B");
    //         }
    //     }
    //     printf("\n");
    // }
    // printf("\n");
}

int board_count_mines(Board *board, size_t x, size_t y) {
    int cnt = 0;
    GameCell *cell = NULL;
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
            if (i == 0 && j == 0) continue;
            if (i == -1 && x == 0) continue;
            if (i == 1 && x == board->x - 1) continue;
            if (j == -1 && y == 0) continue;
            if (j == 1 && y == board->y - 1) continue;
            cell = board_get(board, x + i, y + j);
            if (cell && cell->is_mine) {
                cnt++;
            }
        }
    }
    return cnt;
}

int open_near_cells(Board *board, size_t x, size_t y) {
    int opened_cells = 0;
    unsigned int (*queue)[2] = calloc(board->x * board->y, sizeof(*queue));
    GameCell *cell = NULL;
    size_t tail = 0, head = 0;
    queue[tail][0] = x;
    queue[tail++][1] = y;

    while (tail != head) {
        x = queue[head][0];
        y = queue[head++][1];
        for (int i = -1; i < 2; ++i) {
            for (int j = -1; j < 2; ++j) {
                if (i == 0 && j == 0) continue;

                if (i == -1 && x == 0) continue;
                if (i == 1 && x == board->x - 1) continue;

                if (j == -1 && y == 0) continue;
                if (j == 1 && y == board->y - 1) continue;

                cell = board_get(board, x + i, y + j);
                if (!cell) {
                    int mines_cnt = board_count_mines(board, x + i, y + j);
                    board_insert(board, x + i, y + j, (GameCell){
                        .is_mine = 0,
                        .mines_around = mines_cnt,
                        .state = HIDDEN});
                    cell = board_get(board, x + i, y + j);
                }

                if (cell->state == HIDDEN) {
                    cell->state = OPENED;
                    opened_cells++;
                    if (cell->mines_around == 0) {
                        queue[tail][0] = x + i;
                        queue[tail++][1] = y + j;
                    }
                }

            }
        }

    }
    free(queue);
    return opened_cells;
}
