#ifndef TABLE_H
#define TABLE_H

#include <stdlib.h>
#include <time.h>

#include "status.h"

#define SIMPLE_NUMBER64 2305843009213693951ULL
typedef unsigned long long ull;

typedef struct Table Table;
typedef struct GameCell GameCell;

Table *T_create();
status_t T_insert(Table *table, int key, GameCell cell);
GameCell *T_get(Table *table, int key);
status_t T_delete(Table *table, int key);
void T_print(Table *table);
void T_free(Table *table);

#endif

