CC = cc
CFLAGS = -O2

FLASK_SERVER  = ./flask_server
STATIC_SERVER = ./static_server

STATIC_HTDOCS = $(STATIC_SERVER)/htdocs
STATIC_BLOG   = $(STATIC_HTDOCS)/journal
STATIC_TELE   = $(STATIC_HTDOCS)/noustele
STATIC_ANN    = $(STATIC_HTDOCS)/announcements


#
#	Recipes for the static server
#

strgen:
	mkdir -p $(STATIC_SERVER)/bin	# Ensure that folder for binaries exists in STATIC_SERVER
	$(CC) $(CFLAGS) -o $(STATIC_SERVER)/bin/strgen $(STATIC_SERVER)/src/strgen.c	# Compile strgen program

strcon:
	mkdir -p $(STATIC_SERVER)/bin	# Ensure that folder for binaries exists in STATIC_SERVER
	$(CC) $(CFLAGS) -o $(STATIC_SERVER)/bin/strcon $(STATIC_SERVER)/src/strcon.c	# Compile strcon program

mkdoc:
	mkdir -p $(STATIC_SERVER)/bin	# Ensure that folder for binaries exists in STATIC_SERVER
	$(CC) $(CFLAGS) -o $(STATIC_SERVER)/bin/mkdoc $(STATIC_SERVER)/src/mkdoc.c	# Compile mkdoc program

mkblog:
	mkdir -p $(STATIC_SERVER)/bin	# Ensure that folder for binaries exists in STATIC_SERVER
	$(CC) $(CFLAGS) -o $(STATIC_SERVER)/bin/mkblog $(STATIC_SERVER)/src/mkblog.c	# Compile mkblog program

static_blog: mkblog mkdoc
	mkdir -p $(STATIC_SERVER)/blog/docs	# Ensure that directory for blog mkdoc files exists
	# Compile blog documents for parsing through mkdoc
	$(STATIC_SERVER)/bin/mkblog \
		-p $(STATIC_SERVER)/blog/posts \
		-d $(STATIC_SERVER)/blog/docs \
		-s $(STATIC_SERVER)/blog/skels/post.html \
		-i $(STATIC_SERVER)/blog/skels/index.html
	# Parse blog documents through mkdoc and write to htdocs
	$(STATIC_SERVER)/bin/mkdoc \
		-c $(STATIC_SERVER)/mkdoc.vars \
		-s $(STATIC_SERVER)/blog/docs \
		-d $(STATIC_BLOG)

static_tele: mkblog mkdoc
	mkdir -p $(STATIC_SERVER)/tele/docs	# Ensure that directory for tele mkdoc files exists
	# Compile tele documents for parsing through mkdoc
	$(STATIC_SERVER)/bin/mkblog \
		-p $(STATIC_SERVER)/tele/posts \
		-d $(STATIC_SERVER)/tele/docs \
		-s $(STATIC_SERVER)/tele/skels/post.html \
		-i $(STATIC_SERVER)/tele/skels/index.html
	# Parse tele documents through mkdoc and write to htdocs
	$(STATIC_SERVER)/bin/mkdoc \
		-c $(STATIC_SERVER)/mkdoc.vars \
		-s $(STATIC_SERVER)/tele/docs \
		-d $(STATIC_TELE)

static_ann: mkblog mkdoc
	mkdir -p $(STATIC_SERVER)/ann/docs	# Ensure that directory for ann mkdoc files exists
	# Compile ann documents for parsing through mkdoc
	$(STATIC_SERVER)/bin/mkblog \
		-p $(STATIC_SERVER)/ann/posts \
		-d $(STATIC_SERVER)/ann/docs \
		-s $(STATIC_SERVER)/ann/skels/post.html \
		-i $(STATIC_SERVER)/ann/skels/index.html
	# Parse ann documents through mkdoc and write to htdocs
	$(STATIC_SERVER)/bin/mkdoc \
		-c $(STATIC_SERVER)/mkdoc.vars \
		-s $(STATIC_SERVER)/ann/docs \
		-d $(STATIC_ANN)



static_server: static_blog static_tele


#
#	Recipes for the flask server
#

15_puzzle:
	mkdir -p $(FLASK_SERVER)/bin	# Ensure that folder for binaries exists in FLASK_SERVER
	# Compile 15_puzzle board-generator program
	g++ -std=c++11 -o $(FLASK_SERVER)/bin/15_puzzle \
		$(FLASK_SERVER)/src/15_puzzle/board.cpp \
		$(FLASK_SERVER)/src/15_puzzle/puzzle_solver.cpp \
		$(FLASK_SERVER)/src/15_puzzle/main.cpp

sudoku:
	mkdir -p $(FLASK_SERVER)/bin    # Ensure that folder for binaries exists in FLASK_SERVER
	# Compile sudoku generator program
	g++ -std=c++11 -o $(FLASK_SERVER)/bin/sdk \
		$(FLASK_SERVER)/src/sudoku/main.cpp   \
		$(FLASK_SERVER)/src/sudoku/sudoku.cpp

brainheck:
	mkdir -p $(FLASK_SERVER)/bin	# Ensure that folder for binaries exists in FLASK_SERVER
	$(CC) $(CFLAGS) -o $(FLASK_SERVER)/bin/brainheck $(FLASK_SERVER)/src/brainheck/brainheck.c	# Compile brainheck program

hamming:
	mkdir -p $(FLASK_SERVER)/bin	# Ensure that folder for binaries exists in FLASK_SERVER
	$(CC) $(CFLAGS) -o $(FLASK_SERVER)/bin/hamming $(FLASK_SERVER)/src/hamming.c	# Compile hamming program

flask_server: 15_puzzle sudoku brainheck hamming

#
#	General recipes
#

# Compile the exeirus project
exeirus: static_server flask_server
