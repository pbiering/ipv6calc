/*
 * Project    : ipv6calc
 * File       : ipv6loganon.c
 * Version    : $Id: ipv6loganon.c,v 1.26 2013/10/28 20:10:17 ds6peter Exp $
 * Copyright  : 2007-2013 by Peter Bieringer <pb (at) bieringer.de>
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

#include "ipv6calcoptions.h"

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

#include "../databases/lib/libipv6calc_db_wrapper.h"

#define LINEBUFFER	16384

long int ipv6calc_debug = 0;
int flag_nocache = 0;

/* anonymization default values */
s_ipv6calc_anon_set ipv6calc_anon_set;
int mask_ipv4;
int mask_ipv6;
int mask_iid;
int mask_mac;

/* features */
int feature_zeroize = 1; // always supported
int feature_anon    = 1; // always supported
int feature_kp      = 0; // will be checked later


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


void printversion_verbose(const int level) {
	printversion();
	fprintf(stderr, "\n");
	ipv6calc_print_features_verbose(level);
};

/**************************************************/
/* main */
#define DEBUG_function_name "ipv6loganon/main"
int main(int argc,char *argv[]) {
	int i, lop, result;
	uint32_t command = 0;

	/* options */
	struct option longopts[MAXLONGOPTIONS];
	char   shortopts[NI_MAXHOST] = "";
	int    longopts_maxentries = 0;

	ipv6calc_quiet = 1; //default for ipv6loganon

	/* initialize debug value from environment for bootstrap debugging */
	ipv6calc_debug_from_env();

	/* add options */
	ipv6calc_options_add_common_anon(shortopts, sizeof(shortopts), longopts, &longopts_maxentries);
	ipv6calc_options_add_common_basic(shortopts, sizeof(shortopts), longopts, &longopts_maxentries);
	ipv6calc_options_add(shortopts, sizeof(shortopts), longopts, &longopts_maxentries, ipv6loganon_shortopts, ipv6loganon_longopts, MAXENTRIES_ARRAY(ipv6loganon_longopts));

	/* default */
	result = libipv6calc_anon_set_by_name(&ipv6calc_anon_set, "anonymize-standard");
	if (result != 0) {
		fprintf(stderr, "major problem, ipv6calc anonymization default preset not found: anonymize-standard\n");
		exit(EXIT_FAILURE);
	};

	/* Fetch the command-line arguments. */
	while ((i = getopt_long(argc, argv, shortopts, longopts, &lop)) != EOF) {
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "%s/%s: Parsing option: 0x%08x\n", __FILE__, __func__, i);
		};

		/* catch common options */
		result = ipv6calcoptions_common_basic(i, optarg, longopts);
		if (result == 0) {
			// found
			continue;
		};

		/* catch "common anon" options */
		result = ipv6calcoptions_common_anon(i, optarg, longopts, &ipv6calc_anon_set);
		if (result == 0) {
			// found
			continue;
		};

		switch (i) {
			case -1:
				break;

			case 'v':
				if ((command & CMD_printversion_verbose) != 0) {
					// 3rd time '-v'
					command |= CMD_printversion_verbose2;
				} else if ((command & CMD_printversion) != 0) {
					// 2nd time '-v'
					command |= CMD_printversion_verbose;
				} else {
					command |= CMD_printversion;
				};
				break;

			case 'V':
				ipv6calc_quiet = 0;
				break;

			case 'h':
			case '?':
				command |= CMD_printhelp;
				break;
				
			case 'f':
				file_out_flush = 1;
				break;

			case 'w':
			case 'a':
				if (strlen(optarg) < sizeof(file_out)) {
					snprintf(file_out, sizeof(file_out) - 1, "%s", optarg);
					file_out_flag = 1;
				} else {
					fprintf(stderr, " Output file too long: %s\n", optarg);
					exit(EXIT_FAILURE);
				};

				switch (i) {
					case 'w':
						snprintf(file_out_mode, sizeof(file_out_mode) - 1, "%s", "w");
						break;
					case 'a':
						snprintf(file_out_mode, sizeof(file_out_mode) - 1, "%s", "a");
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

			default:
				ipv6loganon_printinfo();
				exit(EXIT_FAILURE);
				break;
		};
	};
	argv += optind;
	argc -= optind;

	/* initialise database wrapper */
	result = libipv6calc_db_wrapper_init();
	if (result != 0) {
		exit(EXIT_FAILURE);
	};

	/* check for KeepTypeAsnCC support */
	if ((libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV4_REQ_DB) == 1) \
	    && (libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV6_REQ_DB) == 1)) {
		feature_kp = 1;
	};

	/* do work depending on selection */
	DEBUGPRINT_WA(DEBUG_ipv6loganon_general, "command=0x%08x", command);

	if ((command & CMD_printhelp) != 0) {
		ipv6loganon_printhelp();
		exit(EXIT_FAILURE);
	};

	if ((command & CMD_printversion) != 0) {
		if ((command & CMD_printversion_verbose) != 0) {
			printversion_verbose(((command & CMD_printversion_verbose2) !=0) ? LEVEL_VERBOSE2 : LEVEL_VERBOSE);
		} else {
			printversion();
		};
		exit(EXIT_SUCCESS);
	};

	/* check requirements */
	if (libipv6calc_anon_supported(&ipv6calc_anon_set) == 0) {
		fprintf(stderr, "ipv6calc anonymization method not supported\n");
		exit(EXIT_FAILURE);
	};

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
	
	if (ipv6calc_quiet == 0) {
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
			if (ipv6calc_quiet == 0) {
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

	if (ipv6calc_quiet == 0) {
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
	ipv6calc_macaddr  macaddr;

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

		case FORMAT_macaddr:
			retval = addr_to_macaddrstruct(token, resultstring, &macaddr);
			break;
	};

	if (retval != 0) {
		fprintf(stderr, "Can't parse string: %s (%s)\n", token, resultstring);
		return 1;
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
		libipv6addr_anonymize(&ipv6addr, &ipv6calc_anon_set);

		/* convert IPv6 address structure to string */
		ipv6addrstruct_to_compaddr(&ipv6addr, resultstring);

	} else if (ipv4addr.flag_valid == 1) {
		/* anonymize IPv4 address according to settings */
		libipv4addr_anonymize(&ipv4addr, ipv6calc_anon_set.mask_ipv4, ipv6calc_anon_set.method);

		/* convert IPv4 address structure to string */
		libipv4addr_ipv4addrstruct_to_string(&ipv4addr, resultstring, 0);

	} else if (macaddr.flag_valid == 1) {
		/* anonymize MAC address according to settings */
		libmacaddr_anonymize(&macaddr, ipv6calc_anon_set.mask_mac);

		/* convert MAC address structure to string */
		libmacaddr_macaddrstruct_to_string(&macaddr, resultstring, 0);

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

