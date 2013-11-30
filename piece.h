typedef struct
{
    // Simple id.
    char id;
    // Number of segments.
    int num;
    // Position of each segment. Only used by piece Q.
    char *pos;
    // For segments other than 0, the spatial difference from segment 0.
    char *dx;
    char *dy;
    // What unit is at this segment: nothing/cop/robber.
    char *at;
} piece;

int import_pieces(piece*** P, FILE* f, piece** Q);
void free_pieces(piece** P, int k, piece* Q);
void turn(piece* Q, piece* P, int ori, int w);
