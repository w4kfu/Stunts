#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tree.h"

void hex_dump(void *data, int size);

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

void printdotty(FILE *fp, struct s_tree *tree, int p, struct s_tree *tree_l)
{
	static int c = 1;
	int tc;

	if (p == 0)
		c = 0;
	if (tree == NULL)
		return;
	tc = ++c;
	if (tree->key == 0x1B)
		fprintf(fp, "%d [label=\"%04X\" color=\"green\"];\n", c, tree->key);
	else if (tree->used == 0)
		fprintf(fp, "%d [label=\"%04X\" color=\"red\"];\n", c, tree->key);
	else
		fprintf(fp, "%d [label=\"%04X\" color=\"blue\"];\n", c, tree->key);
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

int count = 0;
unsigned char *gcode;
struct symbolsin
{
	unsigned int nb;
	unsigned char *symbol;
};

struct s_tree *tree_uniform_build(size_t depth, size_t depthmax, struct symbolsin *symbolsyn)
{
	struct s_tree *t = NULL;
  	if (depth <= depthmax)
    	{
      		t = malloc(sizeof (struct s_tree));
		if (symbolsyn[depth].nb)
		{
			t->used = 1;
			t->key = *symbolsyn[depth].symbol++;
			symbolsyn[depth].nb--;
			count++;
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

void huff_tree(unsigned int treelevel, struct symbolsin *symbolsin)
{
	struct s_tree *tree = NULL;

	tree = make_tree(256, NULL, NULL);
	tree = tree_uniform_build(0, treelevel, symbolsin);
	//depth_print(tree);
	printf("height = %d\n", height(tree));
	printf("size = %d\n", size(tree));
	printf("count = %X\n", count);
	dotty(tree, "test.dot");
}


void new_huff(unsigned char *buf, size_t size)
{
	unsigned int uncomp_size = 0;
	unsigned int treelevels = 0;
	unsigned int symbol_size = 0;
	struct symbolsin *symbolsin;
	unsigned char *alph = NULL;
	unsigned int i, j;

	printf("CompressedSize = %X\n", size);
	buf++;
	uncomp_size = (*buf) | (*(buf + 1) << 0x8) | (*(buf + 2) << 0x10);
	buf += 3;
	printf("Uncomp_size = %X\n", uncomp_size);
	treelevels = *buf++;
	printf("TreeLevels = %X\n", treelevels);
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
	huff_tree(treelevels, symbolsin);
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
	new_huff(buf, st.st_size);
	//new(buf, st.st_size);
	//uncomp(buf, st.st_size);
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
