#include <stdlib.h>
#include <stdio.h>

#include "piece.h"

int import_pieces(piece*** P, FILE* f, int* maxnum)
{
    int k, i, n, j, x, y, a;
    fscanf(f, "%i\n", &k);
    *P = malloc(k * sizeof(piece*));
    for (i = 0; i < k; i++)
    {
        fscanf(f, "%i|", &n);
        (*P)[i] = malloc(sizeof(piece));
        (*P)[i]->id = 'A'+i;
        (*P)[i]->num = n;
        if (n > *maxnum)
            *maxnum = n;
        (*P)[i]->pos = malloc(n * sizeof(char));
        (*P)[i]->dx = malloc(n * sizeof(char));
        (*P)[i]->dy = malloc(n * sizeof(char));
        (*P)[i]->at = malloc(n * sizeof(char));

        for (j = 0; j < n; j++)
        {
            fscanf(f, "%i.%i:%i", &y, &x, &a);
            (*P)[i]->dx[j] = (char) x;
            (*P)[i]->dy[j] = (char) y;
            (*P)[i]->pos[j] = 0; // pas geinit bij turn
            (*P)[i]->at[j] = (char) a;
            if (j < n-1)
                fscanf(f, ",");
        }
        fscanf(f, "\n");

        (*P)[i]->dx[0] = 0;
        (*P)[i]->dy[0] = 0;

    }
    return k;
}

void free_pieces(piece **P, int k)
{
    int i;
    for (i = 0; i < k; i++)
    {
        free(P[i]->pos);
        free(P[i]->dx);
        free(P[i]->dy);
        free(P[i]->at);
        free(P[i]);
    }
    free(P);
}

void turn(piece *Q, piece *P, int ori, int w)
{
    //piece* newp = malloc(sizeof(piece));
    Q->id = P->id;
    Q->num = P->num;
    Q->at = P->at;

    int i;
    if (ori == 0)
    {
        for (i = 0; i < Q->num; i++)
        {
            Q->dx[i] = P->dx[i];
            Q->dy[i] = P->dy[i];
            Q->pos[i] = Q->dy[i] * w + Q->dx[i];
        }
    }
    if (ori == 1)
    {
        for (i = 0; i < Q->num; i++)
        {
            Q->dx[i] = P->dy[i];
            Q->dy[i] = - P->dx[i];
            Q->pos[i] = Q->dy[i] * w + Q->dx[i];
            //Q->pos[i] = -(P->pos[i] % w) * w + (P->pos[i] / w);
        }
    }
    if (ori == 2)
    {
        for (i = 0; i < Q->num; i++)
        {
            Q->dx[i] = - P->dx[i];
            Q->dy[i] = - P->dy[i];
            Q->pos[i] = Q->dy[i] * w + Q->dx[i];
            //Q->pos[i] = -(P->pos[i]);
        }
    }
    if (ori == 3)
    {
        for (i = 0; i < Q->num; i++)
        {
            Q->dx[i] = - P->dy[i];
            Q->dy[i] = P->dx[i];
            Q->pos[i] = Q->dy[i] * w + Q->dx[i];
            //Q->pos[i] = (P->pos[i] % w) * w - (P->pos[i] / w);
        }
    }
}
