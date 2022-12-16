CC = cc
CFLAGS = -O2
OUT = ./bin
SRC = ./src

POSTS = ./posts
BDOCS = ./bdocs
SKELS = ./skels
CONFIG = ./mkdoc.vars
HTDOCS = ./htdocs
BLOG = ./htdocs/blog
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

15_puzzle:
	g++ -std=c++11 -o $(OUT)/15_puzzle $(SRC)/15_puzzle/main.cpp $(SRC)/15_puzzle/puzzle_solver.cpp $(SRC)/board.cpp

exeirus: tools
	$(OUT)/mkblog -p $(POSTS) -d $(BDOCS) -s $(SKELS)/post.html -i $(SKELS)/index.html
	$(OUT)/mkdoc -c $(CONFIG) -s $(BDOCS) -d $(BLOG)
	$(OUT)/mkdoc -c $(CONFIG) -s $(DOCS) -d $(HTDOCS)
