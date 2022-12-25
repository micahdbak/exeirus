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


Queue_Node::Queue_Node (Board* _brd, int _moves, int _manhattan, Queue_Node* _prev) {
    this->brd = _brd;
    this->moves = _moves;
    this->manhattan = _manhattan;
    this->prev = _prev;
}


// *******************************************
// Queue_Node class functions
// *******************************************


Game_Node::Game_Node (Board* _brd, Game_Node* _next) {
    this->brd = _brd;
    this->next = _next;
}


// *******************************************
// Solver class functions
// *******************************************


Solver::Solver (Board* initial) {
    this->head = NULL;

    // Min-oritented priority queue. Value is by distance from completion in the least amount of steps
    priority_queue <Queue_Node*, vector<Queue_Node*>, Compare_Boards> pq;
    pq.push( new Queue_Node( initial, this->moves, initial->get_manhattan(), new Queue_Node(NULL, 0, 0, NULL) ) );

    // Solving the board - go over priority queue and get new iterations of the board
    Queue_Node* current;
    while (true) {
        // Get the element closest to completion from the queue - remove it as well
        current = pq.top(); pq.pop();
        this->garbage.push_back(current->brd);

        // For when board is solved
        if (current->brd->is_solved()) {

            // Transfer everything to garbage collector
            while (!pq.empty()) {
                this->garbage.push_back(pq.top()->brd);
                pq.pop();
            }
            
            // Start recording moves
            this->moves = 0;

            // Back-track the solution to the board
            while (current->brd != NULL) {
                Game_Node* temp = this->head;
                this->head = new Game_Node(current->brd, temp);

                this->moves++;
                current = current->prev;
            }
            // Break the outer loop
            break;
        }

        // If not solved - consider all neighbors and add to priority queue if it's not the copy of previous board
        for (Board* neighbor : current->brd->neighbors()) {
            if (!neighbor->equals(current->prev->brd) )
                pq.push( new Queue_Node( neighbor, current->moves+1, neighbor->get_manhattan(), current ) );
            else
                delete neighbor;  // Delete in case the board was already 'seen'
        }

        
    }

    this->moves--;  // Counts 1 extra move due to set up through virtual start point
} 


Solver::~Solver () {
    for (auto it : this->garbage) {
        delete it;
    }
}


int Solver::get_moves () {
    return this->moves;
}


string Solver::key_step () {
    int step = rand () % (this->moves - 1) + 1;  // To avoid the key being initial solution we start from 1
    // Find the randomly selected step
    Game_Node* current = this->head;
    for (int i = 0; i < step; i++) {
        current = current->next;
    }
    // 1d representation of that step is the key
    return current->brd->to_str ();
}


void Solver::print_solution () {
    printf("Solution:\n");

    Game_Node* current = this->head;
    while (current != NULL) {
        current->brd->print_board();
        printf("\n");

        current = current->next;
    }
}