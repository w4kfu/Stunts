RM      = rm -f
CC 	= gcc
CFLAGS = -W -Wall -Wextra -ansi -pedantic
UNSTUNTS_DIR = un_stunts
UNSTUNTS_SRC	= $(UNSTUNTS_DIR)/unpack.c
UNSTUNTS_NAME = unstunts

UNSTUNTS_OBJ = $(UNSTUNTS_SRC:.c=.o)

all:	$(UNSTUNTS_NAME)

$(UNSTUNTS_NAME): $(UNSTUNTS_OBJ)
	 $(CC) $(CFLAGS) -o $(UNSTUNTS_NAME) $(UNSTUNTS_OBJ)

clean:
	 $(RM) $(UNSTUNTS_OBJ)

distclean:  clean
	 $(RM) $(UNSTUNTS_NAME)

.PHONY:  clean all distclean
