typedef struct
{
    int n_streets;
    int** street;
    int* street_len;
    int* variance;
    char* openended;
    int max_len;
    int n_poss;
    char** at;
} challenge;

challenge* import_challenge(FILE* f);
challenge* empty_challenge();
int calc_poss(challenge *C, int boardsize);
void free_challenge(challenge* C);
int twofo(int k);
int trifo(int k);
int twefo(int k);
