import os
from flask import Flask, render_template

SECRET_KEY     = "aiusdfasdifiasdiufasdf"

CIPHERHELL_URL = "cipherhell"
FIFTEEN_URL    = "fifteen"
SUDOKU_URL     = "sudoku"
HAMMING_URL    = "hamming"
BRAINHECK_URL  = "brainheck"

COMPLETION_URL = "completion"

def create_app ():

    #
    # Flask server
    #

    app = Flask(__name__, template_folder='./templates')

    app.secret_key = SECRET_KEY

    try:
        os.makedirs(app.instance_path)
    except OSError:
        pass

    #
    # Leaderboard
    #

    from . import users

    app.register_blueprint(users.bp)

    #
    # Sudoku
    #

    from . import sudoku

    sudoku.sudoku(app, SUDOKU_URL, FIFTEEN_URL)

    #
    # 15_Puzzle
    #

    from . import fifteen

    fifteen.fifteen(app, FIFTEEN_URL, COMPLETION_URL)

    #
    # Completion
    #

    @app.route(f'/{COMPLETION_URL}')
    def completion():
        return render_template('completion.html')

    return app


if __name__ == '__main__':
    create_app().run()
