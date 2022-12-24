CC = cc
CFLAGS = -O2
OUT = ./bin
SRC = ./src

BPOSTS = ./blog/posts
BDOCS  = ./blog/docs
BSKELS = ./blog/skels
BLOG   = ./htdocs/eq3Ci8oC

TPOSTS = ./tele/posts
TDOCS  = ./tele/docs
TSKELS = ./tele/skels
TELE   = ./htdocs/xIjNAryAY46kaD4h

CONFIG = ./mkdoc.vars
HTDOCS = ./htdocs
DOCS = ./docs

out:
	mkdir -p $(OUT)

bdocs:
	mkdir -p $(BDOCS)

tdocs:
	mkdir -p $(TDOCS)

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
	$(OUT)/mkblog -p $(BPOSTS) -d $(BDOCS) -s $(BSKELS)/post.html -i $(BSKELS)/index.html
	$(OUT)/mkdoc -c $(CONFIG) -s $(BDOCS) -d $(BLOG)

tele: mkblog tdocs
	$(OUT)/mkblog -p $(TPOSTS) -d $(TDOCS) -s $(TSKELS)/post.html -i $(TSKELS)/index.html
	$(OUT)/mkdoc -c $(CONFIG) -s $(TDOCS) -d $(TELE)

exeirus: tools blog tele
	$(OUT)/mkdoc -c $(CONFIG) -s $(DOCS) -d $(HTDOCS)
