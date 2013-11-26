#include <stdlib.h>
#include <stdio.h>

#include "challenge.h"

challenge* import_challenge(FILE* f)
{
    int n, k, i, j, v;
    char e;
    challenge* C = malloc(sizeof(challenge));

    fscanf(f, "%i\n", &n);
    C->n_streets = n;
    C->street = malloc(n * sizeof(int*));
    C->street_len = malloc(n * sizeof(int));
    C->variance = malloc(n * sizeof(int));
    C->openended = malloc(n * sizeof(char));

    C->max_len = 0;

    for (i = 0; i < n; i++)
    {
        fscanf(f, "%i|", &k);
        C->street_len[i] = k;
        if (k > C->max_len)
            C->max_len = k;

        C->street[i] = malloc(k * sizeof(int));
        for (j = 0; j < k; j++)
        {
            fscanf(f, "%i", &v);
            C->street[i][j] = v;
            if (j < k - 1)
                fscanf(f, ",");
        }

        fscanf(f, "%c\n", &e);
        if (e == ';')
            C->openended[i] = 1;
        else
        {
            C->openended[i] = 0;
            if (e != '.')
                printf("<< Unknown openended char '%c'", e);
        }
    }

    C->n_poss = 0;
    C->at = NULL;

    return C;
}

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

int calc_poss(challenge *C, int boardsize)
{
    int k, i, j, e, x, m;

    C->n_poss = 1;

    for (i = 0; i < C->n_streets; i++)
    {
        k = C->street_len[i];

        if (C->openended[i] == 0)
            C->variance[i] = k * (k - 1) / 2;
        else
        {
            C->variance[i] = k * (k - 1) * (k - 2) * (k - 3) / 24;
        }

        C->n_poss *= C->variance[i];
    }

    if (C->n_poss <= 0)
        return 0;

    printf("Calculating possibilities...\n");

    C->at = malloc(C->n_poss * sizeof(int*));

    for (i = 0; i < C->n_poss; i++)
    {
        C->at[i] = malloc(boardsize * sizeof(int));
        for (j = 0; j < boardsize; j++)
        {
            C->at[i][j] = 0;
        }
    }

    k = C->max_len;
    m = twofo(k);
    int E[m][k];
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < k; j++)
        {
            E[i][j] = 0;
        }
    }
    for (x = 1; x <= k; x++)
    {
        for (i = twofo(x); i < twofo(x+1); i++)
        {
            E[i][x] = 2;
        }
        for (i = 0; i < x; i++)
        {
            E[twofo(x) + i][i] = 1;
        }
        if (twofo(x+1) >= m)
            break;
    }

    m = twefo(k);
    int F[m][k];
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < k; j++)
        {
            F[i][j] = 0;
        }
    }

    for (x = 1; x <= k; x++)
    {
        for (i = twefo(x); i < twefo(x+1); i++)
        {
            F[i][x+2] = 1;
        }

        for (i = 1; i < k; i++)
        {
            for (j = trifo(i); j < trifo(i+1); j++)
            {
                F[twefo(x) + j][i+1] = 2;
            }

            for (j = 1; j < k; j++)
            {
                for (e = twofo(j); e < twofo(j + 1); e++)
                {
                    F[twefo(x) + trifo(i) + e][j] = 2;
                }
                for (e = 0; e < twofo(j); e++)
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

    for (x = 0; x < C->n_streets; x++)
    {
        for (i = 0; i < C->n_poss; i++)
        {
            k = (i / m) % C->variance[x];
            for (j = 0; j < C->street_len[x]; j++)
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

void free_challenge(challenge* C)
{
    int i;
    for (i  = 0; i < C->n_streets; i++)
    {
        free(C->street[i]);
    }
    free(C->street);
    free(C->street_len);
    free(C->variance);
    free(C->openended);

    for (i = 0; i < C->n_poss; i++)
    {
        free(C->at[i]);
    }
    free(C->at);
    free(C);
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
