#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

#include "tree.h"
#include "pqueue.h"

struct symbolsin
{
	unsigned int nb;
	unsigned char *symbol;
};

struct conf_c
{
	char *in;
	char *out;
	char *dot;
};

void help(char *name)
{
	fprintf(stderr, "Usage : %s -i <input_file> -o <output_file> [-d <output_dot_file>]\n", name);
	exit(EXIT_FAILURE);
}

void parse_opt(int argc, char **argv, struct conf_c *conf)
{
   	int c;

   	while (1)
     	{
       		static struct option long_options[] =
         	{
           		{"in", required_argument, 0, 'i'},
           		{"out", required_argument, 0, 'o'},
           		{"dot", required_argument, 0, 'd'},
           		{0, 0, 0, 0}
         	};
       		int option_index = 0;
       		c = getopt_long (argc, argv, "i:o:d:", long_options, &option_index);
       		if (c == -1)
         		break;
       		switch (c)
         	{
         		case 'i':
				conf->in = optarg;
           			break;
         		case 'o':
				conf->out = optarg;
           			break;
         		case 'd':
				conf->dot = optarg;
           			break;
         		case '?':
           			break;
         		default:
           			help(argv[0]);
         	}
     	}
}

void check_opt(struct conf_c *conf, char *name)
{
	if (!conf->in && !conf->out)
		help(name);
	if (conf->in == NULL)
	{
		fprintf(stderr, "You must specify a file input with option -i\n");
		exit(EXIT_FAILURE);
	}
	else if (conf->out == NULL)
	{
		fprintf(stderr, "You must specify a file output with option -o\n");
		exit(EXIT_FAILURE);
	}
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
			t->freq = 1;
			t->ch = *symbolsyn[depth].symbol++;
			symbolsyn[depth].nb--;
		}
		else
		{
			t->freq = 0;
			t->ch = 0xFFFF;
		}
		if (t->freq == 1)
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

void make_symbol(struct s_tree *t, unsigned int level, struct symbolsin *symbolsin)
{
	if ((t->left == NULL) && (t->right) == NULL)
	{
		symbolsin[level].symbol[symbolsin[level].nb] = t->ch;
		symbolsin[level].nb++;
	}
	else
	{
		make_symbol(t->left, level + 1, symbolsin);
		make_symbol(t->right, level + 1, symbolsin);
	}

}

void make_codes(struct s_tree *t, int level, unsigned char *enc, unsigned char **codes) 
{
	if ((t->left == NULL) && (t->right == NULL)) 
	{
		enc[level] = 0;
		codes[t->ch] = (unsigned char*)strdup((char*)enc);
	} 
	else 
	{
		enc[level] = '0';
		make_codes(t->left, level + 1, enc, codes);
		enc[level] = '1';
		make_codes(t->right, level + 1, enc, codes);
	}
}

int nbits, current_byte, nbytes;

void bitout(FILE *f, char b) 
{
	current_byte <<= 1;
	if (b == '1') current_byte |= 1;
	nbits++;
	if (nbits == 8) 
	{
		fputc (current_byte, f);
		nbytes++;
		nbits = 0;
		current_byte = 0;
	}
}

void encode(unsigned char *buf, size_t size_buf, int treelevels, struct symbolsin *symbolsin, unsigned char **codes, struct conf_c *conf)
{
	FILE *fout;
	unsigned char ch;
	unsigned char *s;
	unsigned char *buf_end;
	int i;
	unsigned int j;

	fout = fopen(conf->out, "w");
	if (!fout)
	{
		perror("fopen");
		return;
	}
	fputc(0x02, fout);			// type
	fputc(size_buf & 0xFF, fout);		// size
	fputc((size_buf >> 8) & 0xFF, fout);	// size
	fputc((size_buf >> 0x10) & 0xFF, fout);	// size
	fputc(treelevels, fout);		// Huffman tree depths
	for (i = 1; i <= treelevels; i++)
	{
		fputc(symbolsin[i].nb, fout);	// Symbol freq
	}
	for (i = 1; i <= treelevels; i++)
	{
		for (j = 0; j < symbolsin[i].nb; j++)
			fputc(symbolsin[i].symbol[j], fout);	// Symbol
	}
	buf_end = buf + size_buf;		// compressed data
	while (buf < buf_end)
	{
		ch = *buf++;
		for (s = codes[ch]; *s; s++)
			bitout(fout, *s);
	}	
	while (nbits) 
		bitout(fout, '0');
	fclose(fout);
}

void huff(unsigned char *buf, size_t size_buf, struct conf_c *conf)
{
	unsigned int freqs[256];
	unsigned int i;
	unsigned int treelevel;
	struct s_tree *t = NULL;
	struct symbolsin *symbolsin;
	struct symbolsin *ssymbolsin;
	unsigned char enc[256];
	unsigned char *codes[256];

	memset(freqs, 0, sizeof (freqs));
	for (i = 0; i < size_buf; i++)
		freqs[buf[i]] += 1;
	t = build_huffman(freqs);
	treelevel = tree_height(t);
	printf("Tree Height = %d\n", treelevel);
	printf("Tree Size = %d\n", tree_size(t));
	symbolsin = malloc(sizeof (struct symbolsin) * (treelevel + 1));
	ssymbolsin = malloc(sizeof (struct symbolsin) * (treelevel + 1));
	if (!symbolsin)
	{
		perror("malloc()");
		return;
	}
	for (i = 0; i <= treelevel; i++)
	{
		symbolsin[i].nb = 0;
		ssymbolsin[i].nb = 0;
		symbolsin[i].symbol = malloc(sizeof (char) * 256);
		ssymbolsin[i].symbol = symbolsin[i].symbol;
		if (!symbolsin[i].symbol)
		{
			perror("malloc()");
			return;
		}
	}
	make_symbol(t, 0, symbolsin);
	dbg_print_symbol(symbolsin, treelevel);
	t = tree_uniform_build(0, treelevel, symbolsin);
	if (conf->dot)
		dotty(t, conf->dot);
	make_codes(t, 0, enc, codes);
	make_symbol(t, 0, ssymbolsin);
	encode(buf, size_buf, treelevel, ssymbolsin, codes, conf); 
}

int main(int argc, char **argv)
{
        int fd;
        struct stat st;
        unsigned char *buf = NULL;
	struct conf_c conf = {0};

	parse_opt(argc, argv, &conf);
	check_opt(&conf, argv[0]);
        fd = open(conf.in, O_RDONLY);
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
	huff(buf, st.st_size, &conf);
clean:
        free(buf);
        close(fd);
        return 0;
}
