import os
from flask import Flask, render_template, request

def create_app (config=None):

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

    from . import fifteen

    fifteen.fifteen(app)

    #
    # Database
    #

    from . import db
    db.init_app(app)

    return app

if __name__ == '__main__':
    create_app().run()