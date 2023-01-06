import os
from flask import Flask, render_template, request

def create_app (config=None):

    #
    # Flask server
    #

    app = Flask(__name__, template_folder='./templates')

    # This is a terrible cryptographic key. Please set it in config.py.
    app.secret_key = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'

    if config is None:
        app.config.from_pyfile('config.py', silent=True)
    else:
        app.config.from_mapping(config)

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
    # 15_Puzzle
    #

    from . import fifteen

    fifteen.fifteen(app)
    
    #
    # Sudoku
    #
    
    from . import sudoku
    
    sudoku.sudoku(app)

    return app
