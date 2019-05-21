SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
SUBDIRS := $(wildcard */.)

all : all_objs all_dirs

clean: clean_objs clean_dirs

all_objs : $(OBJ)

.c.o:
	$(CC) $< $(CFLAGS) -c -o $@

clean_objs:
	rm -rf $(OBJ)

all_dirs:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f Makefile all; \
	done

clean_dirs:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f Makefile clean; \
	done

.PHONY: all clean all_dirs clean_dirs all_objs clean_objs