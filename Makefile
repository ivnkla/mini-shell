.PHONY: all debug clean

# Disable implicit rules
.SUFFIXES:

CC = gcc
CFLAGS = -Wall -g
DEBUG_FLAGS = -DDEBUG
VPATH = src/

# Note: -lnsl does not seem to work on Mac OS but will
# probably be necessary on Solaris for linking network-related functions
#LIBS += -lsocket -lnsl -lrt
LIBS += -lpthread

INCLUDE = readcmd.h csapp.h
OBJS = readcmd.o csapp.o
INCLDIR = -I.

# Main target
all: shell

# Debug target - must be before the rule for 'all' since it adds to CFLAGS
debug: CFLAGS += $(DEBUG_FLAGS)
debug: shell

# Pattern rule for object files
%.o: %.c $(INCLUDE)
	$(CC) $(CFLAGS) $(INCLDIR) -c -o $@ $<

# Rule for the shell executable
shell: shell.o $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)

clean:
	rm -f shell *.o
