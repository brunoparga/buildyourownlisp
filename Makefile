CC = gcc
CFLAGS = -g -W -Wall -ggdb3 -std=c99
LDFLAGS = -ledit -lm
COMPILE = $(CC) -c $(CFLAGS) $< -o $@

OBJECTS = src/main.c build/calc.o build/eval.o build/expressionizer.o build/list.o build/parser.o build/repl.o build/value.o build/mpc.o build/read_file.o
TARGET = build/lye

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/repl.o: src/repl.c src/repl.h build/parser.o
	$(COMPILE)

build/parser.o: src/parser.c src/parser.h build/eval.o build/value.o build/mpc.o
	$(COMPILE)

build/eval.o: src/eval.c src/eval.h build/calc.o build/list.o build/value.o
	$(COMPILE)

build/expressionizer.o: src/expressionizer.c src/expressionizer.h build/mpc.o build/value.o
	$(COMPILE)

build/mpc.o: lib/mpc.c lib/mpc.h
	$(COMPILE)

build/calc.o: src/calc.c src/calc.h build/value.o
	$(COMPILE)

build/list.o: src/list.c src/list.h build/value.o
	$(COMPILE)

build/value.o: src/value.c src/value.h
	$(COMPILE)

build/read_file.o: utils/read_file.c utils/read_file.h
	$(COMPILE)

test: $(TARGET)
	$(CC) $(CFLAGS) test/test.c -o build/test
	./build/test

# I might want to delete this is it interferes with gdb
.PHONY: clean

clean:
	@rm -f $(TARGET) $(OBJECTS) core