CC = gcc
CFLAGS = -MMD -Wall -Wextra -pedantic -g

SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=obj/%.o)
DEP = $(SRC:%.c=obj/%.d)

EXE = out

$(EXE) : $(OBJ)
	$(CC) $^ -o $@

obj/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)

clean:
	rm -f $(OBJ) $(DEP) $(EXE)