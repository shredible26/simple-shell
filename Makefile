# provide a makefile with a target called shell_jr that 
# creates the executable shell_jr
# Define compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Define the target executable
TARGET = shell_jr

# Define source and object files
SRC = shell_jr.c
OBJ = $(SRC:.c=.o)

# Default rule for making the target
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Rule for creating object files
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Rule for cleaning up the build environment
clean:
	rm -f $(TARGET) $(OBJ)

# Rules for testing your program with provided input files
test: $(TARGET)
	for i in {01..08}; do \
		./$(TARGET) < public$$i.in > public$$i.test_output; \
		diff -q public$$i.output public$$i.test_output; \
	done

# Mark 'clean' and 'test' as phony targets
.PHONY: clean test
