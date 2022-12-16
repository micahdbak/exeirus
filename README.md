# EXEIRUS

"This is a game. Complete it, and you will be rewarded.
You will come accross a great many puzzles.
By solving a puzzle, your solution will guide you to the next puzzle.
But do not rush, for every puzzle contains something else that is vital to this game's completion.
Keep an eye out for things that look awry, because there are keys hidden in every puzzle, in nearly every page. ..." (from index.html)

EXEIRUS is an ARG (Alternative Reality Game) created for the Simon Fraser University's Computing Science Student Society.

The game consists of puzzles that must be completed in sequential order, each puzzle providing a key.
When every puzzle is completed, these keys can be combined to create a master-key.
This master-key is part of the URL of the completion page.
Finding this completion page ends the game, and thus whoever gets there first is declared the winner.

## Installing

Several programs assist in the repetitive aspects of this project.

To compile all of these programs, simply run ``make tools``.
The executables of these tools will now be in ``bin`` for later use.

To compile the HTML file tree of EXEIRUS, run ``make exeirus``.
The file tree will now be present under the ``htdocs`` folder.

## Tools

### The ``mkdoc`` program

#### Description

The ``mkdoc`` program reads HTML files (or really any file), replacing certain fields with constant variables.

This is an important program if, per-say, you intend to install EXEIRUS on a web-server with a domain-name other than mine.
In this case, without ``mkdoc``, the static HTML files created for the purpose of running on my server will not work properly on yours.
You would need to change every occurrence of my domain in every file, which would be tedious.
Instead, ``mkdoc`` looks to a configuration file that defines the domain that this server is to run on.
It replaces every occurence of ``$DOMAIN$`` with the configured domain.
It is likewise for anything else that you choose to define in the configuration file.

The configuration file should have the following syntax:

```
KEY1: VALUE1
KEY2: VALUE2
KEY3: VALUE3
```

Where every character from the beginning of a line to a colon is a key, and every character following the white-space after the colon to the end of the line is a value.

Given some source file, ``mkdoc`` will parse every character.
So long as ``mkdoc`` does not find a dollar symbol, it will copy characters from the source file to the output file without changing anything.
But anything inbetween dollar symbols will be treated as a key.
``mkdoc`` will attempt to find the value for that key with respect to the configuration file, and write that value to the output file.
For example, with the above key-value pairs, if ``mkdoc`` finds ``$KEY1$`` in some file, then it will write ``VALUE1`` to the output file where ``$KEY1$`` originally was.
Because the dollar symbol is used as an escape character, you must be careful if you intend to use a dollar symbol normally.
To have a single dollar symbol in the output file, ensure that there are two dollar symbols in sequence in the source file.

#### Usage

When you run ``mkdoc``, you must provide three options:

``mkdoc -c [configuration-file] -s [source-directory] -d [destination-directory]``

The ``-c`` option tells ``mkdoc`` where the configuration file is.
``-s`` specifies the directory where source files exist.
``-d`` specifies the directory where output files are to be written.
Every file in the source directory will be parsed by ``mkdoc`` and copied with the same filename to the destination directory.

For example, suppose that we want the key 'TITLE' to have the value 'EXEIRUS'.

Our configuration file, ``mkdoc.vars``, would look like this:

```
TITLE: EXEIRUS
```

Then, an example HTML source file ``example.html`` in the directory ``source-files`` that uses this key would look like this:

```html
<!doctype html>
<html>
<head>
    <title>$TITLE$</title>
</head>
<body>
    <p>I just found $$100!</p>
</body>
</html>
```

If we create a directory ``output-files``, then we can run ``mkdoc`` with:

``mkdoc -c ./mkdoc.vars -s source-files -d output-files``

Which would give us the file ``example.html`` in the directory ``output-files`` as:

```html
<!doctype html>
<html>
<head>
    <title>EXEIRUS</title>
</head>
<body>
    <p>I just found $100!</p>
</body>
</html>
```

### The ``mkblog`` program

#### Description

The ``mkblog`` program reads a folder of post files and creates a static blog web-site with reference to HTML file 'skeletons'.

A post has four different parts.
It has a title, author, date, and content.
A post file reflects these four parts.
The first line of a post file is its title.
The second, its author.
The third, the date that it was written.
Every character proceeding the third line is the post's content.
The content of a post is written in a special syntax that is called blog-script, for lack of a better name.
Blog-script has the capability to easily define line-breaks, paragraph-breaks, and italicized, bolded, striked-through, underlined, or monospace text, and hyperlinks.

The following describes the different escape sequences for text effects:

- ``\t``    tab
- ``\n``    line-break
- ``\P``    paragraph-break
- ``\(i``   start italicized text
- ``\)i``   end italicized text
- ``\(b``   start bolded text
- ``\)b``   end bolded text
- ``\(s``   start striked-through text
- ``\)s``   end striked-through text
- ``\(u``   start underlined text
- ``\)u``   end underlined text
- ``\(m``   start monospace text
- ``\)m``   end monospace text
- ``\(a``   start hyperlink
- ``\)a``   end hyperlink

When ``mkblog`` is run, it runs through a provided posts directory with a number of post files.
Every post file will be written to a provided destination directory with respect to a post skeleton file.
This post skeleton file is the file that dictates how every post is to look when it is compiled by ``mkblog``.
``mkblog`` will read characters from this skeleton file and write them to the output file until it comes across a '#', the escape character for ``mkblog``.
Escape sequences using '#' tell ``mkblog`` where it should write parts of the post to the output file.
For example,

- ``#t``    writes the title of the post,
- ``#a``    writes the author of the post,
- ``#d``    writes the date of the post,
- ``#c``    writes the content of the post,
- ``#f``    writes the filename of the post.

For example, suppose we have the following post:

```
My First Post
Micah Baker
11 Dec 2022
Hello, world!
\P
This is my very first post.
I do hope that you like it!
Here is some \(ifancy\)i \(btext\)b.
\P
Goodbye!
```

Then ``mkblog`` will gather that the title is "My First Post", the author is "Micah Baker", the date is "11 Dec 2022", and the content is the rest of the file.
If we give ``mkblog`` the following post skeleton file:

```html
<!doctype html>
<html>
<head>
    <title>#t</title>
</head>
<body>
    <h1>#t</h1>
    <h2>By #a, on #d</h2>
    <p>#c</p>
</body>
</html>
```

Then ``mkblog`` will compile the post file with respect to the post skeleton file to create the following:

```html
<!doctype html>
<html>
<head>
    <title>My First Post</title>
</head>
<body>
    <h1>My First Post</h1>
    <h2>By Micah Baker, on 11 Dec 2022</h2>
    <p>Hello, world!</p>
<p>This is my very first post.
I do hope that you like it!
Here is some <i>fancy</i> <b>text</b>.</p>
<p>Goodbye!</p>
</body>
</html>
```

This is great and all, but how will the user keep track of all of these outputted files?
This is where the index skeleton comes into play.
The index skeleton file has the same escape sequences as the post skeleton file, minus ``#c`` which gives a post's content.
The index skeleton file has two special escape sequences that must be used correctly.
``mkblog`` keeps track of every post that it has read and written
After it has parsed every post, ``mkblog`` creates the index file which lists every post it has read.
``mkblog`` does this by reading characters from the index skeleton, copying them to the output index file, until it reaches the escape character '#'.
The first escape sequence it should come across is ``#(``, which starts the post-listing skeleton.
The post-listing skeleton will be repeated for every post that ``mkblog`` has read, with respect to each post's information.
The post-listing skeleton is terminated with ``#)``.
Within a post-listing, each of the escape sequences that were defined for post skeletons can be used once more, except for ``#c`` which gives a post's content.

For example, suppose that ``mkblog`` has read the post previously defined.
Let that post have the filename of ``post1``.
Then suppose that we have another post with the filename of ``post2``, and it is defined as follows:

```
My Second Post
Micah Baker
11 Dec 2022
This is my second post.
\P
I don't have much more to say other than that!
```
My First Post
Micah Baker
11 Dec 2022

If we give ``mkblog`` the following index skeleton file:

```html
<!doctype html>
<html>
<head>
    <title>My Blog</title>
</head>
<body>
    <p>Welcome to My Blog.</p>
    <p>The following are posts that are available.</p>
#(
    <p><b>#t</b> by #a on #d. <a href="$DOMAIN$/#f">View Post</a></p>#)
</body>
</html>
```

Then ``mkblog`` will output the following index file with respect to the posts that it has read:

```html
<!doctype html>
<html>
<head>
    <title>My Blog</title>
</head>
<body>
    <p>Welcome to My Blog.</p>
    <p>The following are posts that are available.</p>

    <p><b>My First Post</b> by Micah Baker on 11 Dec 2022. <a href="$DOMAIN$/post1">View Post</a></p>
    <p><b>My Second Post</b> by Micah Baker on 11 Dec 2022. <a href="$DOMAIN$/post2">View Post</a></p>
</body>
</html>
```

Notice the reoccurence of ``$DOMAIN$`` from the index skeleton file to the output index file.
This is because the output files from ``mkblog`` can be used as source files for the ``mkdoc`` program, which can ensure consistent writing of certain information, such as the domain name of the server that this blog is to run on.

#### Usage

Considering the explanation above, ``mkblog`` needs 4 things from the user:
the posts directory, the destination directory, the post skeleton file, and the index skeleton file.
These can be provided to ``mkblog`` by the following command:

``mkblog -p [posts-directory] -d [destination-directory] -s [post-skeleton] -i [index-skeleton]``

### 15 Puzzle

#### Description

This 15-puzzle solver does NOT check for whether the puzzle is solvable or not. 
It assumes it is and will be stuck in an infinite loop if it is passed an unsolvable puzzle

#### Usage

1. Declare random seed
2. Create the board either a random one or by passing 'int arr[4][4]' into the constructor with needed values. 0 represents blank tile
3. Create an instance of the solver class, and pass the board.
4. Done, board is solved.
5. main.cpp contains function to save the board to the .txt file in the format (where initial board is represented as 1d string):

```
$moves
$string_representation_of_board
```

Here is an example program:

```c++
int main () {
    srand(time(0));

    Board* brd = new Board();

    Solver* solv = new Solver (brd);
    save_to_file (solv, brd);
	    
    delete brd;
    delete solv;

    return 0;
}
```

#### API (for users):

Board:
	``void shuffle ()``            - shuffles the board
	``bool equals (Board* x)``     - checks if 2 boards are equal (same elements at the same positions)
	``void print_board ()``        - prints the board to the terminal as 2d array. 0 represents blank tile
	``string to_str ()``           - returns 1d string representation of the board
	``vector<Board*> neighbors()`` - returns *neighbors* of the current board, i.e. when blank tile is moved once in possible directions

Solver:
	``int get_moves ()``           - returns the minimum amount of moves needed to solve the board that was passed to it
	``void print_solution ()``     - prints out all iterations of the board in solution steps. WARNING - can flood your terminal
