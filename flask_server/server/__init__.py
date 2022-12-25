import os
from flask import Flask, render_template

def create_app ():
    # Run the ./bin/15_puzzle program on start, so that ./puzzle.txt exists.
    try:
        os.system('./bin/15_puzzle');
    except OSError:
        # Should crash here.
        pass

    # Read the created ./puzzle.txt file.
    from . import fifteen
    init_vals = fifteen.read_file('./puzzle.txt')

    moves = init_vals[0]
    # original_layout = init_vals[1]
    original_layout = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 15]

    # URL of the puzzle completion page
    solved = 'fYNkLnTkjngs4TfYd0r9ZSZAiquFnmFN'

    app = Flask(__name__, template_folder='./templates')

    @app.route('/fifteen')
    def start ():
        return render_template('fifteen_index.html',
                               moves=moves,
                               original_layout=original_layout,
                               page=solved)

    @app.route('/hello')
    def hello ():
        return "Hello, world!"

    @app.route(f'/{solved}')
    def solved_puzzle ():
        return render_template('fifteen_solved.html')

    return app
