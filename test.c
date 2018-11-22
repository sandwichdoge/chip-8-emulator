#include <stdio.h>
#include <stdlib.h>

void main()
{
    unsigned short op[4];
    unsigned short base = 0xf;
    for (int i = 0; i < 4; i++) {
        printf("0x%x\n", base << i * 4);
    }
    /*
    int rows = 3, columns = 4;
    int **a = malloc(sizeof(int*) * rows);
    for (int sz = 0; sz < rows; ++sz) {
        a[sz] = malloc(columns * sizeof(int));
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            a[i][j] = 5;
        }
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            printf("%d\n", a[i][j]);
        }
    }

    //free 2d array
    for (int sz = 0; sz < rows; ++sz) {
        free(a[sz]);
    }
    free(a);*/
}