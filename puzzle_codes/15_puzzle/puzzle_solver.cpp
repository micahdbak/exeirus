#include "puzzle_solver.h"

using namespace std;

// *******************************************
// comparator structure for minimum priority queue of Queue_Node's
// *******************************************

// Calculates the "distance" between boards. Which is closer to completion of puzzle in least steps
struct Compare_Boards {
    bool operator()(const Queue_Node* q1, const Queue_Node* q2) {
        return (q1->moves + q1->manhattan) - (q2->moves + q2->manhattan) > 0;
    }
};

// *******************************************
// Queue_Node class functions
// *******************************************


Queue_Node::Queue_Node (Board* _brd, int _moves, int _manhattan, Board* _prev) {
    this->brd = _brd;
    this->moves = _moves;
    this->manhattan = _manhattan;
    this->prev = _prev;
}


// *******************************************
// Solver class functions
// *******************************************


Solver::Solver (Board* initial) {
    this->moves = 0;

    // Min-oritented priority queue. Value is by distance from completion in the least amount of steps
    priority_queue <Queue_Node*, vector<Queue_Node*>, Compare_Boards> pq;
    pq.push( new Queue_Node( initial, 0, initial->get_manhattan(), NULL ) );

    // For future correct disposal of all memory
    vector<Board*> garbage;

    // Solving the board - go over priority queue and get new iterations of the board
    Queue_Node* current;
    while (true) {
        // Get the element closest to completion from the queue - remove it as well
        current = pq.top(); pq.pop();
        garbage.push_back(current->brd);

        // If the board is sovled - transfer all remaining nodes to garbage vector and stop the loop
        if (current->brd->is_solved()) {
            while (!pq.empty()) {
                garbage.push_back(pq.top()->brd);
                pq.pop();
            }
            break;
        }

        // If not solved - consider all neighbors and add to priority queue as long as it's not the copy of previous board
        for (Board* neighbor : current->brd->neighbors()) {
            if ( !neighbor->equals(current->prev) )
                pq.push( new Queue_Node( neighbor, current->moves+1, neighbor->get_manhattan(), current->brd ) );
            else
                delete neighbor;
        }

        
    }

    this->moves = current->moves;

    for (auto it : garbage) {
        delete it;
    }
}   


int Solver::get_moves () {
    return this->moves;
}


int main () {
    srand(time(0));

    Board* brd = new Board();
    brd->shuffle();

    Solver* temp = new Solver (brd);
    printf("Took %d moves\n", temp->get_moves());

    return 0;
}