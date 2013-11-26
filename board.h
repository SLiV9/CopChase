typedef struct
{
    int size;
    int width;
    int height;
    char* open;
} board;

board* import_board(FILE* f);
void free_board(board* B);
