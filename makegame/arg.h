#ifndef __ARG_H__
#define __ARG_H__

#include <getopt.h>
#include "file.h"
#include "../uncomp/uncomp.h"

struct dos_header
{
        unsigned short e_magic;
        unsigned short e_cblp;
        unsigned short e_cp;
        unsigned short e_crlc;
        unsigned short e_cparhdr;
        unsigned short e_minalloc;
        unsigned short e_maxalloc;
        unsigned short e_ss;
        unsigned short e_sp;
        unsigned short e_csum;
        unsigned short e_ip;
        unsigned short e_cs;
        unsigned short e_lfarlc;
        unsigned short e_ovno;
};

struct s_conf
{
        char *hdr;
	struct s_file fhdr;
	struct dos_header *dh;
        char *cmn;
	struct s_file fcmn;
	struct s_comp ccmn;
        char *dif;
	struct s_file fdif;
	struct s_comp cdif;
	char *cod;
	struct s_file fcod;
	struct s_comp ccod;
};


void help(char *name);
void parse_opt(int argc, char **argv, struct s_conf *conf);
void check_opt(struct s_conf *conf, char *name);
void clean(struct s_conf *conf);

#endif // __ARG_H__

