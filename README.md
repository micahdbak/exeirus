# EXEIRUS

EXEIRUS is an ARG (Alternative Reality Game) created for the Simon Fraser University's Computing Science Student Society.

The game consists of puzzles that must be completed in sequential order, each puzzle providing a key. When every puzzle is completed, these keys can be combined to create a master-key. This master-key is part of the URL of the completion page. Finding this completion page ends the game, and thus whoever gets there first is declared the winner.

## Installing

Several programs assist in the repetitive aspects of this project.

To compile all of these programs, simply run ``make tools``. The executables of these tools will now be in ``bin`` for later use.

## Tools

### The ``mkdoc`` program

The ``mkdoc`` program reads HTML files (or really any file), replacing certain fields with constant variables.

This is an important program if, per-say, you intend to install EXEIRUS on a web-server with a domain-name other than my own. In this case, the static HTML files created for the purpose of running on https://exeirus.micahbaker.xyz will not work properly with your domain. Without this program, you would need to change every occurrence of my domain in every file, which would be tedious. Instead, ``mkdoc`` with a configuration file declaring the domain that this server is to run on willreplace every occurence of ``$DOMAIN$`` with the configured domain.

The ``mkdoc`` program takes three options:

``mkdoc -c [configuration-file] -s [source-directory] -d [destination-directory]``

The configuration file should have the following syntax:

```
KEY1: VALUE1
KEY2: VALUE2
KEY3: VALUE3
```

Where every character from the beginning of a line to a colon is a key, and every character following the white-space after the colon to the end of the line is a value.

A file in the source directory will have key-fields replaced with the values declared in the configuration file. These key-fields begin when a dollar symbol is found and terminated when another dollar symbol is found. Two dollar symbols in sequence will be processed as a single dollar symbol.

For example, suppose that we want the key 'TITLE' to have the value 'EXEIRUS'.

Our configuration file would look like this:

```
TITLE: EXEIRUS
```

Then, an example HTML source file using this key would look like this:

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

Which would then be processed as:

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

``mkdoc`` will read every file in the source directory as a source file. Each file will be output to the destination directory with the same filename, but processed with respect to the key-value pairs defined in the configuration file.

### The ``mkblog`` program

The ``mkblog`` program reads a folder of post files and creates a static blog web-site with reference to HTML file 'skeletons'.
