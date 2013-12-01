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
    C->openended = malloc(n_streets * sizeof(bool));

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
    if (C->n_streets == 0)
    {
        return 1;
    }

    // If any of the streets have length less than 2, or if any of the
    // openended streets have length less than 4, this challenge is unsolvable.
    // The number of possibilities to place CR on a street of length k, is the same
    // as the number of possibilities to place k-2 "nulls" on a street of length k,
    // which is (k over 2) or var(k, 2).
    // Similarly, the variance of double ended streets is (k over 4) or var(k, 4).
    for (int i = 0; i < C->n_streets; i++)
    {
        int k = C->street_len[i];

        if (C->openended[i])
        {
            C->variance[i] = var(k, 4);
        }
        else
        {
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

    // List all the possibilities of placing CR on single ended streets in E.
    // List all the possibilities of placing CRRC on double ended streets in F.
    // They are listed from shortest to longest, which means the first (l over 2)
    // items of E are the possib. of placing CR on streets of length l, and the
    // first (l over 4) items of F are the posssib. for double streets of length l.
    int streetlen = C->max_len;
    int nposs, poss;

    nposs = var(streetlen, 2);
    char E[nposs][streetlen];
    for (int i = 0; i < nposs; i++)
    {
        for (int j = 0; j < streetlen; j++)
        {
            E[i][j] = 0;
        }
    }

    nposs = var(streetlen, 4);
    char F[nposs][streetlen];
    for (int i = 0; i < nposs; i++)
    {
        for (int j = 0; j < streetlen; j++)
        {
            F[i][j] = 0;
        }
    }

    // Fill E.
    poss = 0;
    for (int lastR = 1; lastR < streetlen; lastR++)
    {
        for (int firstC = 0; firstC < lastR; firstC++)
        {
            // Place a cop, and then a robber.
            E[poss][firstC] = 1;
            E[poss][lastR] = 2;
            poss++;
        }
    }

    // Fill F.
    poss = 0;
    for (int lastC = 3; lastC < streetlen; lastC++)
    {
        for (int lastR = 2; lastR < lastC; lastR++)
        {
            for (int firstR = 1; firstR < lastR; firstR++)
            {
                for (int firstC = 0; firstC < firstR; firstC++)
                {
                    // Place a cop, then a robber, then another robber, then a cop.
                    F[poss][firstC] = 1;
                    F[poss][firstR] = 2;
                    F[poss][lastR] = 2;
                    F[poss][lastC] = 1;
                    poss++;
                }
            }
        }
    }

    // For each street and for each C possibility, fill at[][].
    int m = 1;
    for (int x = 0; x < C->n_streets; x++)
    {
        for (int i = 0; i < C->n_poss; i++)
        {
            // Since n_poss is the product of the variances, (i/m) is the x-th digit.
            // Here, poss is the choice in E or F, as above.
            int poss = (i / m) % C->variance[x];

            // For each possibility i, for each street x, fill the space
            // C->street[x][j] of segment j with the the character
            // (none/cop/robber) as perscribed by E[][j] and F[][j].
            for (int j = 0; j < C->street_len[x]; j++)
            {
                if (C->openended[x])
                {
                    C->at[i][C->street[x][j]] = F[poss][j];
                }
                else
                {
                    C->at[i][C->street[x][j]] = E[poss][j];
                }
            }
        }

        m *= C->variance[x];
    }

    return C->n_poss;
}

// Calculates k over s, or (k!)/((s!)(k-s)!).
unsigned int var(unsigned int k, unsigned int s)
{
    if (k < s || s == 0)
    {
        return 0;
    }

    // First multiply, then divide, since 2/3 = 0.
    unsigned int r = k;
    for (unsigned int i = 1; i < s; i++)
    {
        r *= k-i;
    }
    for (unsigned int i = 2; i <= s; i++)
    {
        r /= i;
    }

    return r;
}
