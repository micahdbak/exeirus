#include "sudoku.h"

using namespace std;


Sudoku::Sudoku () {
    // Set all values to *empty*
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            this->grid[i][j] = 0;
        }
    }
}


Sudoku::Sudoku (int _grid[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            this->grid[i][j] = _grid[i][j];
        }
    }
}


void Sudoku::create_full () {
    // Array of undefined tiles
    for (int i = 0; i < SIZE * SIZE; i++) {
        this->positions[i] = i;
    }
    this->free_to = SIZE * SIZE - 1;

    // While there are non-filled spaces
    while (this->free_to >= 0) {
        // Choose random space from the yet-to-be filled
        int place = rand() % (this->free_to + 1);
        int row = this->positions[place] / SIZE;
        int col = this->positions[place] % SIZE;

        // Find out all possible number that can be placed on this position
        vector<int> valid_nums;
        for (int num = 1; num <= SIZE; num++) {
            if (this->check_rules(row, col, num))
                valid_nums.push_back(num);
        }

        // If there are none - something went wrong, I don't know what
        if (valid_nums.size() == 0) {
            cout << "Error creating" << endl;
            return;
        }

        // Save current state of the board
        this->deep_copy(/*to_reserve=*/true);

        // Go through all possible numbers
        for (int number : valid_nums) {
            // Attempt to put the number and see if board can be solved
            this->grid[row][col] = number;
            
            if (this->solve_sudoku()) {
                // If it can - remove current tile from non-filled
                int temp = this->positions[place];
                this->positions[place] = this->positions[this->free_to];
                this->positions[this->free_to] = temp;
                this->free_to--;

                // Return the board back and place the number at current position
                this->deep_copy(/*to_reserve=*/false);
                this->grid[row][col] = number;
                break;
            }
            else {
                // If not solvable - revert everything
                this->deep_copy (/*to_reserve=*/false);
            }
        }
    }
}


void Sudoku::create_sudoku () {
    // Array of defined tiles that haven't been considered to be removed
    for (int i = 0; i < SIZE * SIZE; i++) {
        this->positions[i] = i;
    }
    this->free_to = SIZE * SIZE - 1;

    // While we have tiles that we haven't checked
    while (this->free_to >= 0) {
        // Chose a random tile that hasn't been inspected
        int place = rand () % (this->free_to + 1);
        int row = this->positions[place] / SIZE;
        int col = this->positions[place] % SIZE;

        // Save current board and try to remove one element from it
        this->deep_copy (/*to_reserve=*/true);
        this->grid[row][col] = 0;
        
        // Find how many solutions are there with this tile undefined
        // If there's only 1 solutions - save the changes, else revert them
        if (this->count_solutions () == 1) {
            this->deep_copy (/*to_reserve=*/false);
            this->grid[row][col] = 0;
        }
        else
            this->deep_copy (/*to_reserve=*/false);

        // This tile was inspected, so we remove it from the list of to-be-inspected
        int temp = this->positions[place];
        this->positions[place] = this->positions[this->free_to];
        this->positions[this->free_to] = temp;
        this->free_to--;
    }
}


bool Sudoku::solve_sudoku () {
    int row, col;
    // If no free tiles were found - sudoku is solved
    if (!this->find_free (row, col))
        return true;

    // Just find the number that satisfies sudoku rules for current position
    for (int num = 1; num <= SIZE; num++) {
        // If num can be placed in location - try to do so
        if (this->check_rules(row, col, num)) {
            this->grid[row][col] = num;


            // If the board is solvable this way - return true
            if (this->solve_sudoku()) {
                return true;
            }

            // If it's not - undo last move and try again with another number
            this->grid[row][col] = 0;
        }
    }

    // Indicates no solution to current state of the board
    return false;
}


// https://stackoverflow.com/questions/24343214/determine-whether-a-sudoku-has-a-unique-solution

int Sudoku::count_solutions (int row, int col, int count) {
    // If reached end of row - set columns to 0 and increase row by 1
    // If it was the last row - sudoku is solved
    if (col == SIZE) {
        col = 0;
        if (++row == SIZE)
            return 1 + count;
    }

    // Skip a filled cell
    if (this->grid[row][col] != 0)
        return this->count_solutions (row, col + 1, count);

    // Now search for several solutions
    // Return if more than 1 solution was found since we only care if there's more than 2
    for (int num = 1; num < SIZE; num++) {
        // If found a legal number to place - try to solve with it
        if (this->check_rules(row, col, num)) {
            this->grid[row][col] = num;
            // Add possible solutions with new layout
            count = this->count_solutions (row, col + 1, count);
        }

        if (count > 1)
            return count;
    }

    // Revert all changes on backtrack
    this->grid[row][col] = 0;
    return count;
}


void Sudoku::deep_copy (bool to_reserve) {
    if (to_reserve) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                    this->reserve[i][j] = this->grid[i][j];
            }
        }
    }

    else {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                    this->grid[i][j] = this->reserve[i][j];
            }
        }
    }
}


bool Sudoku::find_free (int& row, int& col) {
    // Go through the entire grid to find at least one undefined place
    for (row = 0; row < SIZE; row++) {
        for (col = 0; col < SIZE; col++) {
            if (this->grid[row][col] == 0)
                return true;
        }
    }

    return false;
}


bool Sudoku::check_rules (int row, int col, int num) {
    // Check the same row
    for (int cl = 0; cl < SIZE; cl++) {
        if (this->grid[row][cl] == num) 
            return false;
    }

    // Check the same column
    for (int rw = 0; rw < SIZE; rw++) {
        if (this->grid[rw][col] == num) 
            return false;
    }

    // Check the same 3x3 box
    int r_start = row - row % BOX_SIDE;
    int c_start = col - col % BOX_SIDE;
    for (int rw = r_start; rw < r_start + BOX_SIDE; rw++) {
        for (int cl = c_start; cl < c_start + BOX_SIDE; cl++) {
            if (this->grid[rw][cl] == num)
                return false;
        }
    }

    // If all fine - return true
    return this->grid[row][col] == 0;
}

void Sudoku::print_board () {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cout << this->grid[i][j] << " ";
        }
        cout << endl;
    }
}


string Sudoku::board_to_str () {
    string board = "";

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board += (this->grid[i][j] + '0');
            // If it's last character in line - add \n instead of space
            board += (j < SIZE - 1) ? " " : "\n";
        }
    }

    return board;
}