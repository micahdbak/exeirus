from flask import Blueprint, render_template, request, redirect, url_for, session

MAX_USERS = 100
MAX_USERNAME = 16

# Please change this
ADMIN_PASSWORD = '1234'

allow_create = True

bp = Blueprint('users', __name__, url_prefix='/users')

class User:
    def __init__(self, username):
        self.username = username
        self.completed = dict()

    def score(self):
        # Return value
        r = 0

        for i in self.completed:
            r += i

        return r

users = list()

# djb2 hashing algorithm
# http://www.cse.yorku.ca/~oz/hash.html
def djb2(name):
    val = 5381

    for c in name:
        val = ((val << 5) + val) + ord(c)

    # hexadecimal from 0..65536 has 4 characters
    return hex(val % 65536)

@bp.route('/leaderboard', methods = ['GET'])
def leaderboard():
    # users.sort(key=lambda u: u.score())

    return render_template('users_leaderboard.html',
                           users=users)

@bp.route('/create', methods = ['GET', 'POST'])
def create():
    message = None

    if allow_create == False:
        message = 'The period for registering has passed.'

    if request.method == 'POST' and allow_create == True:
        username = request.form['username']

        if not username:
            message = 'A username is required.'
        elif len(username) > MAX_USERNAME:
                message = f'The inputted username exceeds the maximum length of {MAX_USERNAME} characters. Please enter a shorter username.'
        else:
            for user in users:
                if user.username == username:
                    message = f'The username {username} is taken.'

                    break

            # Add this user if the username is not taken
            if message is None:
                users.append(User(username))
                users[-1].hash = djb2(username)

                message = f'Created user {username}.'

    return render_template('users_create.html',
                           message=message)

@bp.route('/admin_login', methods = ['GET', 'POST'])
def admin_login():
    message = None

    if request.method == 'POST':
        pw = request.form['password']

        if pw == ADMIN_PASSWORD:
            session['admin'] = ADMIN_PASSWORD

            return redirect(url_for('users.admin'))
        else:
            message = 'Incorrect password.'

    return render_template('users_admin_login.html', message=message)

@bp.route('/admin', methods = ['GET', 'POST'])
def admin():
    ad = session.get('admin')

    if ad != ADMIN_PASSWORD:
        return redirect(url_for('users.admin_login'))

    # ad == ADMIN_PASSWORD

    if request.method == 'POST':
        if request.form['reset'] == 'yes':
            users.clear()

        if request.form['permit'] == 'yes':
            allow_create = True
        else:
            allow_create = True

        if request.form['logout'] == 'yes':
            session.pop('admin')

            return redirect(url_for('users.admin_login'))

    return render_template('users_admin.html')
