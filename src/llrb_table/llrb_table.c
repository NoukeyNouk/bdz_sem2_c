#ifdef TREE_IMPL


#include "game.h"
#include "status.h"

typedef enum color_t {
    RED = 1,
    BLACK = 0,
} color_t;

typedef struct Node {
    int key;
    GameCell cell;

    struct Node *right;
    struct Node *left;

    color_t color;
} Node;


typedef struct Table {
    Node *root;
    Node *elist;
} Table;


Node *rough_insert(Node *node, int key, GameCell cell, Node *elist);
void rough_free(Node *node, Node *elist);
Node *balance(Node *node);
Node *rotate_left(Node *current);
Node *rotate_right(Node *current);
void swap_colors(Node *current);


Table *T_create() {
    Table *table = calloc(1, sizeof(Table));
    if (!table) {
        return NULL;
    }

    table->elist = calloc(1, sizeof(Node));
    if (!table->elist) {
        free(table);
        return NULL;
    }

    table->elist->right = table->elist;
    table->elist->left = table->elist;
    table->elist->color = BLACK;
    table->root = table->elist;

    return table;
}

Node *create_node(int key, GameCell cell, color_t color, Node *elist) {
    Node *new_node = calloc(1, sizeof(Node));
    if (!new_node) {
        return NULL;
    }

    new_node->right = elist;
    new_node->left = elist;
    new_node->color = color;
    new_node->key = key;
    new_node->cell = cell;
    return new_node;
}


GameCell *T_get(Table *table, int key) {
    Node *current = table->root;
    while (current != table->elist) {
        if (current->key == key) {
            return &current->cell;
        }
        else if (current->key < key) {
            current = current->right;
        }
        else {
            current = current->left;
        }
    }
    return NULL;
}

GameCell *T_insert(Table *table, int key, GameCell cell) {
    Node *new_node = rough_insert(table->root, key, cell, table->elist);
    if (!new_node) {
        return NULL;
    }

    table->root = new_node;
    table->root->color = BLACK;

    return T_get(table, key);
}


void T_free(Table *table) {
    rough_free(table->root, table->elist);
    free(table->elist);
    free(table);
}

status_t T_delete(Table *table, int key) {
    //to be announced
    return SUCCESS;
}

void rough_free(Node *node, Node *elist) {
    if (node == elist) {
        return;
    }
    rough_free(node->left, elist);
    rough_free(node->right, elist);
    free(node);
}


Node *rough_insert(Node *node, int key, GameCell cell, Node *elist) {
    if (node == elist) {
        return create_node(key, cell, RED, elist);
    }
    Node *new_node = NULL;
    if (node->key == key) {
        return node;
    }
    else if (node->key > key) {
        new_node = rough_insert(node->left, key, cell, elist);
        if (!new_node) {
            return NULL;
        }
        node->left = new_node;
    }
    else {
        new_node = rough_insert(node->right, key, cell, elist);
        if (!new_node) {
            return NULL;
        }
        node->right = new_node;
    }

    return balance(node);
}


Node *balance(Node *node) {
    if (node->left->color == BLACK && node->right->color == RED) {
        node = rotate_left(node);
    }
    if (node->left->color == RED && node->left->left->color == RED) {
        node = rotate_right(node);
    }
    if (node->left->color == RED && node->right->color == RED) {
        swap_colors(node);
    }
    return node;
}

Node *rotate_left(Node *current) {
    Node *a = current->left;
    Node *b = current->right->left;
    Node *c = current->right->right;

    Node *new_current = current->right;

    current->right = b;
    new_current->left = current;
    new_current->color = current->color;
    current->color = RED;

    return new_current;
}

Node *rotate_right(Node *current) {
    Node *a = current->left->left;
    Node *b = current->left->right;
    Node *c = current->right;

    Node *new_current = current->left;

    current->left = b;
    new_current->right = current;
    new_current->color = current->color;
    current->color = RED;

    return new_current;
}

void swap_colors(Node *current) {
    current->left->color = 1 - current->left->color;
    current->right->color = 1 - current->right->color;
    current->color = 1 - current->color;
}


Node* move_red_left(Node* current) {
    swap_colors(current);

    if (current->right->left->color == RED) {
        current->right = rotate_right(current->right);
        current = rotate_left(current);
        swap_colors(current);
    }
    return current;
}

Node* move_red_right(Node* current) {
    swap_colors(current);

    if (current->left->left->color == RED) {
        current = rotate_right(current);
        swap_colors(current);
    }
    return current;
}

Node* find_min(Node* current, Node *elist) {
    while (current->left != elist) {
        current = current->left;
    }
    return current;
}

#endif
