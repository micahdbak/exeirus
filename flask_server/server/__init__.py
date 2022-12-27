import os
from flask import Flask, render_template

def create_app (config=None):


    #
    # 15_puzzle
    #

    # Run the ./bin/15_puzzle program on start, so that ./puzzle.txt exists.
    try:
        os.system('./bin/15_puzzle');
    except OSError:
        pass

    # Read the created ./puzzle.txt file.
    from . import fifteen
    init_vals = fifteen.read_file('./puzzle.txt')

    moves = init_vals[0]
    # original_layout = init_vals[1]
    original_layout = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 15]

    # URL of the puzzle completion page
    fifteen_solved = 'fYNkLnTkjngs4TfYd0r9ZSZAiquFnmFN'


    #
    # Flask server
    #

    app = Flask(__name__, template_folder='./templates')
    app.config.from_mapping(
        SECRET_KEY='dev',
        DATABASE=os.path.join(app.instance_path, 'server.sqlite')
    )

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

    @app.route('/fifteen')
    def start ():
        return render_template('fifteen_index.html',
                               moves=moves,
                               original_layout=original_layout,
                               page=fifteen_solved)

    @app.route(f'/{fifteen_solved}')
    def solved_puzzle ():
        return render_template('fifteen_solved.html')

    from . import db
    db.init_app(app)

    return app
