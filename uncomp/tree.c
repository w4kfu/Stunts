#include "tree.h"
 
struct s_tree *make_tree(int k, struct s_tree *lson, struct s_tree *rson)
{
  	struct s_tree *t = NULL;

	t = malloc(TSIZE);
	t->key = k;
	t->used = 0;
	t->left = lson;
	t->right = rson;
	return t;
}
 
size_t height(struct s_tree *t)
{
	size_t h = -1;
	size_t hr;
	size_t hl;

  	if (t)
    	{
      		hl = height(t->left);
      		hr = height(t->right);
      		h = 1 + MAX(hl, hr);
    	}
  	return h;
}
 
size_t size(struct s_tree *t)
{
  return (t ? 1 + size(t->left) + size(t->right): 0);
}
