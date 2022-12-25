import subprocess
import platform
from extractor import  create_file, read_file
from flask import Flask, render_template


create_file("precompiled/")

init_vals = read_file("puzzle.txt")

moves = init_vals[0]
# original_layout = init_vals[1]
original_layout = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 15]
key_step = init_vals[2]

app = Flask(__name__,
              template_folder='web/templates')

@app.route('/')
def start ():
    return render_template('15_Puzzle_Index.html',
                        moves=moves,
                        original_layout=original_layout,
                        page=key_step)

@app.route(f'/{key_step}')
def solved_puzzle ():
    return render_template('15_Puzzle_Solved.html',
                           key_step=key_step)


if __name__ == '__main__':
    app.run ()