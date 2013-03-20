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

int getbit(struct bitmap *p);
void uncomp(unsigned char *buf, struct s_tree *tree, unsigned char *end_buf, unsigned int uncomp_size, char *outfile);
struct s_tree *tree_uniform_build(size_t depth, size_t depthmax, struct symbolsin *symbolsyn);
struct s_tree *huff_tree(unsigned int treelevel, struct symbolsin *symbolsin);
struct s_tree *huff(unsigned char *buf, size_t size, char *outfile);


#endif
