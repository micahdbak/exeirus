# requires python 3.10 or later

# brainheck informal specification:
# - brainheck is comprised of an alternating sequence of digits and special characters
# - at start :: setup an array of 30k u32 integers
# - #> :: increment index by a single digit number, where # is the digit
# - #< :: decrement index by a single digit number, where # is the digit
# - #+ :: increment value at index by a single digit number, where # is the digit
# - #- :: decrement value at index by a single digit number, where # is the digit
# - #. :: write the value at the current and next n indices, then return to the original location. If it is not valid ascii (ie. greater than 127), output no characters.
# - #, :: get value from input and store it in the current location, then step to the next location. Repeat # times.
# - #! :: if the value at the current index is non-zero, jump back # instructions
# finally, all characters not in the following list are considered comments: 0123456789><+-.,!

# uncertain:
# - > :: increment pointer by one (shorthand for 1>) 
# - < :: decrement pointer by one
# - ditto for + - , .

# notes:
# - 0> and 0< are no-ops
# - we will not be using the , operator (getting input) for convenience
# - this interpereter allows multiple digits in a row & takes the latest one

import fileinput

# sanitize input
input_program = ""
program_index = 0
for ch in "".join(fileinput.input()):
    if ch in '0123456789><+-.,!':
        input_program += ch

# setup environment
array = [int(0) for _ in range(0, 30_000)] # please be typed python
index = 0

lastnumber = -1
while program_index < len(input_program):
    ch = input_program[program_index]
    if len(ch) == 1 and ch in '0123456789':
        lastnumber = int(ch)
    elif ch == '>':
        index += lastnumber
    elif ch == '<':
        index -= lastnumber
    elif ch == '+':
        array[index] += lastnumber
    elif ch == '-':
        array[index] += lastnumber
    elif ch == ',':
        print(", has not yet been implemented, exiting immediately")
        quit(0)
    elif ch == '.':
        for i in range(0, lastnumber):
            if array[index+i] < 127:
                print(chr(array[index+i]), end="")
    elif ch == '!':
        if array[index] == 0:
            pass # do nothing, since condition is met!
        else:
            program_index -= lastnumber * 2
            continue
    
    program_index += 1

print("\n", end="")
print("program has completed execution")