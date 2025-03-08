# Target to build the final executable
bin/DataCorruptor: obj/DataCorruptor.o obj/dx_utils.o
	gcc obj/DataCorruptor.o obj/dx_utils.o -o bin/DataCorruptor

# Rule for DataCorruptor.o
obj/DataCorruptor.o: src/DataCorruptor.c inc/DataCorruptor.h
	gcc -c src/DataCorruptor.c -Iinc -o obj/DataCorruptor.o

# Rule for dx_utils.o
obj/dx_utils.o: src/dx_utils.c inc/DataCorruptor.h
	gcc -c src/dx_utils.c -Iinc -o obj/dx_utils.o

# Rule to display a message (optional)
ola:
	@echo "Hello from the DataCorruptor Makefile!"

# Default target to build everything
all: bin/DataCorruptor

# Rule to clean up generated files
clean:
	rm -f obj/*.o bin/DataCorruptor
