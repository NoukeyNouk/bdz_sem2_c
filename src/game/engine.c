#include <stdio.h>
#include "table.h"

#include "status.h"
#include "game.h"

// status_t board_init(Board *board);
status_t board_init(Board *board, size_t safe_x, size_t safe_y);
status_t board_insert(Board *board, size_t key1, size_t key2, GameCell cell);
GameCell *board_get(Board *board, size_t key1, size_t key2);
GameCell *board_place(Board *board, size_t x, size_t y);
status_t board_delete(Board *board, size_t key1, size_t key2);
void board_print(Board *board);
int board_count_mines(Board *board, size_t x, size_t y);
int board_count_flags(Board *board, size_t x, size_t y);
status_t open_near_cells(Board *board, size_t x, size_t y);
status_t board_string_handle(Board *board, const char *command);
status_t board_command_handle(Board *board, char command);

status_t game_loop(size_t x, size_t y, size_t mines) {
    status_t status = SUCCESS;
    GameCell *cell = NULL;
    Board *board = calloc(1, sizeof(Board));
    if (!board) {
        status = ERR_MEM;
        goto cleanup;
    }

    if (x > 65535 && y > 65535) {
        x = 65535;
        y = 65535;
    }

    board->x = x;
    board->y = y;
    board->cx = 0;
    board->cy = 0;
    board->mines = mines;
    board->opened_cells = 0;
    board->data = T_create();
    if (!board->data) {
        status = ERR_MEM;
        goto cleanup;
    }


    int lose = 0;
    while (!lose && board->opened_cells < board->x * board->y - board->mines) {
        board_print(board);
        // printf("opened: %llu\n", board->opened_cells);
        if (board->initialized) {
            printf("type (wasd for moving cursor, o - open, f - flag, u - unflag)\n");
        }
        else {
            printf("type (wasd for moving cursor, o - open)\n");
        }
        
        char command[100];
        int cur = 0;
        scanf("%99s", command);

        status = board_string_handle(board, command);
        if (status == END_GAME) {
            lose = 1;
        }
        else if (status == ERR_MEM) {
            printf("Out of memory\n");
            goto cleanup;
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


status_t board_init(Board *board, size_t safe_x, size_t safe_y) {
    size_t counter = 0;
    while (counter < board->mines) {
        size_t x = rand() % board->x;
        size_t y = rand() % board->y;
        if (abs((int)x - (int)safe_x) <= 1 && abs((int)y - (int)safe_y) <= 1) continue;
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

GameCell *board_place(Board *board, size_t x, size_t y) {
    GameCell *cell = board_get(board, x, y);
    if (!cell) {
        int mines_cnt = board_count_mines(board, x, y);
        board_insert(board, x, y, (GameCell){
            .is_mine = 0,
            .mines_around = mines_cnt, // TODO
            .state = HIDDEN});
        cell = board_get(board, x, y);
    }
    return cell;
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
    int is_red = 0;
    for (size_t i = 0; i < board->x; ++i) {
        for (size_t j = 0; j < board->y; ++j) {
            GameCell *cell = board_get(board, i, j);
            if (board->cx == i && board->cy == j) {
                printf("\x1b[31m");
                is_red = 1;
            }

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
            if (is_red) {
                is_red = 0;
                printf("\x1b[0m");
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

status_t board_string_handle(Board *board, const char *command) {
    size_t cur = 0;
    while (command[cur] != 0) {

        status_t status = board_command_handle(board, command[cur]);
        if (status == ERR_MEM || status == END_GAME) {
            return status;
        }
        cur++;
    }
    return SUCCESS;
}

status_t board_command_handle(Board *board, char command) {
    size_t x = board->cx;
    size_t y = board->cy;

    GameCell *cell = NULL;
    if (board->initialized) {
        cell = board_place(board, x, y);
        if (!cell) {
            return ERR_MEM;
        }
    }
    else {
        if (command != 'o' && command != 'w' &&
            command != 'a' && command != 's' &&
            command != 'd' && command != 'q') return SUCCESS;
    }
    status_t status = SUCCESS;

    switch (command) {
        case 'q':
            status = END_GAME; //!!!!
            break;
        case 'o':
            if (!board->initialized) {
                board->initialized = 1;
                status = board_init(board, board->cx, board->cy);
                cell = board_place(board, x, y);
                if (!cell) {
                    return ERR_MEM;
                }
            }
            if (cell->state == HIDDEN) {
                cell->state = OPENED;
                if (cell->is_mine) {
                    status = END_GAME;
                }
                else {
                    board->opened_cells++;
                    if (cell->mines_around == 0) {
                        status = open_near_cells(board, x, y);
                    }
                }
            }
            else if (cell->state == OPENED) {
                int flags_cnt = board_count_flags(board, x, y);
                if (flags_cnt == cell->mines_around) {
                    status = open_near_cells(board, x, y);
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
        case 'w':
            if (board->cx > 0) {
                board->cx--;
            }
            break;
        case 'a':
            if (board->cy > 0) {
                board->cy--;
            }
            break;
        case 's':
            if (board->cx + 1 < board->x) {
                board->cx++;
            }
            break;
        case 'd':
            if (board->cy + 1 < board->y) {
                board->cy++;
            }
            break;
    }
    return status;
}

int board_count_mines(Board *board, size_t x, size_t y) { // strong
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

int board_count_flags(Board *board, size_t x, size_t y) { // strong
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
            if (cell && cell->state == FLAGGED) {
                cnt++;
            }
        }
    }
    return cnt;
}

status_t open_near_cells(Board *board, size_t x, size_t y) {
    unsigned int (*queue)[2] = calloc(board->x * board->y, sizeof(*queue));
    if (!queue) {
        return ERR_MEM;
    }

    status_t status = SUCCESS;
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

                cell = board_place(board, x + i, y + j);
                if (!cell) {
                    status = ERR_MEM;
                    goto cleanup;
                }

                if (cell->state == HIDDEN) {
                    cell->state = OPENED;
                    board->opened_cells++;
                    if (cell->is_mine) {
                        status = END_GAME;
                        goto cleanup;
                    }
                    if (cell->mines_around == 0) {
                        queue[tail][0] = x + i;
                        queue[tail++][1] = y + j;
                    }
                }
            }
        }
    }

cleanup:
    free(queue);
    return status;
}
