typedef struct
{
    // The dimensions of the board. Size = width * height.
    int size;
    int width;
    int height;

    // Not all spaces on the board have to be usable.
    // (Or rather: the only reason the board is rectangular is for programmer's sake.)
    // Spaces are not blocked by default, but become blocked if specified by the file.
    bool* blocked;
} board;

// Import a board from a file.
void import_board(board* B, FILE* f);

// Destroy a board. Every imported board should be destroyed.
void destroy_board(board* B);
