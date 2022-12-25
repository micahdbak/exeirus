CC = cc
CFLAGS = -O2

FLASK_SERVER  = ./flask_server
STATIC_SERVER = ./static_server

STATIC_HTDOCS = $(STATIC_SERVER)/htdocs
STATIC_BLOG = $(STATIC_HTDOCS)/eq3Ci8oC
STATIC_TELE = $(STATIC_HTDOCS)/xIjNAryAY46kaD4h


#
#	Recipes for the static server
#

strgen:
	mkdir -p $(STATIC_SERVER)/bin	# Ensure that folder for binaries exists in STATIC_SERVER
	$(CC) $(CFLAGS) -o $(STATIC_SERVER)/bin/strgen $(STATIC_SERVER)/src/strgen.c	# Compile strgen program

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

static_server: static_blog static_tele
	# Parse remaining documents through mkdoc and write to htdocs
	$(STATIC_SERVER)/bin/mkdoc \
		-c $(STATIC_SERVER)/mkdoc.vars \
		-s $(STATIC_SERVER)/docs \
		-d $(STATIC_HTDOCS)


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

#
#	General recipes
#

# Compile the exeirus project
exeirus: static_server flask_server
