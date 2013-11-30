#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "challenge.h"

challenge* import_challenge(FILE* f)
{
    challenge* C = malloc(sizeof(challenge));

    // Determine the number of streets.
    int n_streets;
    fscanf(f, "%i\n", &n_streets);
    C->n_streets = n_streets;
    C->street = malloc(n_streets * sizeof(int*));
    C->street_len = malloc(n_streets * sizeof(int));
    C->variance = malloc(n_streets * sizeof(int));
    C->openended = malloc(n_streets * sizeof(char));

    // Max_len is the maximum number of segments for a street.
    C->max_len = 0;

    // Read each of the streets.
    for (int i = 0; i < n_streets; i++)
    {
        // Determine the number of segments.
        int n_segs;
        fscanf(f, "%i|", &n_segs);
        C->street_len[i] = n_segs;
        if (n_segs > C->max_len)
        {
            C->max_len = n_segs;
        }
        C->street[i] = malloc(n_segs * sizeof(int));

        // Read each street segment.
        for (int j = 0; j < n_segs; j++)
        {
            int pos;
            fscanf(f, "%i", &pos);
            C->street[i][j] = pos;

            if (j < n_segs - 1)
            {
                fscanf(f, ",");
            }
        }

        // Read the ending char, which is either ';' or '.'
        char e;
        fscanf(f, "%c\n", &e);
        if (e == ';')
        {
            C->openended[i] = true;
        }
        else
        {
            C->openended[i] = false;

            if (e != '.')
            {
                printf("<< Unknown openended char '%c'", e);
            }
        }
    }

    // n_poss is calculated by a calc_poss() call.
    C->n_poss = 0;
    C->at = NULL;

    return C;
}

// The empty challenge has no streets. Any piece will match.
challenge* empty_challenge()
{
    challenge* C = malloc(sizeof(challenge));

    C->n_streets = 0;
    C->street = malloc(sizeof(int*));
    C->street_len = malloc(sizeof(int));
    C->variance = malloc(sizeof(int));
    C->openended = malloc(sizeof(char));

    C->max_len = 0;
    C->n_poss = 0;
    C->at = NULL;

    return C;
}

void free_challenge(challenge* C)
{
    for (int i  = 0; i < C->n_streets; i++)
    {
        free(C->street[i]);
    }
    free(C->street);

    for (int i = 0; i < C->n_poss; i++)
    {
        free(C->at[i]);
    }
    free(C->at);

    free(C->street_len);
    free(C->variance);
    free(C->openended);

    free(C);
}

unsigned int var(unsigned int k, unsigned int s);

int calc_poss(challenge *C, int boardsize)
{
    // If any of the streets have length less than 2, or if any of the
    // openended streets have length less than 4, this challenge is unsolvable.
    for (int i = 0; i < C->n_streets; i++)
    {
        int k = C->street_len[i];

        if (C->openended[i])
        {
            if (k < 4)
            {
                return 0;
            }

            C->variance[i] = var(k, 4);
        }
        else
        {
            if (k < 2)
            {
                return 0;
            }

            C->variance[i] = var(k, 2);
        }
    }

    // n_poss is the product of the variances of the
    C->n_poss = 1;
    for (int i = 0; i < C->n_streets; i++)
    {
        C->n_poss *= C->variance[i];
    }

    printf("Calculating possibilities...\n");

    // Prepare the at array with emptiness.
    C->at = malloc(C->n_poss * sizeof(int*));
    for (int i = 0; i < C->n_poss; i++)
    {
        C->at[i] = malloc(boardsize * sizeof(int));
        for (int j = 0; j < boardsize; j++)
        {
            C->at[i][j] = 0;
        }
    }

    int k = C->max_len;
    int m;

    m = var(k, 2);
    int E[m][k];
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < k; j++)
        {
            E[i][j] = 0;
        }
    }
    for (int x = 1; x <= k; x++)
    {
        int start = var(x, 2);
        int end = var(x+1, 2);


        for (int i = start; i < end; i++)
        {
            E[i][x] = 2;
        }

        for (int i = 0; i < x; i++)
        {
            E[start + i][i] = 1;
        }

        if (end >= m)
        {
            break;
        }
    }

    m = var(k, 4);
    int F[m][k];
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < k; j++)
        {
            F[i][j] = 0;
        }
    }

    for (int x = 1; x <= k; x++)
    {
        int start = var(x, 4);
        int end = var(x+1, 4);

        for (int i = start; i < end; i++)
        {
            F[i][x+2] = 1;
        }

        for (int i = 1; i < k; i++)
        {
            int ins = var(i, 3);
            int out = var(i+1, 3);

            for (int j = ins; j < out; j++)
            {
                F[start + j][i+1] = 2;
            }

            for (int j = 1; j < k; j++)
            {
                int go = var(j, 2);
                int stop = var(j+1, 2);

                for (int e = go; e < stop; e++)
                {
                    F[start + ins + e][j] = 2;
                }
                for (int e = 0; e < go; e++)
                {
                    F[start + ins + go + e][e] = 1;
                }

                if (start + ins + stop >= start + out)
                    break;
            }

            if (start + out >= end)
                break;
        }

        if (end >= m)
            break;
    }

    m = 1;

    for (int x = 0; x < C->n_streets; x++)
    {
        for (int i = 0; i < C->n_poss; i++)
        {
            int k = (i / m) % C->variance[x];
            for (int j = 0; j < C->street_len[x]; j++)
            {
                if (C->openended[x])
                {
                    C->at[i][C->street[x][j]] = F[k][j];
                }
                else
                {
                    C->at[i][C->street[x][j]] = E[k][j];
                }
            }
        }
        m *= C->variance[x];
    }

    return C->n_poss;
}

unsigned int var(unsigned int k, unsigned int s)
{
    if (k == 0 || s == 0)
    {
        return 0;
    }

    unsigned int r = k;
    for (unsigned int i = 1; i < s; i++)
    {
        k *= (k-i) / (i+1);
    }

    return r;
}
