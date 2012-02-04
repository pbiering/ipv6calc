/*
 * Project    : ipv6calc
 * File       : ipv6loganon.c
 * Version    : $Id: ipv6loganon.c,v 1.10 2012/02/04 21:01:02 peter Exp $
 * Copyright  : 2007-2012 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Dedicated program for logfile anonymization
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <getopt.h> 
#include <unistd.h>

#include "ipv6loganon.h"
#include "libipv6calcdebug.h"
#include "libipv6calc.h"
#include "ipv6calccommands.h"
#include "ipv6calctypes.h"
#include "ipv6loganonoptions.h"
#include "ipv6calchelp.h"
#include "ipv6loganonhelp.h"

#include "libipv4addr.h"
#include "libipv6addr.h"
#include "libmac.h"

#include "librfc1884.h"
#include "librfc1886.h"
#include "librfc1924.h"
#include "libifinet6.h"
#include "librfc2874.h"
#include "librfc3056.h"
#include "libeui64.h"
#include "libieee.h"

#define LINEBUFFER	16384

long int ipv6calc_debug = 0;
int flag_quiet = 1;
int flag_nocache = 0;

/* default values */
int mask_ipv4 = 24;
int mask_ipv6 = 48;
int mask_iid = 1;


/* prototypes */
static int anonymizetoken(char *result, const char *token);
static void lineparser();


/* LRU cache */

#define CACHE_LRU_SIZE 200

static int      cache_lru_max = 0;
static int      cache_lru_last = 0;
int      cache_lru_limit = 20; /* optimum */
static char     cache_lru_key_token[CACHE_LRU_SIZE][NI_MAXHOST];
static char     cache_lru_value[CACHE_LRU_SIZE][NI_MAXHOST];
static long int cache_lru_statistics[CACHE_LRU_SIZE];

char	file_out[NI_MAXHOST] = "";
int	file_out_flag = 0;
int	file_out_flush = 0;
char	file_out_mode[NI_MAXHOST] = "";
FILE	*FILE_OUT;

/**************************************************/
/* main */
#define DEBUG_function_name "ipv6loganon/main"
int main(int argc,char *argv[]) {
	int i, lop;

	/* Fetch the command-line arguments. */
	while ((i = getopt_long(argc, argv, ipv6loganon_shortopts, ipv6loganon_longopts, &lop)) != EOF) {
		switch (i) {
			case -1:
				break;

			case 'v':
				printversion();
				exit(EXIT_FAILURE);
				break;

			case 'V':
				flag_quiet = 0;
				break;

			case 'h':
			case '?':
				ipv6loganon_printhelp();
				exit(EXIT_FAILURE);
				break;
				
			case 'd':
				ipv6calc_debug = atol(optarg);
				break;

			case 'f':
				file_out_flush = 1;
				break;

			case 'w':
			case 'a':
				if (strlen(optarg) < NI_MAXHOST) {
					strcpy(file_out, optarg);
					file_out_flag = 1;
				} else {
					fprintf(stderr, " Output file too long: %s\n", optarg);
					exit(EXIT_FAILURE);
				};

				switch (i) {
					case 'w':
						strcpy(file_out_mode, "w");
						break;
					case 'a':
						strcpy(file_out_mode, "a");
						break;	
				};
				break;

			case 'c':
				cache_lru_limit = atoi(optarg);
				if (cache_lru_limit > CACHE_LRU_SIZE) {
					cache_lru_limit = CACHE_LRU_SIZE;
					fprintf(stderr, " Cache limit too big, built-in limit: %d\n", cache_lru_limit);
				};
				if (cache_lru_limit < 1) {
					cache_lru_limit = 1;
					fprintf(stderr, " Cache limit too small, take minimum: %d\n", cache_lru_limit);
				};
				break;

			case 'n':
				flag_nocache = 1;
				break;

			case CMD_ANON_NO_MASK_IID:
				mask_iid = 0;
				break;

			case CMD_ANON_MASK_IPV4:
				mask_ipv4 = atoi(optarg);
				if (mask_ipv4 < 0 || mask_ipv4 > 32) {
					fprintf(stderr, " value for option 'mask-ipv4' out-of-range  [0-32]\n");
					exit(EXIT_FAILURE);
				};
				break;

			case CMD_ANON_MASK_IPV6:
				mask_ipv6 = atoi(optarg);
				if (mask_ipv6 < 0 || mask_ipv4 > 64) {
					fprintf(stderr, " value for option 'mask-ipv6' out-of-range  [0-64]\n");
					exit(EXIT_FAILURE);
				};
				break;

			case CMD_ANON_PRESET_STANDARD:
				mask_ipv4 = 24;
				mask_ipv6 = 48;
				mask_iid = 1;
				break;

			case CMD_ANON_PRESET_CAREFUL:
				mask_ipv4 = 20;
				mask_ipv6 = 40;
				mask_iid = 1;
				break;

			case CMD_ANON_PRESET_PARANOID:
				mask_ipv4 = 16;
				mask_ipv6 = 32;
				mask_iid = 1;
				break;

			default:
				ipv6loganon_printinfo();
				exit(EXIT_FAILURE);
				break;
		};
	};
	argv += optind;
	argc -= optind;

	if (file_out_flag == 1) {
		if (ipv6calc_debug > 0) {
			fprintf(stderr, "Output file specified: %s\n", file_out);
		};

		FILE_OUT = fopen(file_out, file_out_mode);
		if (! FILE_OUT) {
			fprintf(stderr, "Can't open Output file: %s\n", file_out);
			exit(EXIT_FAILURE);
		} else {
			if (ipv6calc_debug > 0) {
				if (strcmp(file_out_mode, "a") == 0) {
					fprintf(stderr, "Output file opened successfully in append mode: %s\n", file_out);
				} else {
					fprintf(stderr, "Output file opened successfully in write mode: %s\n", file_out);
				};
			};
			file_out_flag = 2;
		};
	};

	lineparser();

	if (file_out_flag == 2) {
		if (ipv6calc_debug > 0) {
			fprintf(stderr, "Output file is closed now: %s\n", file_out);
		};
		fflush(FILE_OUT);
		fclose(FILE_OUT);
	} else {
		fflush(stdout);
	};

	exit(EXIT_SUCCESS);
};
#undef DEBUG_function_name


/*
 * Line parser
 */
#define DEBUG_function_name "ipv6loganon/lineparser"
static void lineparser(void) {
	char linebuffer[LINEBUFFER];
	char token[LINEBUFFER];
	char resultstring[LINEBUFFER];
	char *charptr, *cptr, **ptrptr;
	int linecounter = 0, retval, i;

	ptrptr = &cptr;
	
	if (flag_quiet == 0) {
		fprintf(stderr, "Expecting log lines on stdin\n");
	};

	while (1 == 1) {
		/* read line from stdin */
		charptr = fgets(linebuffer, LINEBUFFER, stdin);
		
		if (charptr == NULL) {
			/* end of input */
			break;
		};

		linecounter++;

		if (linecounter == 1) {
			if (flag_quiet == 0) {
				fprintf(stderr, "Ok, proceeding stdin...\n");
			};
		};
		
		if (ipv6calc_debug == 1) {
			fprintf(stderr, "Line: %d\r", linecounter);
		};

		if (strlen(linebuffer) >= LINEBUFFER) {
			fprintf(stderr, "Line too long: %d\n", linecounter);
			continue;
		};
		
		if (strlen(linebuffer) == 0) {
			fprintf(stderr, "Line empty: %d\n", linecounter);
			continue;
		};
		
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "%s: Got line: '%s'\n", DEBUG_function_name, linebuffer);
		};

		/* look for first token */
		charptr = strtok_r(linebuffer, " \t\n", ptrptr);
		
		if ( charptr == NULL ) {
			fprintf(stderr, "Line contains no token: %d\n", linecounter);
			continue;
		};

		if ( strlen(charptr) >=  LINEBUFFER) {
			fprintf(stderr, "Line too strange: %d\n", linecounter);
			continue;
		};

		snprintf(token, sizeof(token) - 1, "%s", charptr);
		
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "%s: Token 1: '%s'\n", DEBUG_function_name, token);
		};
		
		/* call anonymizer now */
		retval = anonymizetoken(resultstring, charptr);

		if (retval != 0) {
			continue;
		};
		
		/* print result and rest of line, if available */
		if (*ptrptr[0] != '\0') {
			if (file_out_flag == 2) {
				fprintf(FILE_OUT, "%s %s", resultstring, *ptrptr);
			} else {
				printf("%s %s", resultstring, *ptrptr);
			};
		} else {
			if (file_out_flag == 2) {
				fprintf(FILE_OUT, "%s\n", resultstring);
			} else {
				printf("%s\n", resultstring);
			};
		};

		if (file_out_flush == 1) {
			if (file_out_flag == 2) {
				fflush(FILE_OUT);
			} else {
				fflush(stdout);
			};
		};
	};

	if (flag_quiet == 0) {
		fprintf(stderr, "...finished\n");

		if (flag_nocache == 0) {
			fprintf(stderr, "Cache statistics:\n");
			for (i = 0; i < cache_lru_limit; i++) {
				fprintf(stderr, "Cache distance: %3d  hits: %8ld\n", i, cache_lru_statistics[i]);
			};
		};
	};
	return;
};
#undef DEBUG_function_name


/*
 * Anonymize token
 */
#define DEBUG_function_name "ipv6loganon/anonymizetoken"
static int anonymizetoken(char *resultstring, const char *token) {
	long int inputtype = -1;
	int retval = 1, i;

	/* used structures */
	ipv6calc_ipv6addr ipv6addr;
	ipv6calc_ipv4addr ipv4addr;

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Token: '%s'\n", DEBUG_function_name, token);
	};

       	/* clear resultstring */
	resultstring[0] = '\0';

	if (strlen(token) == 0) {
		return (1);
	};

	/* use cache ? */
	if (flag_nocache == 0 && cache_lru_max > 0) {
		/* check last seen one first */
		if ((ipv6calc_debug & 0x4) != 0) {
			fprintf(stderr, "LRU cache: look for key=%s\n", token);
		};

		if (strcmp(cache_lru_key_token[cache_lru_last - 1], token) == 0) {
			snprintf(resultstring, LINEBUFFER - 1, "%s", cache_lru_value[cache_lru_last - 1]);
			cache_lru_statistics[0]++;
			if ((ipv6calc_debug & 0x4) != 0) {
				fprintf(stderr, "LRU cache: hit last line=%d key_token=%s value=%s\n", cache_lru_last - 1, token, resultstring);
			};
			return (0);
		};
		/* run backwards to first entry */
		if (cache_lru_last > 1) {
			for (i = cache_lru_last - 1; i > 0; i--) {
				if (strcmp(cache_lru_key_token[i - 1], token) == 0) {
					snprintf(resultstring, LINEBUFFER - 1, "%s", cache_lru_value[i - 1]);
					cache_lru_statistics[cache_lru_last - i]++;
					if ((ipv6calc_debug & 0x4) != 0) {
						fprintf(stderr, "LRU cache: hit line=%d key_token=%s value=%s\n", i - 1, token, resultstring);
					};
					return (0);
				};
			};
		};
		/* round robin */ 
		if (cache_lru_last < cache_lru_max) {
			for (i = cache_lru_max; i > cache_lru_last; i--) {
				if (strcmp(cache_lru_key_token[i - 1], token) == 0) {
					snprintf(resultstring, LINEBUFFER - 1, "%s", cache_lru_value[i - 1]);
					cache_lru_statistics[cache_lru_max - i + cache_lru_last]++;
					if ((ipv6calc_debug & 0x4) != 0) {
						fprintf(stderr, "LRU cache: hit line=%d key_token=%s value=%s\n", i - 1, token, resultstring);
					};
					return (0);
				};
			};
		};
	};


	/* set addresses to invalid */
	ipv6addr.flag_valid = 0;
	ipv4addr.flag_valid = 0;
	
	/* autodetection */
	inputtype = libipv6calc_autodetectinput(token);

	if (ipv6calc_debug != 0) {
		if (inputtype >= 0) {
			for (i = 0; i < (int) (sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0])); i++) {
				if (inputtype == ipv6calc_formatstrings[i].number) {
					fprintf(stderr, "%s: Found type: %s\n", DEBUG_function_name, ipv6calc_formatstrings[i].token);
				};
				break;
			};
		} else {
			fprintf(stderr, "%s: Input type unknown\n", DEBUG_function_name);
			return (1);
		};
	};

	/* proceed input depending on type */	
	switch (inputtype) {
		case FORMAT_ipv6addr:
			retval = addr_to_ipv6addrstruct(token, resultstring, &ipv6addr);
			break;

		case FORMAT_ipv4addr:
			retval = addr_to_ipv4addrstruct(token, resultstring, &ipv4addr);
			break;
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Token: '%s'\n", DEBUG_function_name, token);
	};

	/***** postprocessing input *****/
	
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Start of postprocessing input\n", DEBUG_function_name);
	};

	if (ipv6addr.flag_valid == 1) {
		/* anonymize IPv6 address according to settings */
		libipv6addr_anonymize(&ipv6addr, mask_iid, mask_ipv6, mask_ipv4);

		/* convert IPv6 address structure to string */
		ipv6addrstruct_to_compaddr(&ipv6addr, resultstring);

	} else if (ipv4addr.flag_valid == 1) {
		/* anonymize IPv6 address according to settings */
		libipv4addr_anonymize(&ipv4addr, mask_ipv4);

		/* convert IPv4 address structure to string */
		libipv4addr_ipv4addrstruct_to_string(&ipv4addr, resultstring, 0);

	} else {
		/* probably reverse DNS resolving lookup string, do not touch */
		snprintf(resultstring, LINEBUFFER - 1, "%s", token);
		return (0);
	};

	/* use cache ? */
	if (flag_nocache == 0) {
		/* calculate pointer */
		if (cache_lru_max < cache_lru_limit) {
			cache_lru_last++;
			cache_lru_max++;
		} else {
			if (cache_lru_last == cache_lru_limit) {
				cache_lru_last = 1;
			} else {
				cache_lru_last++;
			};

		};

		/* store key and value */
		snprintf(cache_lru_key_token[cache_lru_last - 1], NI_MAXHOST - 1, "%s", token);
		snprintf(cache_lru_value[cache_lru_last - 1], NI_MAXHOST - 1, "%s", resultstring);
		if ((ipv6calc_debug & 0x4) != 0) {
			fprintf(stderr, "LRU cache: fill line=%d key_token=%s value=%s\n", cache_lru_last - 1, cache_lru_key_token[cache_lru_last - 1], cache_lru_value[cache_lru_last - 1]);
		};
	};

	return (0);
};
#undef DEBUG_function_name

