#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"


void safe_scanf(void *var, int type) {
    char fmt[10];
    long long temp;
    if (type == GET_INT) {
        strcpy(fmt, "%d");
    }
    else if (type == GET_SIZE) {
        strcpy(fmt, "%zu");
    }
    int writed = 0;
    while (writed < 1) {
        temp = 0;
        writed = scanf(fmt, &temp);

        if (writed == -1) {
            temp = 0;
            break;
        }

        if (type == GET_SIZE && temp < 0) {
            writed = 0;
        }
        if (writed == 0) {
            while (getchar() != '\n');
            printf("please try again.\n");
        }
    }

    char chr = getchar();
    while (chr != '\n' && chr != EOF) {
        chr = getchar();
    }

    if (type == GET_SIZE) {
        *(size_t *)var = *(size_t *)&temp;
    }
    else {
        *(int *)var = *(int *)&temp;
    }
}


void get_int(int *n) {
    safe_scanf(n, GET_INT);
}

void get_size(size_t *n) {
    safe_scanf(n, GET_SIZE);
}

char *get_line(char *prompt) {
    if (prompt != NULL) {
        printf("%s", prompt);
    }

    // char *line = NULL;
    
    char buffer[81] = {0};
    int len = 1;
    char *out_str = calloc(len, sizeof(char));
    if (!out_str) return NULL;

    int n = 1;

    while (n > 0) {
        n = scanf("%80[^\n]", buffer);
        if (n > 0) {
            len += strlen(buffer);

            char *ptr = realloc(out_str, len);
            if (!ptr) {
                free(out_str);
                return NULL;
            }
            out_str = ptr;
            out_str[len - 1] = 0;

            strcat(out_str, buffer);
        }
        if (n == 0) {
            scanf("%*c");
        }
        if (n < 0) {
            printf("\n");
            free(out_str);
            out_str = NULL;
        }
    }
    return out_str;
}

char *fget_line(FILE *file) {
    char buffer[81] = {0};
    int len = 1;
    char *out_str = calloc(len, sizeof(char));
    if (!out_str) return NULL;

    int n = 1;

    while (n > 0) {
        n = fscanf(file, "%80[^\n]", buffer);
        if (n > 0) {
            len += strlen(buffer);

            char *ptr = realloc(out_str, len);
            if (!ptr) {
                free(out_str);
                return NULL;
            }
            out_str = ptr;
            out_str[len - 1] = 0;

            strcat(out_str, buffer);
        }
        if (n == 0) {
            fscanf(file, "%*c");
        }
        if (n < 0) {
            printf("\n");
            free(out_str);
            out_str = NULL;
        }
    }
    return out_str;
}

char **fget_lines(FILE *file, size_t *len) {
    char **lines = malloc(4 * sizeof(char *));
    if (!lines) {
        return NULL;
    }
    size_t size = 4;
    size_t cur = 0;

    char *line = fget_line(file);
    while (line) {
        if (cur == size) {
            char **temp = realloc(lines, size * 2 * sizeof(char *));
            if (!temp) {
                goto cleanup;
            }
            lines = temp;
            size *= 2;
        }

        lines[cur++] = line;
        line = fget_line(file);
    }
    *len = cur;
    return lines;

cleanup:
    for (int i = 0; i < cur; ++i) {
        free(lines[i]);
    }
    free(lines);
    return NULL;
}


int safe_fread(void *ptr, size_t size, size_t count, FILE *file) {
    if (fread(ptr, size, count, file) != count) {
        printf("error while reading file.\n");
        return 0;
    }
    return 1;
}

int safe_fwrite(void *ptr, size_t size, size_t count, FILE *file) {
    if (fwrite(ptr, size, count, file) != count) {
        printf("error while writing file.\n");
        return 0;
    }
    return 1;
}

