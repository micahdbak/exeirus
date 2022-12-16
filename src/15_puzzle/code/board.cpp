#include "board.h"

using namespace std;


// ********************************
// Board private functions
// ********************************


void Board::move (int dir) {
    // Check if we can move the blank slate in given direction
    bool can_move = !(dir == 0 && this->row == 0);         // Moving up
    can_move = can_move && !(dir == 1 && this->row == 3);  // Moving down
    can_move = can_move && !(dir == 2 && this->col == 0);  // Moving left
    can_move = can_move && !(dir == 3 && this->col == 3);  // Moving right
    if (!can_move) return;

    // Record value in the tile to swap and set it to 0 (blank), update the position of 0
    // Update distances before the swap for correct change
    int temp;
    if (dir == 0) {
        this->update_distance (row-1, col);

        temp = this->tiles[row-1][col];
        this->tiles[row-1][col] = 0;
        this->tiles[row][col] = temp;
        
        this->row --;
    }
    else if (dir == 1) {
        this->update_distance (row+1, col);

        temp = this->tiles[row+1][col];
        this->tiles[row+1][col] = 0;
        this->tiles[row][col] = temp;

        this->row ++;
    }
    else if (dir == 2) {
        this->update_distance (row, col-1);

        temp = this->tiles[row][col-1];
        this->tiles[row][col-1] = 0;
        this->tiles[row][col] = temp;
        
        this->col --;
    }
    else if (dir == 3) {
        this->update_distance (row, col+1);

        temp = this->tiles[row][col+1];
        this->tiles[row][col+1] = 0;
        this->tiles[row][col] = temp;

        this->col ++;
    }
}


void Board::update_distance (int i, int j) {
    int value = this->tiles[i][j] - 1;

    // For hamming: (incorr->incorr = no change) (corr->incorr = hamming++) (incorr->corr = hamming--)
    // Check if element used to be in correct place
    if (value == j + i*4)
        hamming++;
    // Check if it will be moved to correct place
    else if (value == this->col + this->row*4) 
        hamming--;

    // For manhattan:
    // Remove the original manhattan values, then add new ones
    this->manhattan -= abs(value / 4 - i);  // Remove vertical part
    this->manhattan -= abs(value % 4 - j);  // Remove horizontal part

    this->manhattan += abs(value / 4 - this->row); // Add new vertical part
    this->manhattan += abs(value % 4 - this->col); // Add new horizontal part
}


// ******************************************
// Board public functions
// ******************************************


Board::Board () {
    // Set default values
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            this->tiles[i][j] = j + i*4 + 1;
    }
    // Set 0 to starting position
    this->tiles[3][3] = 0;
    this->row = 3; this->col = 3;

    // Since board is solved, distances are 0
    this->manhattan = 0;
    this->hamming = 0;

    // Shuffle the board
    this->shuffle ();
}


Board::Board (int tiles_p[4][4]) {
    // Set distances to 0 for future change
    this->hamming = 0;
    this->manhattan = 0;

    // Go through given array, deep copy to own tiles
    // When encounter 0 - update it's position values for the board
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            this->tiles[i][j] = tiles_p[i][j];

            if (tiles_p[i][j] == 0) {
                this->row = i;
                this->col = j;
            }
            // If tile isn't 0 and not on it's place - add to distances
            else if ((tiles[i][j] - 1) != j + i*4) {
                this->hamming ++;
                this->manhattan += abs((this->tiles[i][j]-1) / 4 - i);  // vertical distance
                this->manhattan += abs((this->tiles[i][j]-1) % 4 - j);  // horizontal distance
            }
        }
    }
}


void Board::shuffle () {
    for (int i = 0; i < 100; i++) {
        int dir = rand() % 4;
        this->move (dir);
    }
}


int Board::get_hamming () {
    return this->hamming;
}

int Board::get_manhattan () {
    return this->manhattan;
}


bool Board::is_solved () {
    return this->hamming == 0;
}


bool Board::equals (Board* x) {
    if (x == NULL) return false;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (this->tiles[i][j] != x->tiles[i][j])
                return false;
        }
    }
    return true;
}


void Board::print_board () {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            printf("%d ", this->tiles[i][j]);
        printf("\n");
    }
}


string Board::to_str () {
    string brd_str = "";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            brd_str += to_string(this->tiles[i][j]) + " ";
        }
    }

    // Remove extra ' ' at the end
    brd_str.pop_back();
    return brd_str;
}


vector<Board*> Board::neighbors () {
    vector<Board*> neighbs;
    
    Board* temp;
    // Up-neighbor
    if (this->row > 0) {
        temp = new Board(this->tiles);
        temp->move(0);
        neighbs.push_back(temp);
    }
    // Down-neighbor
    if (this->row < 3) {
        temp = new Board(this->tiles);
        temp->move(1);
        neighbs.push_back(temp);
    }
    // Left-neightbor
    if (this->col > 0) {
        temp = new Board(this->tiles);
        temp->move(2);
        neighbs.push_back(temp);
    }
    // Right-neighbor
    if (this->col < 3) {
        temp = new Board(this->tiles);
        temp->move(3);
        neighbs.push_back(temp);
    }

    return neighbs;
}