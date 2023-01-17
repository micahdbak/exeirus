from flask import Flask, render_template, request, redirect
import os, random, string


# Main func - creates the page for sudoku
def sudoku (app, url, sol):
    # Run the ./bin/sdk program on start, so that ./sudoku.txt exists
    if not os.path.exists('./sudoku.txt'):
        try:
            os.system('./bin/sdk')
        except OSError:
            pass


    # Read the created ./sudoku.txt file
    init_vals = read_sudoku('./sudoku.txt')
    sdk = init_vals[0]
    mask = init_vals[1]
    key = init_vals[2]
    
    # Main page of sudoku
    @app.route(f'/{url}', methods=["GET", "POST"])
    def start_sudoku ():
        if request.method == "POST":
            json_data = request.get_json()
            rspn = "Solved"
            # Try to retrieve needed values from passed values
            try:
                solution = json_data['brd']
                state = check_sudoku_solution(solution, mask, sdk)
                # If user cheated
                if state == -1:
                    rspn = "Idiot"
                # If user solved incorrectly
                elif state == 0:
                    rspn = "Declined"
                # Else (correct solution) keep default positive response
            # If there's an error - likely cheating
            except Exception:
                rspn = "Idiot"

            if rspn == 'Solved':
                return {
                    'state' : rspn,
                    'link' : f'{sol}'
                }
            else:
                return {
                    'state' : rspn,
                    'link' : 'https://youtu.be/tMB4knXZNHM?t=4' if rspn == "Idiot" else '/{url}'
                }

        return render_template('sudoku_index.html',
                               original_sudoku=sdk,
                               pos_mask=mask,
                               url=f'/{url}')


def read_sudoku (filename):
    # Output array
    init_vals = []
    # Array of strings of sudoku
    sdk = []

    file = open(filename, "r")
    # Pass all strings of sudoku into the array
    for line in file:
        # Break the line into number and map to integer
        mapped = map(int, line.split(" "))
        # Convert to list
        sdk.append( list (mapped) )
    # Add sudoku at element 0
    init_vals.append(sdk)

    # Fill out the mask where 1 signifies that position can be changed by user
    mask = []
    cur_row = 0
    for row in init_vals[0]:
        mask.append([])
        for val in row:
            if val == 0:
                mask[cur_row].append(1)
            else:
                mask[cur_row].append(0)
        cur_row += 1
    # Add mask to the array
    init_vals.append(mask)

    # Generate a random key
    letters = string.ascii_letters
    init_vals.append( ''.join(random.choice(letters) for i in range(8)) )
    
    return init_vals


def check_sudoku_solution (sdk, mask, orig_sdk):
    # Try to work with received values
    try:
        # Check if user has modified original values
        for row in range(len(sdk)):
            for col in range(len(sdk[0])):
                # If value isn't modifiable and differs from original
                if mask[row][col] == 0 and sdk[row][col] != orig_sdk[row][col]:
                    return -1

        # Check validity of rows through hashmap
        for row in range (9):
            nums = {}
            for col in range (9):
                val = sdk[row][col]
                nums[val] = nums.get(val, 0) + 1
                if nums[val] > 1:
                    return 0

        # Check validity of columns through hashmap
        for col in range (9):
            nums = {}
            for row in range (9):
                val = sdk[row][col]
                nums[val] = nums.get(val, 0) + 1
                if nums[val] > 1:
                    return 0

        # Check validity of boxes through hashmap
        for box in range (9):
            # Starting coordinates for row and col
            r_st, c_st = 3 * int (box / 3), 3 * (box % 3)

            nums = {}
            for row in range (3):
                for col in range (3):
                    val = sdk[row + r_st][col + c_st]
                    nums[val] = nums.get(val, 0) + 1
                    if nums[val] > 1:
                        return 0
                    
        # If all validity checks are passed
        return 1
    # If not possible - obviously an attempt to cheat
    except Exception:
        return -1
