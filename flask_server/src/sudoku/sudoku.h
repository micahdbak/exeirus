#ifndef SUDOKU_H
#define SUDOKU_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <string>

#include "sudoku.h"

#define BOX_SIDE 3
#define SIZE 9

class Sudoku {
public:
    // Deep copy of the grid array - free to access by user
    int reserve[SIZE][SIZE];

    // Inializes fully empty board
    Sudoku ();
    // Initializes board from the given values
    Sudoku (int _grid[SIZE][SIZE]);    

    // Randomly generates a complete valid board
    void create_full ();
    // Generates the incomplete solvable board
    void create_sudoku ();

    // Given the current board - find it's solution (fully filled)
    bool solve_sudoku ();
    // Calculates how many solutions the current board has
    int count_solutions (int row = 0, int col = 0, int count = 0);

    // Prints sudoku in 9x9 fashion
    void print_board ();
    // Returns string representation of the board in same format as print_board
    std::string board_to_str ();

private:  
    // Represents the board                      
    int grid[SIZE][SIZE];       
    

    // Stores positions that aren't yet filled or inspected. For random generation
    int positions[SIZE * SIZE];
    // Tracks the last free position in array above
    int free_to;

    // Copy grid to reserve if true, reserve to grid if false
    void deep_copy (bool to_reserve);

    // Returns true if there's at least one 0 in grid - aka undefined tile
    bool find_free (int& row, int& col);

    // Checks if we can put the given number into the position without breaking the ruls
    bool check_rules (int row, int col, int num);
};


#endif