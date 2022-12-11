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

mkdoc: out
	$(CC) $(CFLAGS) -o $(OUT)/mkdoc $(SRC)/mkdoc.c

mkblog: out bdocs
	$(CC) $(CFLAGS) -o $(OUT)/mkblog $(SRC)/mkblog.c

tools: mkdoc mkblog

exeirus: tools
	$(OUT)/mkblog -p $(POSTS) -d $(BDOCS) -s $(SKELS)/post.html -i $(SKELS)/index.html
	$(OUT)/mkdoc -c $(CONFIG) -s $(BDOCS) -d $(BLOG)
	$(OUT)/mkdoc -c $(CONFIG) -s $(DOCS) -d $(HTDOCS)
