typedef struct
{
    // A challenge consists of n_streets streets, each consisting of
    // street_len[i] (at most max_len) segments.
    int n_streets;
    int** street;
    int* street_len;
    int max_len;

    // Some streets are openended, meaning they have exits at both ends.
    bool* openended;

    // Streets have a number of ways the cops and robbers can be placed on them.
    // Each normal street contains one cop and one robber, CR.
    // Each openended street contains two cops and two robbers, CRRC.
    // This is called their variance.
    int* variance;

    // A challenge can be solved in n_poss ways, where the cops and robbers
    // are scattered across the board. For each possibility i, at[i][j] tells
    // us which character (none/cop/robber) is at board position j.
    int n_poss;
    char** at;
} challenge;

challenge* import_challenge(FILE* f);
challenge* empty_challenge();
int calc_poss(challenge *C, int boardsize);
void free_challenge(challenge* C);
