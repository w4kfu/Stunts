#include "arg.h"

void help(char *name)
{
        fprintf(stderr, "Usage : %s -h <.hdr> -m <.cmn> -d <.dif> -c <.cod>\n", name);
        exit(EXIT_FAILURE);
}

void parse_opt(int argc, char **argv, struct s_conf *conf)
{
        int c;

        while (1)
        {
                static struct option long_options[] =
                {
                        {"hdr", required_argument, 0, 'h'},
                        {"cmn", required_argument, 0, 'm'},
                        {"dif", required_argument, 0, 'd'},
                        {"cod", required_argument, 0, 'c'},
                        {0, 0, 0, 0}
                };
                int option_index = 0;
                c = getopt_long (argc, argv, "h:m:d:c:", long_options, &option_index);
                if (c == -1)
                        break;
                switch (c)
                {
                        case 'h':
                                conf->hdr = optarg;
                                break;
                        case 'm':
                                conf->cmn = optarg;
                                break;
                        case 'd':
                                conf->dif = optarg;
                                break;
                        case 'c':
                                conf->cod = optarg;
                                break;
                        case '?':
                                break;
                        default:
                                help(argv[0]);
                }
        }
}

void check_opt(struct s_conf *conf, char *name)
{
        if (!conf->hdr && !conf->cmn && !conf->dif && !conf->cod)
                help(name);
        if (conf->hdr == NULL)
        {
                fprintf(stderr, "You must specify a file input (*.HDR) with option -h\n");
                exit(EXIT_FAILURE);
        }
        else if (conf->cmn == NULL)
        {
                fprintf(stderr, "You must specify a file output (*.CMN) with option -m\n");
                exit(EXIT_FAILURE);
        }
        else if (conf->dif == NULL)
        {
                fprintf(stderr, "You must specify a file output (*.DIF) with option -d\n");
                exit(EXIT_FAILURE);
        }
        else if (conf->cod == NULL)
        {
                fprintf(stderr, "You must specify a file output (*.COD) with option -c\n");
                exit(EXIT_FAILURE);
        }
	if (open_and_map(conf->hdr, &conf->fhdr) == 0)
	{
		fprintf(stderr, "[-] open_and_map(\"%s\", ... )\n", conf->hdr);
		clean(conf);
                exit(EXIT_FAILURE);
	}
	if (open_and_map(conf->cmn, &conf->fcmn) == 0)
	{
		fprintf(stderr, "[-] open_and_map(\"%s\", ... )\n", conf->cmn);
		clean(conf);
                exit(EXIT_FAILURE);
	}
	if (open_and_map(conf->dif, &conf->fdif) == 0)
	{
		fprintf(stderr, "[-] open_and_map(\"%s\", ... )\n", conf->dif);
		clean(conf);
                exit(EXIT_FAILURE);
	}
	if (open_and_map(conf->cod, &conf->fcod) == 0)
	{
		fprintf(stderr, "[-] open_and_map(\"%s\", ... )\n", conf->cod);
		clean(conf);
                exit(EXIT_FAILURE);
	}
	conf->dh = (struct dos_header*)conf->fhdr.bMap;
	conf->ccmn = (struct s_comp*)conf->fcmn.bMap;
	conf->cdif = (struct s_comp*)conf->fdif.bMap;
	conf->ccod = (struct s_comp*)conf->fcod.bMap;
}

void clean(struct s_conf *conf)
{
	clean_file(&conf->fhdr);
	clean_file(&conf->fcmn);
	clean_file(&conf->fdif);
	clean_file(&conf->fcod);
}

