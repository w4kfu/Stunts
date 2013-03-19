RM      = rm -f
CC 	= gcc
CFLAGS = -W -Wall -Wextra -ansi -pedantic
CFLAGS += -std=c99
UNSTUNTS_DIR = un_stunts
UNSTUNTS_SRC	= $(UNSTUNTS_DIR)/unpack.c
UNSTUNTS_NAME = un_stunts_bin

UNCOMP_DIR = uncomp
UNCOMP_SRC = $(UNCOMP_DIR)/main.c \
		$(UNCOMP_DIR)/tree.c
UNCOMP_NAME = uncomp_bin


UNSTUNTS_OBJ = $(UNSTUNTS_SRC:.c=.o)
UNCOMP_OBJ = $(UNCOMP_SRC:.c=.o)

all:	$(UNSTUNTS_NAME)	$(UNCOMP_NAME)

$(UNSTUNTS_NAME): $(UNSTUNTS_OBJ)
	 $(CC) $(CFLAGS) -o $(UNSTUNTS_NAME) $(UNSTUNTS_OBJ)

$(UNCOMP_NAME): $(UNCOMP_OBJ)
	 $(CC) $(CFLAGS) -o $(UNCOMP_NAME) $(UNCOMP_OBJ)


clean:
	 $(RM) $(UNSTUNTS_OBJ)
	 $(RM) $(UNCOMP_OBJ)

distclean:  clean
	 $(RM) $(UNSTUNTS_NAME)
	 $(RM) $(UNCOMP_NAME)

.PHONY:  clean all distclean
