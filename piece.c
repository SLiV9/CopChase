#include <stdlib.h>
#include <stdio.h>

#include "piece.h"

void import_pieces(pieceholder* H, FILE* f)
{
    if (!H) return;

    fscanf(f, "%i\n", &(H->n_pieces));
    H->P = malloc(H->n_pieces * sizeof(piece));
    H->max_n_segments = 0;

    for (int i = 0; i < H->n_pieces; i++)
    {
        int n_segments;
        fscanf(f, "%i|", &n_segments);
        piece p = H->P[i];
        p.id = 'A' + i;
        p.num = n_segments;
        if (n_segments > H->max_n_segments)
        {
            H->max_n_segments = n_segments;
        }
        p.pos = malloc(n_segments * sizeof(char));
        p.dx = malloc(n_segments * sizeof(char));
        p.dy = malloc(n_segments * sizeof(char));
        p.at = malloc(n_segments * sizeof(char));

        for (int j = 0; j < n_segments; j++)
        {
            int x, y, a;
            fscanf(f, "%i.%i:%i", &y, &x, &a);
            p.dx[j] = (char) x;
            p.dy[j] = (char) y;
            p.pos[j] = 0; // pas geinit bij turn
            p.at[j] = (char) a;
            if (j < n_segments-1)
            {
                fscanf(f, ",");
            }
        }
        fscanf(f, "\n");

        p.dx[0] = 0;
        p.dy[0] = 0;
    }
}

void destroy_pieces(pieceholder* H)
{
    for (int i = 0; i < H->n_pieces; i++)
    {
        free(H->P[i].pos);
        free(H->P[i].dx);
        free(H->P[i].dy);
        free(H->P[i].at);
    }
    free(H->P);
}

void turn(piece *Q, piece *P, int ori, int w)
{
    //piece* newp = malloc(sizeof(piece));
    Q->id = P->id;
    Q->num = P->num;

    for (int i = 0; i < Q->num; i++)
    {
        Q->at[i] = P->at[i];
    }

    if (ori == 0)
    {
        for (int i = 0; i < Q->num; i++)
        {
            Q->dx[i] = P->dx[i];
            Q->dy[i] = P->dy[i];
            Q->pos[i] = Q->dy[i] * w + Q->dx[i];
        }
    }
    if (ori == 1)
    {
        for (int i = 0; i < Q->num; i++)
        {
            Q->dx[i] = P->dy[i];
            Q->dy[i] = - P->dx[i];
            Q->pos[i] = Q->dy[i] * w + Q->dx[i];
            //Q->pos[i] = -(P->pos[i] % w) * w + (P->pos[i] / w);
        }
    }
    if (ori == 2)
    {
        for (int i = 0; i < Q->num; i++)
        {
            Q->dx[i] = - P->dx[i];
            Q->dy[i] = - P->dy[i];
            Q->pos[i] = Q->dy[i] * w + Q->dx[i];
            //Q->pos[i] = -(P->pos[i]);
        }
    }
    if (ori == 3)
    {
        for (int i = 0; i < Q->num; i++)
        {
            Q->dx[i] = - P->dy[i];
            Q->dy[i] = P->dx[i];
            Q->pos[i] = Q->dy[i] * w + Q->dx[i];
            //Q->pos[i] = (P->pos[i] % w) * w - (P->pos[i] / w);
        }
    }
}
