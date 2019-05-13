SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all : $(OBJ)

.c.o:
	$(CC) $< $(CFLAGS) -c -o $@

clean:
	rm -rf $(OBJ)