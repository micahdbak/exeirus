#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include <queue>

#include "board.h"
#include "puzzle_solver.h"


class Queue_Node {
public:
    Board* brd;
    int moves;
    int manhattan; // caching manhattan distance
    Board* prev;   // previous state of the board - used to avoid repetition
    
    Queue_Node (Board* _brd, int _moves, int _manhattan, Board* _prev);
};


// Main class that solves the puzzle at initialization
class Solver {
private:
    int moves; // Total moves needed to solve

public:
    Solver (Board* initial);
    int get_moves ();
};

#endif