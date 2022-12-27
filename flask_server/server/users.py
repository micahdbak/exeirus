from flask import Blueprint, flash, g, redirect, render_template, request, url_for
from server.db import get_db

PART1_PASSWORD = 'secret password'

bp = Blueprint('users', __name__, url_prefix='/users')

@bp.route('/leaderboard', methods = ['GET'])
def leaderboard ():
    db = get_db()
    users = db.execute('SELECT id, name FROM user')

    return render_template('leaderboard.html',
                           users=users,
                           URL_create=url_for('users.create'))

@bp.route('/create', methods = ['GET', 'POST'])
def create ():
    if request.method == 'POST':
        name = request.form['name']
        
        db = get_db()

        error = None

        if not name:
            error = 'Username is required.'

        if error is None:
            try:
                db.execute(
                    "INSERT INTO user (name) VALUES (?)",
                    (name,)
                ).fetchone()
                db.commit()
            except db.IntegrityError:
                error = f"User {name} is already registered."
            else:
                return redirect(url_for('users.leaderboard'))

        if error is not None:
            return render_template('error.html', error=error)

    return render_template('create.html')

@bp.route('/part1', methods = ['GET', 'POST'])
def part1 ():
    if request.method == 'POST':
        name = request.form['name']
        password = request.form['password']

        db = get_db()
        
        error = None

        if not name:
            error = 'Username is required.'

        if error is None and password == PART1_PASSWORD:
            db.execute(
                "UPDATE user SET part1 = 1 WHERE name = ?",
                (name,)
            ).fetchone()

            return render_template('part1.html', msg='attempted.')

        return redirect(url_for('users.leaderboard'))

    return render_template('part1.html', msg='enter your password.')
