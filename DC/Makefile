# Target to build the final executable
bin/DataCreator: obj/DataCreator.o
	gcc obj/DataCreator.o -o bin/DataCreator

# Rule for DataCreator.o
obj/DataCreator.o: src/DataCreator.c inc/DataCreator.h
	gcc -c src/DataCreator.c -Iinc -o obj/DataCreator.o

# Rule to display a message (optional)
ola:
	@echo "Hello from the DataCreator Makefile!"

# Default target to build everything
all: bin/DataCreator

# Rule to clean up generated files
clean:
	rm -f obj/*.o bin/DataCreator
