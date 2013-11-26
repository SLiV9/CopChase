#include <stdlib.h>
#include <stdio.h>

#include "board.h"

board* import_board(FILE* f)
{
    int w, h, n, i, v;
    board* B = malloc(sizeof(board));

    fscanf(f, "%i\n", &w);
    B->width = w;
    fscanf(f, "%i\n", &h);
    B->height = h;
    B->size = w * h;

    B->open = malloc(w * h * sizeof(char));

    for (i = 0; i < w * h; i++)
    {
        B->open[i] = 0;
    }

    fscanf(f, "%i|", &n);
    for (i = 0; i < n; i++)
    {
        fscanf(f, "%i", &v);
        if (v >= 0 && v < w * h)
            B->open[v] = 1;
        else
            printf("Boardblocker out of bounds! v = %i.\n", v);
        if (i < n - 1)
            fscanf(f, ",");
    }

    return B;
}

void free_board(board* B)
{
    free(B->open);
    free(B);
}
