#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tree.h"
#include "pqueue.h"

#define HTREE_MAXLEVEL          24

struct symbolsin
{
	unsigned int nb;
	unsigned char *symbol;
};

void printdotty(FILE *fp, struct s_tree *tree, int p, struct s_tree *tree_l)
{
        static int c = 1;
        int tc;

        if (p == 0)
                c = 0;
        if (tree == NULL)
                return;
        tc = ++c;
        if (tree->ch == 0xFFFF)
                fprintf(fp, "%d [label=\"%04X\" color=\"red\"];\n", c, tree->ch);
        else
                fprintf(fp, "%d [label=\"%04X\" color=\"blue\"];\n", c, tree->ch);
        if (p != 0)
        {
            fprintf(fp, "%d->%d", p, tc);
            if (tree == tree_l)
                fprintf(fp," [label=\"0\"];\n");
            else
                fprintf(fp," [label=\"1\"];\n");
        }
        printdotty(fp, tree->left, tc, tree->left);
        printdotty(fp, tree->right, tc, tree->left);
        return;
}


void dotty(struct s_tree *tree,char *filename)
{
        FILE *fp;

        fp = fopen(filename, "wb");
        if(fp == NULL)
        {
                perror("fopen()");
                return;
        }
        fprintf(fp,"digraph huff_tree{\n");
        printdotty(fp, tree, 0, tree->left);
        fprintf(fp,"}\n");
        fclose(fp);
        return;
}


/*struct s_tree *build_tree(unsigned char *freqs)
{
	int i;
	struct	s_queue	*q = NULL;
	struct s_tree *parent = NULL;
	struct s_tree *left = NULL;
	struct s_tree *right = NULL;

	for (i = 0; i < 256; i++)
	{
		if (freqs[i] != 0)
			q = push(make_node(i, freqs[i], NULL, NULL), q);
	}
	printf("Size %X\n", queue_size(q));
	//t = top(q);
	//printf("T = %X\n", t->c);
	while (queue_size(q) > 1)
	{
		right = top(q);
		printf("RIGHT = %X\n", right->c);
		q = pop(q);
		left = top(q);
		printf("LEFT = %X\n", left->c);
		q = pop(q);
		parent = make_node(0, 0, left, right);
		q = push(parent, q);
	}
	dotty(top(q), "comp.dot");
	return top(q);
}*/

struct s_tree *build_huffman(unsigned int *freqs) 
{
	int i;
	int n;
	struct s_tree *left = NULL;
	struct s_tree *right = NULL;
	struct s_tree *parent = NULL;
	struct s_pqueue p;

	create_pq(&p);
	for (i = 0; i < NUM_CHARS; i++) 
	{
		if (freqs[i] == 0)
			continue;
		left = malloc (sizeof (struct s_tree));
		left->left = NULL;
		left->right = NULL;
		left->freq = freqs[i];
		left->ch = (unsigned short)i;
		insert_pq (&p, left);
	}
	n = p.heap_size - 1;
	for (i = 0; i < n; i++) 
	{
		parent = malloc(sizeof (struct s_tree));
		left = extract_min_pq(&p);
		right = extract_min_pq(&p);
		parent->left = left;
		parent->right = right;
		parent->freq = left->freq + right->freq;
		parent->ch = 0xFFFF;
		insert_pq(&p, parent);
	}
	return extract_min_pq(&p);
}

void dbg_print_symbol(struct symbolsin *symbolsin, unsigned int treelevel)
{
	unsigned int i;
	unsigned int j;

	for (i = 0; i <= treelevel; i++)
	{
		printf("Symbol %d : ", i);
		for (j = 0; j < symbolsin[i].nb; j++)
		{
			printf("0x%02X ;", symbolsin[i].symbol[j]);
		}
		printf("\n");
	}
}

void make_symbol(struct s_tree *t, unsigned int level, unsigned char *enc, unsigned char **codes, struct symbolsin *symbolsin)
{
	if ((t->left == NULL) && (t->right) == NULL)
	{
		enc[level] = 0;
		codes[t->ch] = strdup(enc);
		symbolsin[level].symbol[symbolsin[level].nb] = t->ch;
		symbolsin[level].nb++;
	}
	else
	{
		enc[level] = '0';
		make_symbol(t->left, level + 1, enc, codes, symbolsin);
		enc[level] = '1';
		make_symbol(t->right, level + 1, enc, codes, symbolsin);
	}

}

void huff(unsigned char *buf, size_t size_buf)
{
	unsigned int freqs[256];
	unsigned int i;
	unsigned int treelevel;
	struct s_tree *t = NULL;
	struct symbolsin *symbolsin;
	unsigned char enc[256];
	unsigned char *codes[256];

	memset(freqs, 0, sizeof (freqs));
	for (i = 0; i < size_buf; i++)
		freqs[buf[i]] += 1;
	t = build_huffman(freqs);
	dotty(t, "comp.dot");
	treelevel = tree_height(t);
	printf("Tree Height = %d\n", treelevel);
	printf("Tree Size = %d\n", tree_size(t));
	symbolsin = malloc(sizeof (struct symbolsin) * (treelevel + 1));
	if (!symbolsin)
	{
		perror("malloc()");
		return;
	}
	for (i = 0; i <= treelevel; i++)
	{
		symbolsin[i].nb = 0;
		symbolsin[i].symbol = malloc(sizeof (char) * 256);
		if (!symbolsin[i].symbol)
		{
			perror("malloc()");
			return;
		}
	}
	make_symbol(t, 0, enc, codes, symbolsin);
	dbg_print_symbol(symbolsin, treelevel);
}

int main(int argc, char **argv)
{
        int fd;
        struct stat st;
        unsigned char *buf = NULL;

	if (argc != 2)
	{
		fprintf(stderr, "%s <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

        fd = open(argv[1], O_RDONLY);
        if (fd == -1)
        {
                perror("open()");
                exit(EXIT_FAILURE);
        }
        if (fstat(fd, &st) == -1)
        {
                perror("fstat()");
                exit(EXIT_FAILURE);
        }
        if ((buf = malloc(sizeof (char) * st.st_size)) == NULL)
        {
                perror("malloc()");
                exit(EXIT_FAILURE);
        }
        if (read(fd, buf, st.st_size) != st.st_size)
        {
                perror("read()");
                goto clean;
        }
	huff(buf, st.st_size);
clean:
        free(buf);
        close(fd);
        return 0;
}

void hex_dump(void *data, int size)
{
    unsigned char *p = (unsigned char*)data;
    unsigned char c;
    int n;
    char bytestr[4] = {0};
    char addrstr[10] = {0};
    char hexstr[16 * 3 + 5] = {0};
    char charstr[16 * 1 + 5] = {0};

    for(n = 1; n <= size; n++)
    {
        if (n % 16 == 1)
        {
                snprintf(addrstr, sizeof(addrstr), "%.4x",
                    ((unsigned int)p-(unsigned int)data));
        }
        c = *p;
        if (isprint(c) == 0)
        {
            c = '.';
        }
        snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
        strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr)-1);
        snprintf(bytestr, sizeof(bytestr), "%c", c);
        strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr)-1);
        if (n % 16 == 0)
        {
            printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
            hexstr[0] = 0;
            charstr[0] = 0;
        }
        else if (n % 8 == 0)
        {
            strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr)-1);
            strncat(charstr, " ", sizeof(charstr)-strlen(charstr)-1);
        }
        p++;
    }
    if (strlen(hexstr) > 0)
    {
        printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
    }
}
