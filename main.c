#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "board.h"
#include "piece.h"
#include "challenge.h"
#include "check.h"

#define MAJOR 2
#define MINOR 0
#define MAX_CALC 1000000
#define MAX_WORK 5000

int main(int argc, char* argv[])
{
    int logged = 0, saved = 0, verbatim = 0;
    printf("CopChase Solver\n");
    printf("< version %i.%i >\n\n", MAJOR, MINOR);

    // The resources we shall be using:
    // A board.
    board* B;
    // An array of pieces.
    piece** P;
    // A challenge.
    challenge* C;
    // A temporary piece.
    piece* Q;

    // Read the arguments.
    //      boardfile piecesfile [challengefile] [outputfile] [logfile] [verbatim]
    // Any optional [-file] argument can be set to "null" if not used.
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
    {
        challengefile = argv[3];
    }
    if (argc >= 5)
    {
        outputfile = argv[4];
    }
    if (argc >= 6)
    {
        logfile = argv[5];
    }
    if (argc >= 7)
    {
        verbatim = atoi(argv[6]);
    }

    // Open an output file for writing down the results.
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

    // Open a log file for writing down logs.
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
    {
        printf("Saved: %i\tLogged: %i\n\n", saved, logged);
    }

    FILE* f;

    // Import the board from the boardfile.
    f = fopen(boardfile, "r");
    if (f == NULL)
    {
        printf("Unable to open <%s>.\n", boardfile);
        return -1;
    }
    B = import_board(f);
    fclose(f);
    if (logged)
    {
        fprintf(log, "Imported board from %s.\n", boardfile);
    }

    f = fopen(piecesfile, "r");
    if (f == NULL)
    {
        printf("Unable to open <%s>.\n", piecesfile);
        return -1;
    }
    int n_pieces = import_pieces(&P, f, &Q);
    fclose(f);
    if (logged)
    {
        fprintf(log, "Imported pieces from %s.\n", piecesfile);
    }

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
        {
            fprintf(log, "Imported challenge from %s.\n", challengefile);
        }
    }
    else
    {
        C = empty_challenge();
        if (logged)
        {
            fprintf(log, "Created empty challenge.\n");
        }
    }

    if (logged)
    {
        fprintf(log, "\n");
    }

    int n_poss;
    n_poss = calc_poss(C, B->size);
    printf("Number of possibilities: %i.\n\n", n_poss);
    if (saved)
    {
        fprintf(output, "Number of possibilities: %i.\n\n", n_poss);
    }

    // BEGIN (OF HELL)

    // Each piece will have an orientation and a position.
    int ori[n_pieces], spot[n_pieces];

    // Taken[i] tells us what spaces have been taken after i pieces have been placed.
    bool taken[n_pieces][B->size];

    // Match[i] tells us what possibilities still remain after i pieces.
    bool match[n_pieces][n_poss];

    // Pc indicates which piece we are currently placing.
    int pc = 0;
    ori[0] = 0;
    spot[0] = 0;

    // N_found tells us how many solutions we have found so far.
    int n_found = 0;

    // Start working until done, or until max_work is exceeded.
    int work, busy;
    bool done = false;
    for (work = 0; !done && work < MAX_WORK; work++)
    {
        for (busy = 0; !done && busy < MAX_CALC; busy++)
        {
            // First, set the taken and match bitstrings.
            if (pc == 0)
            {
                for (int j = 0; j < B->size; j++)
                {
                    taken[0][j] = B->blocked[j];
                }

                for (int j = 0; j < n_poss; j++)
                {
                    match[0][j] = true;
                }
            }
            else
            {
                for (int j = 0; j < B->size; j++)
                {
                    taken[pc][j] = taken[pc - 1][j];
                }
                for (int j = 0; j < n_poss; j++)
                {
                    match[pc][j] = match[pc - 1][j];
                }
            }

            // If spot = 0, we have started a section where piece pc is turned
            // in a specific way. Save this turned piece in Q.
            if (spot[pc] == 0)
            {
                turn(Q, P[pc], ori[pc], B->width);
            }

            if (spot[pc] < B->size)
            {
                bool fitsandmatches = false;

                if (fits(B, taken[pc], Q, spot[pc]))
                {
                    if (logged)
                    {
                        fprintf(log, "pc[%i]: %c; (%i,%i) fits",
                                pc, Q->id, ori[pc], spot[pc]);
                    }

                    matches(match[pc], C, Q, spot[pc]);

                    // If there are still matchable possibilities:
                    int m_left = matchex(match[pc], n_poss);
                    if (m_left > 0)
                    {
                        if (logged)
                        {
                            fprintf(log, " and matches");
                        }

                        fitsandmatches = true;

                        // Mark each space occupied by the current piece as taken.
                        for (int i = 0; i < Q->num; i++)
                        {
                            taken[pc][spot[pc] + Q->pos[i]] = true;
                        };

                        if (pc == n_pieces - 1)
                        {
                            // If this is the last piece, we have found a solution.
                            if (verbatim)
                            {
                                printf("Found ");
                                for (int i = 0; i < n_pieces; i++)
                                {
                                    printf("p%i r%i/at%i, ",
                                            i, ori[i], spot[i]);
                                }
                                printf("(%i)", m_left);
                                printf("\n");
                            }

                            if (saved)
                            {
                                fprintf(output, "Found ");
                                for (int i = 0; i < n_pieces; i++)
                                {
                                    fprintf(output, "p%i r%i/at%i, ",
                                            i, ori[i], spot[i]);
                                }
                                fprintf(output, "(%i)", m_left);
                                fprintf(output, "\n");
                            }

                            n_found += 1;

                            // Continue to the next spot.
                            spot[pc] += 1;
                        }
                        else
                        {
                            // If this is not the last piece, go deeper.
                            pc += 1;
                            ori[pc] = 0;
                            spot[pc] = 0;
                        }
                    }

                    if (logged)
                    {
                        fprintf(log, "\n");
                    }
                }

                if (!fitsandmatches)
                {
                    spot[pc] += 1;
                }
            }

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
                done = true;
            }
        }

        if (!done)
        {
            printf("[ %i%% || %i million calc. perf. ]\n",
                   (25 * ori[0] + 25 * spot[0] / B->size), work + 1);
        }
    }

    if (!done)
    {
        printf("\nMaximum number of calculations reached.\n");
    }

    if (work == 0)
    {
        printf("\nNumber of calculations performed: %i.\n", busy + 1);
    }
    else
    {
        printf("\nNumber of calculations performed: over %i million.\n", work);
    }

    if (done)
    {
        printf("\nFound %i solutions.\n", n_found);
        if (saved)
        {
            fprintf(output, "\nFound %i solutions.\n", n_found);
        }
    }

    // END

    // Done. Start freeing the used resources.

    free_pieces(P, n_pieces, Q);
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

    // Done, all is freed, all is released.

    printf("\a\n\n[ Done. ]\n\n");

    // Since our data needs to be read, wait for an any enter press to close.
    scanf("%i", &verbatim);
    return 0;
}
