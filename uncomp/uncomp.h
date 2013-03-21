#ifndef _UNCOMP_H
#define _UNCOMP_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "tree.h"

#define HTREE_MAXLEVEL          24

struct symbolsin
{
	unsigned int nb;
	unsigned char *symbol;
};

struct bitmap
{
	unsigned char *pbuf;
        unsigned char buf;
        unsigned char count;
};

struct s_comp
{
        unsigned char type;
        unsigned int size:24;
	unsigned char *buf_data;
	struct s_tree *tree;
	unsigned char *buf_out;
};

int getbit(struct bitmap *p);
void uncomp(struct s_comp *comp, unsigned char *end_buf);
struct s_tree *tree_uniform_build(size_t depth, size_t depthmax, struct symbolsin *symbolsyn);
struct s_tree *huff_tree(unsigned int treelevel, struct symbolsin *symbolsin);
void huff(unsigned char *buf, size_t size, struct s_comp *comp);


#endif
