CC := gcc
objects := build/main.o build/cmd.o build/dynarr.o build/utils.o
debug_objects := build/main-debug.o build/cmd-debug.o build/dynarr-debug.o build/utils-debug.o

bin/cash: $(objects)
	$(CC) -fsanitize=address -std=c99 -Wall -Werror $^ -o bin/cash

$(objects): build/%.o: src/%.c
	$(CC) -c $^ -o $@


bin/debug: $(debug_objects)
	$(CC) -g -fsanitize=address -std=c99 -Wall -Werror $^ -o bin/debug 

$(debug_objects): build/%-debug.o: src/%.c
	$(CC) -g -c $^ -o $@

clean:
	rm $(objects)

DIRS := build bin
$(shell mkdir -p $(DIRS))
