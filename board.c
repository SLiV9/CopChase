#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "board.h"

board* import_board(FILE* f)
{
    int w, h, n, i, v;
    board* B = malloc(sizeof(board));

    // Set the dimensions.
    fscanf(f, "%i\n", &w);
    B->width = w;
    fscanf(f, "%i\n", &h);
    B->height = h;
    B->size = w * h;

    // Make every space not blocked by default.
    B->blocked = malloc(w * h * sizeof(bool));
    for (i = 0; i < w * h; i++)
    {
        B->blocked[i] = false;
    }

    // Read which spaces should become blocked.
    fscanf(f, "%i|", &n);
    for (i = 0; i < n; i++)
    {
        fscanf(f, "%i", &v);

        if (v >= 0 && v < w * h)
        {
            B->blocked[v] = true;
        }
        else
        {
            printf("Boardblocker out of bounds! v = %i.\n", v);
        }

        if (i < n - 1)
        {
            fscanf(f, ",");
        }
    }

    return B;
}

void free_board(board* B)
{
    free(B->blocked);
    free(B);
}
