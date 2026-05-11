#include <stdlib.h>
#include <stdio.h>

#include "table.h"
#include "status.h"

typedef struct GameCell;

typedef struct Cell {
    int key;
    GameCell cell;
    int busy;
} Cell;

typedef struct Table {
    Cell *table1;
    Cell *table2;
    ull hash_pair1[2];
    ull hash_pair2[2];
    size_t size; // I think of 1 of tables (their size should be equal)
} Table;

ull create_rand_ull();
ull get_a();
ull get_b();
size_t hash(int number, ull *hp, size_t size);
size_t h1(int number, Table *table);
size_t h2(int number, Table *table);
status_t T_rehash(Table *table);


ull create_rand_ull() {
    ull num = 0;
    num = rand();
    num <<= 32;
    num += rand();
    return num;
}

ull get_a() {
    return create_rand_ull() % (SIMPLE_NUMBER64 - 1) + 1;
}

ull get_b() {
    return create_rand_ull() % SIMPLE_NUMBER64;
}


size_t hash(int number, ull *hp, size_t size) {
    return ((hp[0] * (unsigned)number + hp[1]) % SIMPLE_NUMBER64) % size;
}


size_t h1(int number, Table *table) {
    return hash(number, table->hash_pair1, table->size);
}

size_t h2(int number, Table *table) {
    return hash(number, table->hash_pair2, table->size);
}


status_t T_rehash(Table *table) {
    Cell *new_table1 = calloc(table->size * 2, sizeof(Cell));
    if (!new_table1) {
        return ERR_MEM;
    }

    Cell *new_table2 = calloc(table->size * 2, sizeof(Cell));
    if (!new_table2) {
        free(new_table1);
        return ERR_MEM;
    }
    table->hash_pair1[0] = get_a();
    table->hash_pair2[0] = get_a();

    table->hash_pair1[1] = get_b();
    table->hash_pair2[1] = get_b();

    size_t cycle = table->size;
    table->size *= 2;
    Cell *temp = table->table1;
    table->table1 = new_table1;
    new_table1 = temp;

    temp = table->table2;
    table->table2 = new_table2;
    new_table2 = temp;
    for (size_t i = 0; i < cycle; ++i) {
        if (new_table1[i].busy == 1) {
            T_insert(table, new_table1[i].key, new_table1[i].info);
        }
        if (new_table2[i].busy == 1) {
            T_insert(table, new_table2[i].key, new_table2[i].info);
        }
    }
    free(new_table1);
    free(new_table2);
    return SUCCESS;
}


Table *T_create() {
    Table *table = calloc(1, sizeof(Table));
    if (!table) {
        goto cleanup;
    }
    table->size = 8;

    table->table1 = calloc(table->size, sizeof(Cell));
    if (!table->table1) {
        goto cleanup;
    }

    table->table2 = calloc(table->size, sizeof(Cell));
    if (!table->table2) {
        goto cleanup;
    }
    table->hash_pair1[0] = get_a();
    table->hash_pair2[0] = get_a();

    table->hash_pair1[1] = get_b();
    table->hash_pair2[1] = get_b();

    return table;

cleanup:
    if (!table) {
        return NULL;
    }
    free(table->table1);
    free(table->table2);
    free(table);
    return NULL;
}

status_t T_insert(Table *table, int key, char *info) {
    size_t ind1 = h1(key, table);
    size_t ind2 = h2(key, table);
    Cell *cur1 = table->table1 + ind1;
    Cell *cur2 = table->table2 + ind2;

    //trying to find the key
    if (cur1->busy == 1 && cur1->key == key) {
        return ERR_VALUE;
    }
    if (cur2->busy == 1 && cur2->key == key) {
        return ERR_VALUE;
    }

    //inserting if there's some space
    if (cur1->busy == 0) {
        cur1->busy = 1;
        cur1->key = key;
        cur1->info = info;
        return SUCCESS;
    }
    if (cur2->busy == 0) {
        cur2->busy = 1;
        cur2->key = key;
        cur2->info = info;
        return SUCCESS;
    }

    // if there's no space summoning bird
    int cycle = 50;
    int i = 0;
    int current_key = key;
    char *current_info = info;
    int table_id = 1;
    while (i < cycle) {
        Cell *current = NULL;
        if (table_id == 1) {
            current = table->table1 + h1(current_key, table);
        }
        else {
            current = table->table2 + h2(current_key, table);
        }

        if (current->busy == 0) {
            current->busy = 1;
            current->key = current_key;
            current->info = current_info;
            return SUCCESS;
        }

        int temp_key = current->key;
        char *temp_info = current->info;
        current->key = current_key;
        current->info = current_info;
        current_key = temp_key;
        current_info = temp_info;

        table_id = 3 - table_id; // 1 >> 2 ; 2 >> 1
        i++;
    }
    status_t status = T_rehash(table);
    if (status == ERR_MEM) {
        free(current_info);
        return ERR_MEM;
    }
    return T_insert(table, current_key, current_info);
}

char *T_get(Table *table, int key) {
    Cell *cur1 = table->table1 + h1(key, table);
    Cell *cur2 = table->table2 + h2(key, table);

    if (cur1->busy == 1 && cur1->key == key) {
        return cur1->info;
    }
    if (cur2->busy == 1 && cur2->key == key) {
        return cur2->info;
    }
    return NULL;
}

status_t T_delete(Table *table, int key) {
    Cell *cur1 = table->table1 + h1(key, table);
    Cell *cur2 = table->table2 + h2(key, table);

    if (cur1->busy == 1 && cur1->key == key) {
        cur1->busy = 0;
        free(cur1->info);
        cur1->info = NULL;
        return SUCCESS;
    }
    if (cur2->busy == 1 && cur2->key == key) {
        cur2->busy = 0;
        free(cur2->info);
        cur2->info = NULL;
        return SUCCESS;

    }
    return ERR_NOT_FOUND;
}

void T_free(Table *table) {
    if (!table) return;
    if (table->table1 || table->table2) {
        for (size_t i = 0; i < table->size; ++i) {
            if (table->table1) {
                if (table->table1[i].busy == 1) {
                    free(table->table1[i].info);
                }
            }
            if (table->table2) {
                if (table->table2[i].busy == 1) {
                    free(table->table2[i].info);
                }
            }
        }
    }
    free(table->table1);
    free(table->table2);
    free(table);
}

void T_print(Table *table) { // strong
    if (!table) return;
    if (table->size == 0) {
        printf("\nTable is empty.\n\n");
        return;
    }
    printf("table1:\n");
    printf("\n|ID  |BUSY |KEY     |INFO\n");

    for (size_t i = 0; i < table->size; ++i) {
        printf("|%-4llu|%-5d|%-8u|%s\n", i, table->table1[i].busy, table->table1[i].key, table->table1[i].info);
    }
    printf("\n");

    printf("table2:\n");
    printf("\n|ID  |BUSY |KEY     |INFO\n");

    for (size_t i = 0; i < table->size; ++i) {
        printf("|%-4llu|%-5d|%-8u|%s\n", i, table->table2[i].busy, table->table2[i].key, table->table2[i].info);
    }
    printf("\n");

}

