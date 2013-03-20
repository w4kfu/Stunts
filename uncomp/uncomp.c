#include "uncomp.h"

void dump_to_file(char *filename, unsigned char *buf, size_t size)
{
	int fd;

        fd = open(filename, O_WRONLY | O_CREAT, 0644);
        if (fd == -1)
        {
                perror("open()");
                exit(0);
        }
	write(fd, buf, size);
	close(fd);
}

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

void uncomp(unsigned char *buf, struct s_tree *tree, unsigned char *end_buf, unsigned int uncomp_size, char *outfile)
{
	struct bitmap bits;
	struct s_tree *dtree = NULL;
	unsigned int count = 0;
	unsigned char *buf_res = NULL;

	buf_res = malloc(sizeof (char) * uncomp_size);
	if (!buf)
	{
		perror("malloc()");
		return;
	}
	bits.count = 0x8;
	bits.pbuf = buf;
	bits.buf = *bits.pbuf++;
	dtree = tree;
	while (count < uncomp_size)
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
		if (buf > end_buf)
			break;
		dtree = tree;
	}
	if (count != uncomp_size)
	{
		fprintf(stderr, "Data left !\n");
	}
	dump_to_file(outfile, buf_res, count);
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


struct s_tree *huff(unsigned char *buf, size_t size, char *outfile)
{
	unsigned int uncomp_size = 0;
	unsigned int treelevels = 0;
	struct symbolsin *symbolsin;
	unsigned char *alph = NULL;
	unsigned int i, j;
	struct s_tree *tree = NULL;
	unsigned char *buf_end = NULL;

	buf_end = buf + size;
	printf("CompressedSize = %X\n", size);
	if (*buf++ != 0x02)
	{
		fprintf(stderr, "Wrong type of file\n");
		return NULL;
	}
	uncomp_size = (*buf) | (*(buf + 1) << 0x8) | (*(buf + 2) << 0x10);
	buf += 3;
	printf("Uncomp_size = %X\n", uncomp_size);
	treelevels = *buf++;
	printf("TreeLevels = %X\n", treelevels);
	if (treelevels > HTREE_MAXLEVEL)
	{
		fprintf(stderr, "Huffman tree has insane levels\n");
		return NULL;
	}
	symbolsin = malloc(sizeof (struct symbolsin) * (treelevels + 1));
	if (!symbolsin)
	{
		perror("malloc()");
		return NULL;
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
	uncomp(alph, tree, buf_end, uncomp_size, outfile);
	return tree;
}
