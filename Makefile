CC 		= gcc
RM      	= rm -f
DOT		= dot

CFLAGS 		= -W -Wall -Wextra -ansi -pedantic -D_BSD_SOURCE -std=c99

# Unpacker
UNSTUNTS_DIR 	= unstunts
UNSTUNTS_SRC	= $(UNSTUNTS_DIR)/unpack.c
UNSTUNTS_NAME 	= unstunts
UNSTUNTS_OBJ 	= $(UNSTUNTS_SRC:.c=.o)

# Decompressor
UNCOMP_DIR 	= uncomp
UNCOMP_SRC 	= 	$(UNCOMP_DIR)/main.c 	\
 			$(UNCOMP_DIR)/uncomp.c 	\
			$(UNCOMP_DIR)/tree.c
UNCOMP_NAME 	= uncomp
UNCOMP_OBJ 	= $(UNCOMP_SRC:.c=.o)

# Compressor
COMP_DIR	= comp
COMP_SRC	=	$(COMP_DIR)/main.c	\
			$(COMP_DIR)/tree.c	\
			$(COMP_DIR)/pqueue.c
COMP_NAME	= comp
COMP_OBJ	= $(COMP_SRC:.c=.o)

# Stats / all
STATS_DIR	= stats
STATS_SRC	=	$(STATS_DIR)/main.c	\
			$(STATS_DIR)/file.c
STATS_NAME	= stats
STATS_OBJ	= $(STATS_SRC:.c=.o)

# MakeRealGame
MAKEGAME_DIR	= makegame
MAKEGAME_SRC	=	$(MAKEGAME_DIR)/main.c	\
			$(MAKEGAME_DIR)/file.c	\
			$(MAKEGAME_DIR)/arg.c	\
 			$(UNCOMP_DIR)/uncomp.c 	\
			$(UNCOMP_DIR)/tree.c

MAKEGAME_NAME	= makegame
MAKEGAME_OBJ	= $(MAKEGAME_SRC:.c=.o)

BIN_DIR		= bin

all: $(UNSTUNTS_NAME) $(UNCOMP_NAME) $(COMP_NAME) $(STATS_NAME) $(MAKEGAME_NAME)

$(UNSTUNTS_NAME): $(BIN_DIR) $(UNSTUNTS_OBJ)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(UNSTUNTS_NAME) $(UNSTUNTS_OBJ)

$(UNCOMP_NAME): $(BIN_DIR) $(UNCOMP_OBJ)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(UNCOMP_NAME) $(UNCOMP_OBJ)

$(COMP_NAME): $(BIN_DIR) $(COMP_OBJ)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(COMP_NAME) $(COMP_OBJ)

$(STATS_NAME): $(BIN_DIR) $(STATS_OBJ)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(STATS_NAME) $(STATS_OBJ)

$(MAKEGAME_NAME): $(BIN_DIR) $(MAKEGAME_OBJ)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(MAKEGAME_NAME) $(MAKEGAME_OBJ)

$(BIN_DIR):
	mkdir bin

clean:
	$(RM) $(UNSTUNTS_OBJ)
	$(RM) $(UNCOMP_OBJ)
	$(RM) $(COMP_OBJ)
	$(RM) $(STATS_OBJ)
	$(RM) $(MAKEGAME_OBJ)

genstats: $(STATS_NAME)
	$(BIN_DIR)/$(STATS_NAME) /tmp/4DSD/
	markdown stats.md > index.html

distclean:  clean
	$(RM) -r $(BIN_DIR)

.PHONY:  clean all distclean debug
