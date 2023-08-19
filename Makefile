# Code with help from chatGPT

# Compiler
CC = gcc

# Compiler flags
# CFLAGS = -Wall -Wextra -g
CFLAGS = -Wall -g

# Source files
# SRCS = main.c functions.c // mjc follow this template for adding files
SRCS = blackjack.c customBMP.c

# Object files
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = blackjack

# Check the operating system
UNAME := $(shell uname)

# Default library flags for Linux
LIBS = -lGL -lGLU -lglut

# Override library flags for macOS
ifeq ($(UNAME), Darwin)
    LIBS = -framework OpenGL -framework GLUT
endif

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o $(TARGET) $(OBJS)

# Rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Phony rule to build all targets
all: $(TARGET)

# Phony rule to clean up the generated files
clean:
	rm -f $(OBJS) $(TARGET)
