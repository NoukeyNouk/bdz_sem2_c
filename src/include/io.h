#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <stdio.h>

#define GET_INT 1
#define GET_SIZE 2

void safe_scanf(void *, int);
void get_int(int *n);
void get_size(size_t *n);
char *get_line(char *prompt);
char *fget_line(FILE *file);
char **fget_lines(FILE *file, size_t *len);

int safe_fread(void *ptr, size_t size, size_t count, FILE *file);
int safe_fwrite(void *ptr, size_t size, size_t count, FILE *file);

#endif
