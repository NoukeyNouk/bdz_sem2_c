#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "table.h"
#include "io.h"


int main() {
    srand(time(NULL));

    size_t x = 0, y = 0, mines = 0;
    printf("Width of board (min. 4): ");
    get_size(&x);
    if (x < 4) x = 4;

    printf("Height of board (min. 4): ");
    get_size(&y);
    if (y < 4) y = 4;

    printf("Count of mines (between 1..%zu): ", x * y - 9);
    get_size(&mines);
    if (mines < 1) mines = 1;
    if (mines > x * y - 9) mines = x * y - 9;

    game_loop(x, y, mines);
}
