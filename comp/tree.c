#include "tree.h"
 
struct s_tree *make_node(char c, int freq, struct s_tree *lson, struct s_tree *rson)
{
  	struct s_tree *t = NULL;

	t = malloc(TSIZE);
	t->ch = c;
	t->freq = freq;
	t->left = lson;
	t->right = rson;
	return t;
}
 
size_t tree_height(struct s_tree *t)
{
	size_t h = -1;
	size_t hr;
	size_t hl;

  	if (t)
    	{
      		hl = tree_height(t->left);
      		hr = tree_height(t->right);
      		h = 1 + MAX(hl, hr);
    	}
  	return h;
}
 
size_t tree_size(struct s_tree *t)
{
  return (t ? 1 + tree_size(t->left) + tree_size(t->right): 0);
}
