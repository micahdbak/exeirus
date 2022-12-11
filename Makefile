CC = cc
CFLAGS = -O2
OUT = ./bin

out:
	mkdir -p $(OUT)

mkdoc: out
	rm -f $(OUT)/mkdoc
	$(CC) $(CFALGS) -o $(OUT)/mkdoc ./src/mkdoc.c

tools: mkdoc
