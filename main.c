#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "board.h"
#include "piece.h"
#include "challenge.h"
#include "check.h"

#define MAJOR 1
#define MINOR 1
#define MAX_CALC 1000000
#define MAX_WORK 5000

int main(int argc, char* argv[])
{
    int i, j, logged = 0, saved = 0, verbatim = 0, maxnum = 0;
    printf("CopChase Solver\n");
    printf("< version %i.%i >\n\n", MAJOR, MINOR);

    if (argc < 3)
    {
        printf("Too few arguments. No boardfile or piecesfile specified.\nAborting.\n\n");
        return -1;
    }

    char* boardfile = argv[1];//"board2.txt";
    char* piecesfile = argv[2];//"pieces2.txt";
    char* challengefile = "null";
    char* outputfile = "null";
    char* logfile = "null";
    if (argc >= 4)
        challengefile = argv[3];
    if (argc >= 5)
        outputfile = argv[4];
    if (argc >= 6)
        logfile = argv[5];
    if (argc >= 7)
        verbatim = atoi(argv[6]);

    FILE* output;
    if (strcmp(outputfile,"null") != 0)
    {
        saved = 1;
        output = fopen(outputfile, "w");
        if (output == NULL)
        {
            printf("Unable to open <%s>.\n", outputfile);
            return -1;
        }
        fprintf(output, "This file is automatically produced.\n");
        fprintf(output, "Do not alter unless you know what you're doing.\n");
        fprintf(output, "\n###\n\n");
    }

    FILE* log;
    if (strcmp(logfile,"null") != 0)
    {
        logged = 1;
        log = fopen(logfile, "w");
        if (log == NULL)
        {
            printf("Unable to open <%s>.\n", logfile);
            return -1;
        }
        fprintf(log, "This log is automatically generated.\n");
        fprintf(log, "It may be freely altered or deleted.\n");
        fprintf(log, "\n###\n\n");
    }

    if (verbatim)
        printf("Saved: %i\tLogged: %i\n\n", saved, logged);

    FILE* f = fopen(boardfile, "r");
    if (f == NULL)
    {
        printf("Unable to open <%s>.\n", boardfile);
        return -1;
    }
    board* B = import_board(f);
    fclose(f);
    if (logged)
        fprintf(log, "Imported board from %s.\n", boardfile);

    piece** P;
    f = fopen(piecesfile, "r");
    if (f == NULL)
    {
        printf("Unable to open <%s>.\n", piecesfile);
        return -1;
    }
    int n_pieces = import_pieces(&P, f, &maxnum);
    fclose(f);
    if (logged)
        fprintf(log, "Imported pieces from %s.\n", piecesfile);

    challenge* C;
    if (strcmp(challengefile,"null") != 0)
    {
        f = fopen(challengefile, "r");
        if (f == NULL)
        {
            printf("Unable to open <%s>.\n", challengefile);
            return -1;
        }
        C = import_challenge(f);
        fclose(f);
        if (logged)
            fprintf(log, "Imported challenge from %s.\n", challengefile);
    }
    else
    {
        C = empty_challenge();
        if (logged)
            fprintf(log, "Created empty challenge.\n");
    }

    if (logged)
        fprintf(log, "\n");

    int num;
    num = calc_poss(C, B->size);
    printf("Number of possibilities: %i.\n\n", num);
    if (saved)
        fprintf(output, "Number of possibilities: %i.\n\n", num);

    // BEGIN

    piece *Q = malloc(sizeof(piece));
    Q->pos = malloc(maxnum * sizeof(char));
    Q->dx = malloc(maxnum * sizeof(char));
    Q->dy = malloc(maxnum * sizeof(char));

    int ori[n_pieces], spot[n_pieces];
    char t[n_pieces][B->size];
    char m[n_pieces][num];
    int pc = 0;
    ori[0] = 0;
    spot[0] = 0;
    int n_found = 0;
    int work, busy, done = 0;

    for (work = 0; work < MAX_WORK; work++)
    {
        for (busy = 0; busy < MAX_CALC; busy++)
        {
            if (pc == 0)
            {
                //fprintf(output, "pc == 0; (%i,%i)\n", ori[0], spot[0]);
                for (j = 0; j < B->size; j++)
                {
                    t[0][j] = B->blocked[j];
                }

                for (j = 0; j < num; j++)
                {
                    m[0][j] = 1;
                }
            }
            else
            {
                for (j = 0; j < B->size; j++)
                {
                    t[pc][j] = t[pc - 1][j];
                }
                for (j = 0; j < num; j++)
                {
                    m[pc][j] = m[pc - 1][j];
                }
            }

            if (spot[pc] == 0)
                turn(Q, P[pc], ori[pc], B->width);

            if (spot[pc] < B->size)
            {

                if (fits(B, t[pc], Q, spot[pc]))
                {
                    if (logged)
                        fprintf(log, "pc[%i]: %c; (%i,%i) fits", pc, Q->id, ori[pc], spot[pc]);

                    matches(m[pc], C, Q, spot[pc]);

                    if (matchex(m[pc], num))
                    {
                        if (logged)
                            fprintf(log, " and matches");
                        //binarystring(n_pieces, (1 << (n_pieces - 1 - pc)));
                        //printf(" <<\n");

                        for (i = 0; i < Q->num; i++)
                        {
                            t[pc][spot[pc] + Q->pos[i]] = 1;
                        };

                        if (pc == n_pieces - 1)
                        {
                            if (verbatim)
                            {
                                printf("Found ");
                                for (i = 0; i < n_pieces; i++)
                                {
                                    printf("p%i r%i/at%i, ", i, ori[i], spot[i]);
                                }
                                printf("(%i)", matchex(m[pc], num));
                                //binarystring(C->n_poss, m[pc]);
                                printf("\n");
                            }
                            if (saved)
                            {
                                fprintf(output, "Found ");
                                for (i = 0; i < n_pieces; i++)
                                {
                                    fprintf(output, "p%i r%i/at%i, ", i, ori[i], spot[i]);
                                }
                                fprintf(output, "(%i)", matchex(m[pc], num));
                                //binarystring(C->n_poss, m[pc]);
                                fprintf(output, "\n");
                            }
                            n_found += 1;
                            spot[pc] += 1;
                        }
                        else
                        {
                            pc += 1;
                            for (j = 0; j < num; j++)
                            {
                                m[pc][j] = m[pc - 1][j];
                            }
                            for (j = 0; j < B->size; j++)
                            {
                                t[pc][j] = t[pc - 1][j];
                            }
                            ori[pc] = 0;
                            spot[pc] = 0;
                        }
                    }
                    if (logged)
                        fprintf(log, "\n");
                }
                else
                {
                    spot[pc] += 1;
                }
            }

            if (!matchex(m[pc], num))
                spot[pc] += 1;
            if (spot[pc] >= B->size)
            {
                ori[pc] += 1;
                spot[pc] = 0;
            }
            if (ori[pc] >= 4)
            {
                pc -= 1;
                if (pc >= 0)
                {
                    spot[pc] += 1;
                    turn(Q, P[pc], ori[pc], B->width);
                }
            }
            if (pc < 0)
            {
                done = 1;
                break;
            }
        }
        if (done)
            break;
        else
        {
            printf("[ %i%% || %i million calc. perf. ]\n",
                   25 * spot[0] / B->size + 25 * ori[0], work + 1);
        }
    }

    if (!done)
    {
        printf("\nMaximum number of calculations reached.\n");
    }

    if (work == 0)
        printf("\nNumber of calculations performed: %i.\n", busy + 1);
    else
        printf("\nNumber of calculations performed: over %i million.\n", work);

    if (done)
    {
        printf("\nFound %i solutions.\n", n_found);
        if (saved)
            fprintf(output, "\nFound %i solutions.\n", n_found);
    }

    // END

    free(Q->pos);
    free(Q->dx);
    free(Q->dy);
    free(Q);
    free_pieces(P, n_pieces);
    free_challenge(C);
    free_board(B);

    if (saved)
    {
        fprintf(output, "\n\n[ Done. ]\n");
        fclose(output);
    }

    if (logged)
    {
        fprintf(log, "\n\n[ Done. ]\n");
        fclose(log);
    }

    printf("\a\n\n[ Done. ]\n\n");
    scanf("%i", &verbatim);
    return 0;
}
