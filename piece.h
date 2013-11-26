typedef struct
{
    char id;
    int num;
    char *pos;
    char *dx;
    char *dy;
    char *at;
} piece;

typedef struct
{
    piece* P;
    int n_pieces;
    int max_n_segments;
} pieceholder;

void import_pieces(pieceholder* H, FILE* f);
void destroy_pieces(pieceholder* H);
void turn(piece* target, piece* mold, int ori, int w);
