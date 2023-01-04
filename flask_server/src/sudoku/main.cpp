#include "sudoku.h"
#include <fstream>

#define FILENAME "sudoku.txt"

using namespace std;

void save_to_file (Sudoku* sdk) {
    ofstream save_file;
    save_file.open(FILENAME);

    if (save_file.is_open()) {
        save_file << sdk->board_to_str ();
        save_file.close ();
    }
    else {
        cout << "Error with opening file" << endl;
    }
}


int main () {
    srand(time(0));

    /*int _grid[SIZE][SIZE] = { { 3, 0, 6, 5, 0, 8, 4, 0, 0 },
                                { 5, 2, 0, 0, 0, 0, 0, 0, 0 },
                                { 0, 8, 7, 0, 0, 0, 0, 3, 1 },
                                { 0, 0, 3, 0, 1, 0, 0, 8, 0 },
                                { 9, 0, 0, 8, 6, 3, 0, 0, 5 },
                                { 0, 5, 0, 0, 9, 0, 6, 0, 0 },
                                { 1, 3, 0, 0, 0, 0, 2, 5, 0 },
                                { 0, 0, 0, 0, 0, 0, 0, 7, 4 },
                                { 0, 0, 5, 2, 0, 6, 3, 0, 0 } };

    Sudoku* skd = new Sudoku (_grid);
    sdk->solve_sudoku ();
    sdk->print_board ();

    delete sdk;

    return 0;
    */
    Sudoku* sdk = new Sudoku ();
    
    sdk->create_full ();
    
    sdk->create_sudoku ();

    save_to_file (sdk);

    delete sdk;

    return 0;
}