typedef struct
{
    char id;
    int num;
    char *pos;
    char *dx;
    char *dy;
    char *at;
} piece;

int import_pieces(piece*** P, FILE* f, int* maxnum);
void free_pieces(piece** P, int k);
void turn(piece* Q, piece* P, int ori, int w);
