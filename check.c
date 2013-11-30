#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "board.h"
#include "piece.h"
#include "challenge.h"
#include "check.h"

// Checks if the piece fits.
// If its out of bounds, if it overlaps another piece or if it overflows the edge,
// it does not fit. Otherwise, it fits.
int fits(board* B, bool* taken, piece* P, int spot)
{
    // Determine if the spot is in bounds and not taken.
    if (spot < 0 || spot >= B->size)
    {
        return 0;
    }
    if (taken[spot])
    {
        return 0;
    }

    // Determine if all the segments are in bounds and not taken.
    for (int i = 1; i < P->num; i++)
    {
        int spi = spot + P->pos[i];
        if (spi < 0 || spi >= B->size)
        {
            return 0;
        }
        if (taken[spi])
        {
            return 0;
        }
    }

    // Determine if any overflowing occurs:
    // If two pieces have the same dy, they should end up on the same board row.
    for (int i = 0; i < P->num - 1; i++)
    {
        for (int j = i + 1; j < P->num; j++)
        {
            if (P->dy[j] == P->dy[i])
            {
                if ((spot + P->pos[j]) / B->width != (spot + P->pos[i]) / B->width)
                {
                    return 0;
                }
            }
        }
    }

    return 1;
}

// Determines which possibilities of a challenge a piece matches.
void matches(bool* M, challenge* C, piece* P, int spot)
{
    // If the challenge is empty (i.e. has no streets),
    // a piece always matches. Empty challenges are used
    // to determine the number of permutations of pieces
    // on a board.
    if (C->n_streets == 0)
    {
        return;
    }

    for (int i = 0; i < C->n_poss; i++)
    {
        for (int j = 0; j < P->num; j++)
        {
            if (C->at[i][spot + P->pos[j]] != P->at[j])
            {
                M[i] = false;
                break;
            }
        }
    }
}

// Sums up all num values of bitstring M.
int matchex(bool* M, int num)
{
    int count = 0;
    for (int i = 0; i < num; i++)
    {
        if (M[i])
        {
            count++;
        }
    }
    return count;
}
