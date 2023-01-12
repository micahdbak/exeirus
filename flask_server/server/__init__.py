import os
from flask import Flask, render_template

from . import users, sudoku, fifteen

SECRET_KEY     = "7NfBbChMw5Ohbi3Q8F4iaWjM5NYhgNbd"

FIFTEEN_URL    = "Cm8PrJj7m302AjzBYagkSN6JwzvBtD8A"
SUDOKU_URL     = "eBlsv2O529h3BB2faqcRayS8ZWSR03cH"
HAMMING_URL    = "vDjD2ZVyvia2USuAMRoRcjk9IPCLPFDb"
BRAINHECK_URL  = "aK9uRaR3IzescIs5sQd1dznoBRNQE2uV"

COMPLETION_URL = "VF5oW6oovAy8zrfGOykpwJVH0Zvymi5A"

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

    # Leaderboard
    app.register_blueprint(users.bp)

    # Sudoku
    sudoku.sudoku(app, SUDOKU_URL, FIFTEEN_URL)

    # 15_Puzzle
    fifteen.fifteen(app, FIFTEEN_URL, HAMMING_URL)

    # Hamming
    @app.route(f'/{HAMMING_URL}')
    def hamming():
        return render_template('hamming.html')

    # Brainheck
    @app.route(f'/{BRAINHECK_URL}')
    def brainheck():
        return render_template('brainheck.html')

    # Completion
    @app.route(f'/{COMPLETION_URL}')
    def completion():
        return render_template('completion.html')

    return app


if __name__ == '__main__':
    create_app().run()
