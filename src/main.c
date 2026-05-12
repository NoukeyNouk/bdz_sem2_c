#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "table.h"


int main() {
    srand(time(NULL));
    game_loop(50, 50, 250);
}
