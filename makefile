CC=gcc
CFLAGS=-g3 -pg 
DEPS = DataTypes.h Types.h
OBJ = input.o check.o crossword.o DataTypes.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
crswrd: solve clean
solve: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
clean:
	rm  -f $(OBJ)