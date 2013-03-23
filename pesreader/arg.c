#include "arg.h"

void help(char *name)
{
        fprintf(stderr, "Usage : %s -p <.pes>\n", name);
        exit(EXIT_FAILURE);
}

void parse_opt(int argc, char **argv, struct s_conf *conf)
{
        int c;

        while (1)
        {
                static struct option long_options[] =
                {
                        {"pes", required_argument, 0, 'p'},
                        {0, 0, 0, 0}
                };
                int option_index = 0;
                c = getopt_long (argc, argv, "p:", long_options, &option_index);
                if (c == -1)
                        break;
                switch (c)
                {
                        case 'p':
                                conf->pes = optarg;
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
        if (conf->pes == NULL)
        {
                fprintf(stderr, "You must specify a file input (*.PES) with option -p\n");
		help(name);
                exit(EXIT_FAILURE);
        }
	if (open_and_map(conf->pes, &conf->fpes) == 0)
	{
		fprintf(stderr, "[-] open_and_map(\"%s\", ... )\n", conf->pes);
		clean(conf);
                exit(EXIT_FAILURE);
	}
}

void clean(struct s_conf *conf)
{
	clean_file(&conf->fpes);
}

