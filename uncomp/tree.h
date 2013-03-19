#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
 
#ifndef _TREE_H
#define _TREE_H

#define MAX(a,b) ((a)<(b)?(b):(a))

struct s_tree
{
	unsigned int key;
	int used;
 	struct s_tree *left;
	struct s_tree *right;
};

#define TSIZE (sizeof (struct s_tree))
 
struct s_tree *make_tree(int k, struct s_tree *lson, struct s_tree *rson);
size_t  height(struct s_tree *t);
size_t  size(struct s_tree *t);
 
#endif
