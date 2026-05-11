#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include "table.h"

typedef enum CellState {
    OPENED = 1,
    HIDDEN = 2,
    FLAGGED = 3,
} CellState;

typedef struct GameCell {
    char is_mine;
    char mines_around;
    CellState state;
} GameCell;

typedef struct Board {
    size_t x;
    size_t y;
    Table *data;
    size_t mines;
} Board;

status_t game_loop(size_t x, size_t y, size_t mines);

#endif
