#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include <queue>

#include "board.h"
#include "puzzle_solver.h"


// Aux class used for priority queue
class Queue_Node {
public:
    Board* brd;
    int moves;
    int manhattan; // caching manhattan distance
    Queue_Node* prev;   // previous node, used to avoid looping + tracking game solution
    
    Queue_Node (Board* _brd, int _moves, int _manhattan, Queue_Node* _prev);
};


// Aux class used for saving the game solution in linked list
class Game_Node {
public:
    Board* brd;
    Game_Node* next;

    Game_Node (Board* _brd, Game_Node* _next);
};


// Main class that solves the puzzle at initialization
class Solver {
private:
    int moves;       // Total moves needed to solve
    Game_Node* head; // Head of linked list of solutions
    std::vector<Board*> garbage; // 'garbage collector', stores all iterations of the board to dispose of them in destructor

public:
    Solver (Board* initial);
    ~Solver ();

    int get_moves ();
    std::string key_step (); // Returns string representation of 1 of the steps throught solution
    void print_solution ();
};

#endif