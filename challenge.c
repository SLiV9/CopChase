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

int calc_poss(challenge *C, int boardsize)
{
    C->n_poss = 1;

    for (int i = 0; i < C->n_streets; i++)
    {
        int k = C->street_len[i];

        if (C->openended[i])
        {
            C->variance[i] = k * (k - 1) * (k - 2) * (k - 3) / 24;
        }
        else
        {
            C->variance[i] = k * (k - 1) / 2;
        }

        C->n_poss *= C->variance[i];
    }

    // If any of the streets have length less than 2, or if any of the
    // openended streets have length less than 4, this challenge is unsolvable.
    if (C->n_poss <= 0)
    {
        C->n_poss = 0;
        return 0;
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

    m = twofo(k);
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
        for (int i = twofo(x); i < twofo(x+1); i++)
        {
            E[i][x] = 2;
        }
        for (int i = 0; i < x; i++)
        {
            E[twofo(x) + i][i] = 1;
        }
        if (twofo(x+1) >= m)
            break;
    }

    m = twefo(k);
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
        for (int i = twefo(x); i < twefo(x+1); i++)
        {
            F[i][x+2] = 1;
        }

        for (int i = 1; i < k; i++)
        {
            for (int j = trifo(i); j < trifo(i+1); j++)
            {
                F[twefo(x) + j][i+1] = 2;
            }

            for (int j = 1; j < k; j++)
            {
                for (int e = twofo(j); e < twofo(j + 1); e++)
                {
                    F[twefo(x) + trifo(i) + e][j] = 2;
                }
                for (int e = 0; e < twofo(j); e++)
                {
                    F[twefo(x) + trifo(i) + twofo(j) + e][e] = 1;
                }

                if (twefo(x) + trifo(i) + twofo(j+1) >= twefo(x) + trifo(i+1))
                    break;
            }

            if (twefo(x) + trifo(i+1) >= twefo(x+1))
                break;
        }

        if (twefo(x+1) >= m)
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

int twofo(int k)
{
    return k * (k - 1) / 2;
}

int trifo(int k)
{
    return k * (k - 1) * (k - 2) / 6;
}

int twefo(int k)
{
    return k * (k - 1) * (k - 2) * (k - 3) / 24;
}
