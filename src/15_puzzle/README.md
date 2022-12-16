Note:
	This 15-puzzle solver does NOT check for whether the puzzle is solvable or not. 
	It assumes it is and will be stuck in an infinite loop if it is passed an unsolvable puzzle

Usage:
	1) Declare random seed
	2) Create the board either a random one or by passing 'int arr[4][4]' into the constructor with needed values. 0 represents blank tile
	3) Create an instance of the solver class, and pass the board.
	4) Done, board is solved.
	5) main.cpp contains function to save the board to the .txt file in the format:
		$moves
		$string_representation_of_board
	where initial board is represented as 1d string

	Example:

	int main () {
	    srand(time(0));

	    Board* brd = new Board();

	    Solver* solv = new Solver (brd);
	    save_to_file (solv, brd);
	    
	    delete brd;
	    delete solv;

	    return 0;
	}
	
API (for users):
	Board:
		void shuffle ()            - shuffles the board
		bool equals (Board* x)     - checks if 2 boards are equal (same elements at the same positions)
		void print_board ()        - prints the board to the terminal as 2d array. 0 represents blank tile
		string to_str ()           - returns 1d string representation of the board
		vector<Board*> neighbors() - returns *neighbors* of the current board, i.e. when blank tile is moved once in possible directions
	Solver:
		int get_moves ()           - returns the minimum amount of moves needed to solve the board that was passed to it
		void print_solution ()     - prints out all iterations of the board in solution steps. WARNING - can flood your terminal
