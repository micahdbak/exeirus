Note:
	This 15-puzzle solver does NOT check for whether the puzzle is solvable or not. 
	It assumes it is and will be stuck in an infinite loop if it is passed an unsolvable puzzle

Usage:
	1) Declare random seed
	2) Create the board either a random one or by passing 'int arr[4][4]' into the constructor with needed values. 0 represents blank tile
	3) Create an instance of the solver class, and pass the board.
	4) Done, board is solved. You can retrieve the number of needed moves through 'Solver->get_moves();'

Example:

int main () {
    srand(time(0));

    Board* brd = new Board();

    Solver* temp = new Solver (brd);
    printf("Took %d moves\n", temp->get_moves());

    return 0;
}