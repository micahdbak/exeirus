import os
from flask import Flask, render_template

from . import users, sudoku, fifteen

SECRET_KEY     = "7NfBbChMw5Ohbi3Q8F4iaWjM5NYhgNbd"

ENTRY_URL      = "w5bf6OooZFPhsQ78dNxoHpr4iUMbFiGx"
SUDOKU_URL     = "It9aD1y4k7ivJlbLxHsw04xDkfGaZFTk"
FIFTEEN_URL    = "3OmkQIBYljHwmpSohYRodNbMg7TaW3aB"
HAMMING_URL    = "XSFbPBiLajR1hYLbpFZ5FW3J6KAJfNU6"
BRAINHECK_URL  = "HuvBf1XusCiOSXZWoAkD7HC5Z7H06TNp"
COMPLETION_URL = "UNqtb4VnMRuvlyV2oUnMrde9n6bC69Eh"

PORT = 3141

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

    @app.route(f'/{ENTRY_URL}/index.html')
    def entry():
        return render_template('entry.html')

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
    create_app().run(host='0.0.0.0', port=PORT)
