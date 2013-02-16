#
# Makefile
#
# Programmer: Josh Inscoe
# Date: 2/14/13
#
# Theseus and the Minotaur Game
#

# Compiler to use
CC = gcc

# Flags to pass to the compiler
CFLAGS = -std=c99 -O3

# Name for executable program
EXE = theseus

# List of header files
HDRS = loader.h board.h movement.h game.h welcome.h

# Libraries to link to when compiling
LIBS = -lncurses

# List of source files
SRCS = loader.c board.c movement.c game.c welcome.c main.c

# An automatically generated list of object files
OBJS = $(SRCS:.c=.o)

# Default target
$(EXE): $(OBJS) $(HDRS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# Dependencies (Object files)
$(OBJS): $(HDRS) Makefile

# Target to clean up after compiling the default target
clean:
	rm -f core *.o
