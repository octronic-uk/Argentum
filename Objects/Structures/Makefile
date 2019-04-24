SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all : $(OBJ)

$(OBJ) : $(SRC)
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -rf $(OBJ)