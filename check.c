#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "piece.h"
#include "challenge.h"
#include "check.h"

int fits(board* B, char* taken, piece* P, int spot)
{
    if (spot < 0 || spot >= B->size)
        return 0;
    if (taken[spot])
        return 0;
    int i, j;
    for (i = 1; i < P->num; i++)
    {
        if (spot + P->pos[i] < 0)
            return 0;
        if (spot + P->pos[i] >= B->size)
            return 0;
        if (taken[spot + P->pos[i]])
            return 0;
    }
    for (i = 0; i < P->num - 1; i++)
    {
        for (j = i + 1; j < P->num; j++)
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

void matches(char* M, challenge* C, piece* P, int spot)
{
    int i, j, ok;

    if (C->n_streets > 0)
    for (i = 0; i < C->n_poss; i++)
    {
        ok = M[i];
        for (j = 0; ok && j < P->num; j++)
        {
            if (C->at[i][spot + P->pos[j]] != P->at[j])
            {
                ok = 0;
            }
        }
        M[i] = ok;
    }
}

int matchex(char* M, int num)
{
    int i, count = 0;
    for (i = 0; i < num; i++)
    {
        if (M[i])
            count++;
    }
    return count;
}

void binarystring(int len, int m)
{
    char str[len+4];
    str[0]='>';
    str[1]='>';
    str[2]=' ';
    str[len+3]='\0';
    int i;
    for (i = 0; i < len; i++)
    {
        if (m % 2)
            str[len+2-i] = 'x';
        else
            str[len+2-i] = '_';
        m = m / 2;
    }
    printf("%s", str);
}
