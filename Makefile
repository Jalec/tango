# Define the compiler
CC = gcc

# Define compiler flags
CFLAGS = -lSDL2 -lm

# Define the source files
SRCS = main.c render_figures.c

# Define the object files
OBJS = $(SRCS:.c=.o)

# Define the executable
TARGET = program

# Default target to build the executable
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

# Compile source files into object files
%.o: %.c
	$(CC) -c $< -o $@

# Clean target to remove generated files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony target for make
.PHONY: all clean

