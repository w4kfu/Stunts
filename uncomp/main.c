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
#include "uncomp.h"

struct conf_c
{
	char *in;
	char *out;
	char *dot;
};

void hex_dump(void *data, int size);

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
	/* debug information */
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

int main(int argc, char **argv)
{
        int fd;
        struct stat st;
        unsigned char *buf = NULL;
	struct conf_c conf = {0};
	struct s_tree *tree = NULL;

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
	tree = huff(buf, st.st_size, conf.out);
	if (tree)
	{
		if (conf.dot)
			dotty(tree, conf.dot);
	}
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
