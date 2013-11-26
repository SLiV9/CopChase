#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "board.h"

void import_board(board* B, FILE* f)
{
    if (!B) return;

    // Set the dimensions.
    int w, h;
    fscanf(f, "%i\n", &w);
    B->width = w;
    fscanf(f, "%i\n", &h);
    B->height = h;
    B->size = w * h;

    // Make every space not blocked by default.
    B->blocked = malloc(B->width * B->height * sizeof(bool));
    for (int i = 0; i < B->width * B->height; i++)
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

        if (blockpos >= 0 && blockpos < B->width * B->height)
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
}

void destroy_board(board* B)
{
    free(B->blocked);
}
