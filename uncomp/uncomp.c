#include "uncomp.h"

int getbit(struct bitmap *p) 
{
    int b;

    b = p->buf & 0x80;
    if(--p->count == 0)
    {
        p->buf = *p->pbuf++;
        p->count = 0x8;
    }
    else
        p->buf <<= 1;
    return b;
}

void uncomp(struct s_comp *comp, unsigned char *end_buf)
{
	struct bitmap bits;
	struct s_tree *dtree = NULL;
	unsigned int count = 0;
	unsigned char *buf_res = NULL;

	buf_res = malloc(sizeof (char) * comp->size);
	if (!buf_res)
	{
		perror("malloc()");
		comp->buf_out = NULL;
		return;
	}
	bits.count = 0x8;
	bits.pbuf = comp->buf_data;
	bits.buf = *bits.pbuf++;
	dtree = comp->tree;
	while (count < comp->size)
	{
		while (dtree->key == 256)
		{
			if (getbit(&bits))
			{
				dtree = dtree->right;
			}
			else
			{
				dtree = dtree->left;
			}
			if (!dtree)
			{
				fprintf(stderr, "WTF !?\n");
				exit(EXIT_FAILURE);
			}

		}
		buf_res[count] = dtree->key;
		count++;
		if (bits.pbuf > end_buf)
			break;
		dtree = comp->tree;
	}
	if (count != comp->size)
	{
		fprintf(stderr, "Data left !\n");
		exit(EXIT_FAILURE);
	}
	comp->buf_out = buf_res;
}

struct s_tree *tree_uniform_build(size_t depth, size_t depthmax, struct symbolsin *symbolsyn)
{
	struct s_tree *t = NULL;

  	if (depth <= depthmax)
    	{
      		t = malloc(sizeof (struct s_tree));
		if (!t)
		{
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		if (symbolsyn[depth].nb)
		{
			t->used = 1;
			t->key = *symbolsyn[depth].symbol++;
			symbolsyn[depth].nb--;
		}
		else
		{
			t->used = 0;
			t->key = 256;
		}
		if (t->used == 1)
		{
			t->left = NULL;
			t->right = NULL;
		}
		else
		{
      			t->left = tree_uniform_build(depth + 1, depthmax, symbolsyn);
      			t->right = tree_uniform_build(depth + 1, depthmax, symbolsyn);
		}
    	}
  	return t;
}

struct s_tree *huff_tree(unsigned int treelevel, struct symbolsin *symbolsin)
{
	struct s_tree *tree = NULL;

	tree = make_tree(256, NULL, NULL);
	tree = tree_uniform_build(0, treelevel, symbolsin);
	printf("height = %d\n", height(tree));
	printf("size = %d\n", size(tree));
	return tree;
}


void huff(unsigned char *buf, size_t size, struct s_comp *comp)
{
	unsigned int uncomp_size = 0;
	unsigned int treelevels = 0;
	struct symbolsin *symbolsin;
	unsigned char *alph = NULL;
	unsigned int i, j;
	struct s_tree *tree = NULL;
	//unsigned char *buf_end = NULL;

	//buf_end = buf + size;
	printf("CompressedSize = %X\n", size);
	if (*buf++ != 0x02)
	{
		fprintf(stderr, "Wrong type of file\n");
		return;
	}
	uncomp_size = (*buf) | (*(buf + 1) << 0x8) | (*(buf + 2) << 0x10);
	buf += 3;
	printf("Uncomp_size = %X\n", uncomp_size);
	treelevels = *buf++;
	printf("TreeLevels = %X\n", treelevels);
	if (treelevels > HTREE_MAXLEVEL)
	{
		fprintf(stderr, "Huffman tree has insane levels\n");
		return;
	}
	symbolsin = malloc(sizeof (struct symbolsin) * (treelevels + 1));
	if (!symbolsin)
	{
		perror("malloc()");
		return;
	}
	symbolsin[0].nb = 0;
	alph = buf + treelevels;
        for (i = 1; i <= treelevels; i++)
        {
                symbolsin[i].nb = *buf++;
		symbolsin[i].symbol = malloc(sizeof (char) * symbolsin[i].nb);
		for (j = 0; j < symbolsin[i].nb; j++)
		{
			symbolsin[i].symbol[j] = *alph++;
		}	
        }
	tree = huff_tree(treelevels, symbolsin);
	comp->size = uncomp_size;
	comp->buf_data = alph;
	comp->tree = tree;
	/*if (buf_data)
		*buf_data = alph;*/
	//uncomp(alph, tree, buf_end, uncomp_size, outfile);
	return;
}
