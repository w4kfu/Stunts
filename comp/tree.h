#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
 
#ifndef _TREE_H
#define _TREE_H

#define MAX(a,b) ((a)<(b)?(b):(a))

struct s_tree
{
	int freq;
	unsigned short ch;
 	struct s_tree *left;
	struct s_tree *right;
};

#define TSIZE (sizeof (struct s_tree))
 
struct s_tree *make_node(char c, int freq, struct s_tree *lson, struct s_tree *rson);
size_t  tree_height(struct s_tree *t);
size_t  tree_size(struct s_tree *t);
 
#endif
