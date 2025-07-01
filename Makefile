CC = gcc
CFLAGS = -MMD -Wall -Wextra -Iinclude -pedantic -g

SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=obj/%.o)
DEP = $(SRC:src/%.c=obj/%.d)

EXE = out

$(EXE) : $(OBJ)
	$(CC) $^ -o $@

obj/%.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)

clean:
	rm -f $(OBJ) $(DEP) $(EXE)