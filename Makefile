CC 		= gcc
CFLAGS 		= -W -Wall -Wextra -ansi -pedantic -ggdb
CFLAGS 		+= -std=c99
RM      	= rm -f

# Unpacker
UNSTUNTS_DIR 	= un_stunts
UNSTUNTS_SRC	= $(UNSTUNTS_DIR)/unpack.c
UNSTUNTS_NAME 	= un_stunts_bin
UNSTUNTS_OBJ 	= $(UNSTUNTS_SRC:.c=.o)

# Decompressor
UNCOMP_DIR 	= uncomp
UNCOMP_SRC 	= 	$(UNCOMP_DIR)/main.c 	\
			$(UNCOMP_DIR)/tree.c
UNCOMP_NAME 	= uncomp_bin
UNCOMP_OBJ 	= $(UNCOMP_SRC:.c=.o)

# Compressor
COMP_DIR	= comp
COMP_SRC	=	$(COMP_DIR)/main.c	\
			$(COMP_DIR)/tree.c	\
			$(COMP_DIR)/pqueue.c
COMP_NAME	= comp_bin
COMP_OBJ	= $(COMP_SRC:.c=.o)


all: $(UNSTUNTS_NAME) $(UNCOMP_NAME) $(COMP_NAME)

$(UNSTUNTS_NAME): $(UNSTUNTS_OBJ)
	$(CC) $(CFLAGS) -o $(UNSTUNTS_NAME) $(UNSTUNTS_OBJ)

$(UNCOMP_NAME): $(UNCOMP_OBJ)
	$(CC) $(CFLAGS) -o $(UNCOMP_NAME) $(UNCOMP_OBJ)

$(COMP_NAME): $(COMP_OBJ)
	$(CC) $(CFLAGS) -o $(COMP_NAME) $(COMP_OBJ)

clean:
	$(RM) $(UNSTUNTS_OBJ)
	$(RM) $(UNCOMP_OBJ)
	$(RM) $(COMP_OBJ)

distclean:  clean
	$(RM) $(UNSTUNTS_NAME)
	$(RM) $(UNCOMP_NAME)
	$(RM) $(COMP_NAME)

.PHONY:  clean all distclean
