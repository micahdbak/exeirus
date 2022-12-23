#include <fstream>
#include <iostream>
#include "puzzle_solver.h"

#define MIN_MOVES 15
#define MAX_MOVES 20
#define FILENAME "puzzle.txt"

using namespace std;

// Saves information neeeded about the generated puzzle into .txt file
void save_to_file (Solver* solv, Board* brd) {
    ofstream save_file;
    save_file.open(FILENAME);

    if (save_file.is_open()) {
        save_file << solv->get_moves () << endl;  // Record number of moves to solve the puzzle
        save_file << brd->to_str () << endl;      // Record initial state as 1d array
        save_file << solv->key_step ();           // Record the key for user to use as 1d array
        save_file.close();
    }
    else {
        cout << "ERROR: file cannnot be opened or created, do something to solve it." << endl;
    }
}


int main () {
    srand(time(0));

    Board* brd;
    Solver* solv;
    int moves;

    // Generate the puzzle with required amount of moves to solve through trial and error
    while (true) {
        brd = new Board ();
        printf("\n");
        brd->print_board ();

        solv = new Solver (brd);
        moves = solv->get_moves ();

        if (moves >= MIN_MOVES && moves <= MAX_MOVES)
            break;
        
        delete solv;
    }

    cout << "Took " << moves << " moves to solve" << endl;
    cout << "To str rep: '" << brd->to_str () << "'" << endl;

    save_to_file (solv, brd);

    delete brd;
    delete solv;

    return 0;
}