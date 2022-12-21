CC = cc
CFLAGS = -O2
OUT = ./bin
SRC = ./src

POSTS = ./blog/posts
BDOCS = ./blog/docs
SKELS = ./blog/skels
CONFIG = ./mkdoc.vars
HTDOCS = ./htdocs
BLOG = ./htdocs/eq3Ci8oC
DOCS = ./docs

out:
	mkdir -p $(OUT)

bdocs:
	mkdir -p $(BDOCS)

strgen: out
	$(CC) $(CFLAGS) -o $(OUT)/strgen $(SRC)/strgen.c

mkdoc: out
	$(CC) $(CFLAGS) -o $(OUT)/mkdoc $(SRC)/mkdoc.c

mkblog: out bdocs
	$(CC) $(CFLAGS) -o $(OUT)/mkblog $(SRC)/mkblog.c

tools: strgen mkdoc mkblog

15_puzzle: out
	g++ -std=c++11 -o $(OUT)/15_puzzle $(SRC)/15_puzzle/main.cpp $(SRC)/15_puzzle/puzzle_solver.cpp $(SRC)/15_puzzle/board.cpp

blog: mkblog bdocs
	$(OUT)/mkblog -p $(POSTS) -d $(BDOCS) -s $(SKELS)/post.html -i $(SKELS)/index.html

exeirus: tools blog
	$(OUT)/mkdoc -c $(CONFIG) -s $(BDOCS) -d $(BLOG)
	$(OUT)/mkdoc -c $(CONFIG) -s $(DOCS) -d $(HTDOCS)
