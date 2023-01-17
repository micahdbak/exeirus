from flask import Flask, render_template, request
import copy, os, random, string


# Main func - creates the page for the puzzle
def fifteen (app, url, sol):
    # Run the ./bin/15_puzzle program on start, so that ./puzzle.txt exists.
    if not os.path.exists('./puzzle.txt'):
        try:
            os.system('./bin/15_puzzle')
        except OSError:
            pass


    # Read the created ./puzzle.txt file.
    init_vals = read_15('./puzzle.txt')
    moves = init_vals[0]
    orig_layout = init_vals[1]
    # orig_layout = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 15]
    key = init_vals[2]

    # Main page of 15 puzzle
    @app.route(f'/{url}', methods=["GET", "POST"])
    def start_15 ():
        # Receiving the message from JavaScript about possible completion
        if request.method == "POST":
            json_data = request.get_json()
            rspn = "Allowed"

            # Try to retrieve needed values from passed values
            try:
                moves_left = int(json_data['left'])
                solution = json_data['check']
                # Check if completion is valid
                if not check_15_solution(solution, orig_layout, moves - moves_left, moves):
                    rspn = "Declined"
            # If not - an attempt to cheat
            except Exception:
                rspn = "Declined"
            # Sending the message to JavaScript about validity with the link to redirect to
            return {
                'state': rspn,
                'link': f'{sol}' if rspn == "Allowed" else 'https://youtu.be/P8fovVJ17GE?t=6'
            }

        return render_template('fifteen_index.html',
                               moves=moves,
                               original_layout=orig_layout,
                               url=url)


def read_15 (filename):
    # Output array
    init_vals = []

    file = open(filename, "r")
    # Pass all strings into the array
    for line in file:
        init_vals.append(line)

    # Convert moves from string to int
    init_vals[0] = int(init_vals[0])
    # Break string into numbers and map to integers
    init_vals[1] = list( map( int,  init_vals[1].split(" ")) )
    # Create the key consisting of random ASCII characters
    letters = string.ascii_letters
    init_vals.append( ''.join(random.choice(letters) for i in range(8)) )

    return init_vals


# Checks if the solution is valid:
# 1) Starts from the original board and ends at correct board
# 2) Amount of moves used is valid and matches recorded steps
# 3) Is continuous - each next board is a neighbor of previous
def check_15_solution (solution, orig, delta_moves, min_moves):
    # Try to work with received values
    try:
        lines = solution.splitlines()

        # Check if amount of moves used is less than minimum and
        # If amount of recorded steps doesn't match moves used (+1 for initial state in lines)
        if delta_moves < min_moves or len(lines) != delta_moves + 1:
            return False

        # Check for first and last values of solution - must be original and final
        mapped = list( map( int,  lines[0].split(" ")) )
        for i in range(len(  mapped ) ):
            if mapped[i] != orig[i]:
                return False
        if lines[-1] != "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 0":
            return False

        # Check for continuity of solution
        prev, cur = 0, 0
        for line in lines:
            prev = cur
            cur = to_array(line)

            # Check if previous move and current move are 2 neighbor boards
            if prev != 0 and are_neighbors(copy.deepcopy(prev), copy.deepcopy(cur)) == False:
                return False

        return True
    # If not possible - obviously an attempt to cheat
    except Exception:
        return False


# Checks if 2 boards are "neighbors"
# 1) Check for adjacent zeroes
# 2) Swap so that zeroes are in same position
# 3) Check if boards are the same
def are_neighbors(board_a, board_b):
    row, col = 0, 0
    # Find 0 in first board
    for r in range(4):
        for c in range(4):
            if board_a[r][c] == 0:
                row, col = r, c

    # Check from the top
    if row > 0 and board_b[row - 1][col] == 0:
        board_b[row][col], board_b[row - 1][col] = 0, board_b[row][col]
    # Check from the bottom
    elif row < 3 and board_b[row + 1][col] == 0:
        board_b[row][col], board_b[row + 1][col] = 0, board_b[row][col]
        pass
    # Check from the left
    elif col > 0 and board_b[row][col - 1] == 0:
        board_b[row][col], board_b[row][col - 1] = 0, board_b[row][col]
    # Check from the right
    elif col < 3 and board_b[row][col + 1] == 0:
        board_b[row][col], board_b[row][col + 1] = 0, board_b[row][col]
    # If none are true - boards aren't neighbors
    else:
        return False

    for r in range(4):
        for c in range(4):
            if board_a[r][c] != board_b[r][c]:
                return False

    return True


# Transforms 1 line which represents a state into 2d array
def to_array (line):
    one_d = []  # Aux array to store 4 values
    two_d = []
    n = len(line)
    i = 0
    while i < n:
        if line[i] == ' ':
            i += 1
            continue

        if i < n-1 and line[i+1] != ' ':
            one_d.append( int( line[i:i+2] ))
            i = i + 2
        else:
            one_d.append( int( line[i] ))
            i += 1

        if len(one_d) == 4:
            two_d.append(one_d)
            one_d = []

    return two_d
