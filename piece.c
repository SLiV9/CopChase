#include <stdlib.h>
#include <stdio.h>

#include "piece.h"

int import_pieces(piece*** P, FILE* f, piece** Q)
{
    // Determine the number of pieces.
    int n_pieces;
    fscanf(f, "%i\n", &n_pieces);
    *P = malloc(n_pieces * sizeof(piece*));

    // The piece Q will be filled with the contents of another piece,
    // thus should be large enough to contain any of the pieces.
    // Max_segs counts the largest amount of segments in a piece.
    int max_segs = 0;

    // Import each of the pieces.
    for (int i = 0; i < n_pieces; i++)
    {
        // Determine the number of segments in this piece.
        int n_segs;
        fscanf(f, "%i|", &n_segs);
        (*P)[i] = malloc(sizeof(piece));
        (*P)[i]->id = 'A'+i;
        (*P)[i]->num = n_segs;
        if (n_segs > max_segs)
        {
            max_segs = n_segs;
        }

        // P does not need a pos, this is calculated for Q at turn().
        (*P)[i]->dx = malloc(n_segs * sizeof(char));
        (*P)[i]->dy = malloc(n_segs * sizeof(char));
        (*P)[i]->at = malloc(n_segs * sizeof(char));

        // Read each of the segments.
        for (int j = 0; j < n_segs; j++)
        {
            int x, y, a;
            fscanf(f, "%i.%i:%i", &y, &x, &a);
            (*P)[i]->dx[j] = (char) x;
            (*P)[i]->dy[j] = (char) y;
            (*P)[i]->at[j] = (char) a;
            if (j < n_segs-1)
            {
                fscanf(f, ",");
            }
        }
        fscanf(f, "\n");

        // The position of segment 0 is used by the GUI to arrange the pieces nicely,
        // but is of no use to us. It is set to 0, since that is its distance to 0.
        (*P)[i]->dx[0] = 0;
        (*P)[i]->dy[0] = 0;
    }

    // Another piece is constructed that will hold the contents of any of the pieces
    // after they have been turned.
    *Q = malloc(sizeof(piece));
    (*Q)->pos = malloc(max_segs * sizeof(char));
    (*Q)->dx = malloc(max_segs * sizeof(char));
    (*Q)->dy = malloc(max_segs * sizeof(char));
    // Q->at is not malloced, since its always a reference to another piece's at.

    return n_pieces;
}

void free_pieces(piece **P, int n_pieces, piece* Q)
{
    free(Q->pos);
    free(Q->dx);
    free(Q->dy);
    free(Q);

    for (int i = 0; i < n_pieces; i++)
    {
        free(P[i]->dx);
        free(P[i]->dy);
        free(P[i]->at);
        free(P[i]);
    }
    free(P);
}

void turn(piece *Q, piece *P, int ori, int w)
{
    // The piece Q is filled with the contents of piece P as if it were rotated
    // ori * 90 degrees counterclockwise (?). To determine the actual position,
    // the width of the board, w, is used.
    Q->id = P->id;
    Q->num = P->num;
    Q->at = P->at;

    switch (ori)
    {
        case 1:
        for (int i = 0; i < Q->num; i++)
        {
            Q->dx[i] = P->dy[i];
            Q->dy[i] = - P->dx[i];
            Q->pos[i] = Q->dy[i] * w + Q->dx[i];
        }
        break;

        case 2:
        for (int i = 0; i < Q->num; i++)
        {
            Q->dx[i] = - P->dx[i];
            Q->dy[i] = - P->dy[i];
            Q->pos[i] = Q->dy[i] * w + Q->dx[i];
        }
        break;

        case 3:
        for (int i = 0; i < Q->num; i++)
        {
            Q->dx[i] = - P->dy[i];
            Q->dy[i] = P->dx[i];
            Q->pos[i] = Q->dy[i] * w + Q->dx[i];
        }
        break;

        default:
        for (int i = 0; i < Q->num; i++)
        {
            Q->dx[i] = P->dx[i];
            Q->dy[i] = P->dy[i];
            Q->pos[i] = Q->dy[i] * w + Q->dx[i];
        }
        break;
    }
}
