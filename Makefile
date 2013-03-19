CC 		= gcc
CFLAGS 		= -W -Wall -Wextra -ansi -pedantic -ggdb
CFLAGS 		+= -std=c99
RM      	= rm -f

# Unpacker
UNSTUNTS_DIR 	= unstunts
UNSTUNTS_SRC	= $(UNSTUNTS_DIR)/unpack.c
UNSTUNTS_NAME 	= unstunts
UNSTUNTS_OBJ 	= $(UNSTUNTS_SRC:.c=.o)

# Decompressor
UNCOMP_DIR 	= uncomp
UNCOMP_SRC 	= 	$(UNCOMP_DIR)/main.c 	\
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

BIN_DIR		= bin

all: $(UNSTUNTS_NAME) $(UNCOMP_NAME) $(COMP_NAME)

$(UNSTUNTS_NAME): $(BIN_DIR) $(UNSTUNTS_OBJ)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(UNSTUNTS_NAME) $(UNSTUNTS_OBJ)

$(UNCOMP_NAME): $(BIN_DIR) $(UNCOMP_OBJ)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(UNCOMP_NAME) $(UNCOMP_OBJ)

$(COMP_NAME): $(BIN_DIR) $(COMP_OBJ)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(COMP_NAME) $(COMP_OBJ)

$(BIN_DIR):
	mkdir bin

clean:
	$(RM) $(UNSTUNTS_OBJ)
	$(RM) $(UNCOMP_OBJ)
	$(RM) $(COMP_OBJ)

distclean:  clean
	$(RM) -r $(BIN_DIR)

.PHONY:  clean all distclean
