# Target to build the final executable
bin/DataReader: obj/DataReader.o
	gcc obj/DataReader.o -o bin/DataReader

# Rule for DataReader.o
obj/DataReader.o: src/DataReader.c inc/drcommon.h
	gcc -c src/DataReader.c -Iinc -o obj/DataReader.o

# Rule to display a message (optional)
ola:
	@echo "Hello from the DataReader Makefile!"

# Default target to build everything
all: bin/DataReader

# Rule to clean up generated files
clean:
	rm -f obj/*.o bin/DataReader
