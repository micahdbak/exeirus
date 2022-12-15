#ifndef BOARD_H
#define BOARD_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>

#include "board.h"

// ******************************************
// Represents the board of 15-puzzle game,
// Handles manhattan and hamming distances
// ******************************************

class Board {
private:
    int tiles[4][4];        // 2d represenation of the board
    int row, col;           // position of zero (blank) on the board

    int hamming, manhattan; // *offset* from the final state through 2 methods
    // hamming = number of tiles not in their solved place
    // manhattan = total distance of tiles from their solved place

    void move (int dir);    // Moves blank slate in given direction. 0-up, 1-down, 2-left, 3-right
    void update_distance (int i, int j); // Updates distances when we moved [i][j]th element to blank spot

public:
    Board ();                   // Creates board in default (solved) state
    Board (int tiles_p[4][4]);  // Creates board from given tiles
    void shuffle ();            // Shiffles current board

    int get_hamming ();         // Getter for hamming distance
    int get_manhattan ();       // Getter for manhattan distance
    bool is_solved ();          // States whether the board is solved
    bool equals (Board* x);     // Boards are equal when all of their elements are in the same places

    void print_board ();

    // Returns vector of neighbors (0 moved in all directions if possible) that can exist
    std::vector<Board*> neighbors (); 
}; 

#endif