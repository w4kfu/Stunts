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

void firstpasses(unsigned char *buf, unsigned char *dbuf, unsigned short *head, unsigned int uncomp_size)
{
	unsigned short block_size = 0;
	unsigned short size = 0;
	unsigned char escape = 0;
	unsigned char actual = 0;
	unsigned char *buf_seq = NULL;
	unsigned char nb_seq = 0;
	int i;

	(void)uncomp_size;
	block_size = head[0];
	size = head[1];
	escape = head[3] & 0xFF;
	printf("Block_size = %X\n", block_size);
	printf("Size = %X\n", size);
	printf("Escape = %X\n", escape);
	while (1)
	{
		actual = *buf++;
		if (actual != escape)
		{
			*dbuf++ = actual;
			block_size--;
			if (!block_size)
			{
				if (!size)
					break;
				size--;
			}
		}
		else
		{
			buf_seq = buf;
			while ((actual = *buf++) != escape)
			{
				*dbuf++ = actual;
				block_size--;
				if (!block_size)
				{
					if (!size)
					{
						printf("BREAK WTF !?");
						break;
					}
					size--;
				}
			}
			nb_seq = *buf++ - 1;
			while(nb_seq--)
			{
				for (i = 0; i < (buf - buf_seq - 2); i++)
					*dbuf++ = *(buf_seq + i);
			}
			if (!block_size && !size)
				break;
		}
	}
}

void secondpasses(unsigned char *buf, unsigned char *res, unsigned short *head, unsigned int uncomp_size)
{
	unsigned char tab[0x100];
	unsigned char *t = NULL;
	unsigned int i;
	unsigned int count = 0;
	unsigned char actual = 0;
	unsigned char nb_rep = 0;
	unsigned char *sres = NULL;

	memset(tab, 0, 0x100);
	printf("LEN = %X\n", head[2]);
	t = (unsigned char*)head + 5;
	printf("START = %X\n", *t);
	for (i = 0; i < (head[2] & 0x7F); i++)
	{
		tab[t[i]] = i + 1;
	}
	//hex_dump(tab, 0x100);
	sres = res;
	while (count < uncomp_size)
	{
		actual = *buf++;
		if (tab[actual])
		{
			if (tab[actual] == 0x01)
			{
				nb_rep = *buf++;
				actual = *buf++;
			}
			else if (tab[actual] == 0x03)
			{
				nb_rep = *buf++;	
				nb_rep |= *buf++ << 8;
				actual = *buf++;
			}
			else
			{
				nb_rep = tab[actual] - 1;
				actual = *buf++;
			}
			count += nb_rep;
			while (nb_rep--)
			{
				*res++ = actual;
			}
		}
		else
		{
			*res++ = actual;
			count += 1;
		}
	}
	printf("COunt = %X\n", count);
	(void)sres;
	//hex_dump(sres, count);
}

void rle(unsigned char *rbuf, unsigned int buf_size)
{
	unsigned int uncomp_size;
	unsigned short head[0x8];
	unsigned char *buf = NULL;
	unsigned char *tmp = NULL;
	unsigned char *res = NULL;

	(void)buf_size;
	buf = rbuf;
	if (*buf++ != 0x01)
	{
		fprintf(stderr, "[-] Wrong type !\n");
	}
	uncomp_size = (*buf) | (*(buf + 1) << 0x8) | (*(buf + 2) << 0x10);
	buf += 3;
	printf("Uncomp_size = %X\n", uncomp_size);
	memcpy(head, buf, 0x8 * 2);	
	buf = rbuf + 9;
	printf("OFF = %X\n", head[2]);
	printf("OFF = %X\n", head[2] & 0x7F);
	buf = buf + (head[2] & 0x7F);
	printf("DIFF = %02X\n", buf - rbuf);
	if ((head[2] & 0xFF) <= 0x80)
	{
		if (head[2] != 1)
		{
			/*unsigned short bx, ax = 0;
			bx = head[0];
			ax = head[1];
			printf("BX = %X\n", bx);
			printf("AX = %X (LOOK LIKE SIZE)\n", ax);*/
			tmp = malloc(sizeof (char) * uncomp_size);
			if (!tmp)
			{
				fprintf(stderr, "malloc()");
				return;
			}
			firstpasses(buf, tmp, head, uncomp_size);
			buf = tmp;
		}
	}
	if (!(res = malloc(sizeof (char) * uncomp_size)))
	{
		perror("malloc()");
		return;
	}
	secondpasses(buf, res, head, uncomp_size);
	//hex_dump(tmp, uncomp_size);
}

