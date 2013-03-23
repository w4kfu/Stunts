#ifndef __ARG_H__
#define __ARG_H__

#include <getopt.h>
#include "file.h"

struct s_conf
{
        char *pes;
	struct s_file fpes;
};


void help(char *name);
void parse_opt(int argc, char **argv, struct s_conf *conf);
void check_opt(struct s_conf *conf, char *name);
void clean(struct s_conf *conf);

#endif // __ARG_H__

