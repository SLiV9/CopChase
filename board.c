#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "board.h"

board* import_board(FILE* f)
{
    board* B = malloc(sizeof(board));

    // Set the dimensions.
    int w, h;
    fscanf(f, "%i\n", &w);
    B->width = w;
    fscanf(f, "%i\n", &h);
    B->height = h;
    B->size = w * h;

    // Make every space not blocked by default.
    B->blocked = malloc(w * h * sizeof(bool));
    for (int i = 0; i < w * h; i++)
    {
        B->blocked[i] = false;
    }

    // Read which spaces should become blocked.
    int nblocked;
    fscanf(f, "%i|", &nblocked);
    for (int i = 0; i < nblocked; i++)
    {
        int blockpos;
        fscanf(f, "%i", &blockpos);

        if (blockpos >= 0 && blockpos < w * h)
        {
            B->blocked[blockpos] = true;
        }
        else
        {
            printf("Boardblocker out of bounds! blockpos = %i.\n", blockpos);
        }

        if (i < nblocked - 1)
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
