CC := gcc
objects := build/main.o build/cmd.o build/dynarr.o build/utils.o

bin/cash: $(objects)
	$(CC) -fsanitize=address -std=c99 -Wall -Werror $^ -o bin/cash

$(objects): build/%.o: src/%.c
	$(CC) -c $^ -o $@

clean:
	rm $(objects)

DIRS := build bin
$(shell mkdir -p $(DIRS))
