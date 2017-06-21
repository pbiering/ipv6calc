/*
 * Project    : ipv6calc
 * File       : ipv6loganon.c
 * Version    : $Id$
 * Copyright  : 2007-2017 by Peter Bieringer <pb (at) bieringer.de>
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
#include "libeui64.h"

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

long int ipv6calc_debug = 0; // ipv6calc_debug usage ok
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
static int anonymizetoken(char *result, const size_t resultstring_length, const char *token);
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
int main(int argc,char *argv[]) {
	/* check compatibility */
#ifdef SHARED_LIBRARY
	IPV6CALC_LIB_VERSION_CHECK_EXIT(IPV6CALC_PACKAGE_VERSION_NUMERIC, IPV6CALC_PACKAGE_VERSION_STRING)
	IPV6CALC_DB_LIB_VERSION_CHECK_EXIT(IPV6CALC_PACKAGE_VERSION_NUMERIC, IPV6CALC_PACKAGE_VERSION_STRING)
#endif // SHARED_LIBRARY

	int i, lop, result;
	uint32_t command = 0;

	/* options */
	struct option longopts[IPV6CALC_MAXLONGOPTIONS];
	char   shortopts[NI_MAXHOST] = "";
	int    longopts_maxentries = 0;
	extern int optopt;

	ipv6calc_quiet = 1; //default for ipv6loganon

	/* initialize debug value from environment for bootstrap debugging */
	ipv6calc_debug_from_env(); // ipv6calc_debug usage ok

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

	/* initialize options from environment */
	ipv6calc_common_options_from_env(longopts, &ipv6calc_anon_set);

	/* Fetch the command-line arguments. */
	while ((i = getopt_long(argc, argv, shortopts, longopts, &lop)) != EOF) {
		DEBUGPRINT_WA(DEBUG_ipv6loganon_general, "Parsing option: 0x%08x (%d: %s)", i, optind, argv[optind - 1]);

		if ((i == '?') && (strcmp(argv[optind - 1], "-?") != 0)) {
			exit(EXIT_FAILURE);
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
					snprintf(file_out, sizeof(file_out), "%s", optarg);
					file_out_flag = 1;
				} else {
					fprintf(stderr, " Output file too long: %s\n", optarg);
					exit(EXIT_FAILURE);
				};

				switch (i) {
					case 'w':
						snprintf(file_out_mode, sizeof(file_out_mode), "%s", "w");
						break;
					case 'a':
						snprintf(file_out_mode, sizeof(file_out_mode), "%s", "a");
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
	result = libipv6calc_db_wrapper_init("");
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

	if ((command & CMD_printversion) != 0) {
		if ((command & CMD_printversion_verbose) != 0) {
			printversion_verbose(((command & CMD_printversion_verbose2) !=0) ? LEVEL_VERBOSE2 : LEVEL_VERBOSE);
		} else {
			printversion();
		};

		if ((command & CMD_printhelp) != 0) {
			printversion_help();
		};

		exit(EXIT_SUCCESS);
	};

	if ((command & CMD_printhelp) != 0) {
		ipv6loganon_printhelp();
		exit(EXIT_FAILURE);
	};

	/* check requirements */
	if (libipv6calc_anon_supported(&ipv6calc_anon_set) == 0) {
		fprintf(stderr, "ipv6calc anonymization method not supported\n");
		exit(EXIT_FAILURE);
	};

	if (file_out_flag == 1) {
		DEBUGPRINT_WA(DEBUG_ipv6loganon_general, "Output file specified: %s", file_out);

		FILE_OUT = fopen(file_out, file_out_mode);
		if (! FILE_OUT) {
			fprintf(stderr, "Can't open Output file: %s\n", file_out);
			exit(EXIT_FAILURE);
		} else {
			DEBUGPRINT_WA(DEBUG_ipv6loganon_general, "Output file opened successfully in %s mode: %s", (strcmp(file_out_mode, "a") == 0) ? "append" : "write", file_out);
			file_out_flag = 2;
		};
	};

	lineparser();

	if (file_out_flag == 2) {
		DEBUGPRINT_WA(DEBUG_ipv6loganon_general, "Output file is closed now: %s", file_out);
		fflush(FILE_OUT);
		fclose(FILE_OUT);
	} else {
		fflush(stdout);
	};

	libipv6calc_db_wrapper_cleanup();

	exit(EXIT_SUCCESS);
};


/*
 * Line parser
 */
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
	
		DEBUGPRINT_WA(DEBUG_ipv6loganon_general, "Line number: %d", linecounter);

		if (strlen(linebuffer) >= LINEBUFFER) {
			fprintf(stderr, "Line too long: %d\n", linecounter);
			continue;
		};
		
		if (strlen(linebuffer) == 0) {
			fprintf(stderr, "Line empty: %d\n", linecounter);
			continue;
		};
		
		DEBUGPRINT_WA(DEBUG_ipv6loganon_general, "Got line: '%s'", linebuffer);

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

		snprintf(token, sizeof(token), "%s", charptr);
		
		DEBUGPRINT_WA(DEBUG_ipv6loganon_general, "Token 1: '%s'", token);
		
		/* call anonymizer now */
		retval = anonymizetoken(resultstring, sizeof(resultstring), charptr);

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


/*
 * Anonymize token
 */
static int anonymizetoken(char *resultstring, const size_t resultstring_length, const char *token) {
	uint32_t inputtype = FORMAT_undefined;
	int retval = 1, i;

	/* used structures */
	ipv6calc_ipv6addr  ipv6addr;
	ipv6calc_ipv4addr  ipv4addr;
	ipv6calc_macaddr   macaddr;
	ipv6calc_eui64addr eui64addr;

	DEBUGPRINT_WA(DEBUG_ipv6loganon_general, "Token: '%s'", token);

       	/* clear resultstring */
	resultstring[0] = '\0';

	if (strlen(token) == 0) {
		return (1);
	};

	/* use cache ? */
	if (flag_nocache == 0 && cache_lru_max > 0) {
		/* check last seen one first */
		DEBUGPRINT_WA(DEBUG_ipv6loganon_cache, "LRU cache: look for key=%s", token);

		if (strcmp(cache_lru_key_token[cache_lru_last - 1], token) == 0) {
			snprintf(resultstring, resultstring_length, "%s", cache_lru_value[cache_lru_last - 1]);
			cache_lru_statistics[0]++;
			DEBUGPRINT_WA(DEBUG_ipv6loganon_cache, "LRU cache: hit last line=%d key_token=%s value=%s", cache_lru_last - 1, token, resultstring);
			return (0);
		};
		/* run backwards to first entry */
		if (cache_lru_last > 1) {
			for (i = cache_lru_last - 1; i > 0; i--) {
				if (strcmp(cache_lru_key_token[i - 1], token) == 0) {
					snprintf(resultstring, resultstring_length, "%s", cache_lru_value[i - 1]);
					cache_lru_statistics[cache_lru_last - i]++;
					DEBUGPRINT_WA(DEBUG_ipv6loganon_cache, "LRU cache: hit line=%d key_token=%s value=%s", i - 1, token, resultstring);
					return (0);
				};
			};
		};
		/* round robin */ 
		if (cache_lru_last < cache_lru_max) {
			for (i = cache_lru_max; i > cache_lru_last; i--) {
				if (strcmp(cache_lru_key_token[i - 1], token) == 0) {
					snprintf(resultstring, resultstring_length, "%s", cache_lru_value[i - 1]);
					cache_lru_statistics[cache_lru_max - i + cache_lru_last]++;
					DEBUGPRINT_WA(DEBUG_ipv6loganon_cache, "LRU cache: hit line=%d key_token=%s value=%s", i - 1, token, resultstring);
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

	DEBUGSECTION_BEGIN(DEBUG_ipv6loganon_general)
		if (inputtype != FORMAT_undefined) {
			for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_formatstrings); i++) {
				if (inputtype == ipv6calc_formatstrings[i].number) {
					DEBUGPRINT_WA(DEBUG_ipv6loganon_general, "Found type: %s", ipv6calc_formatstrings[i].token);
				};
				break;
			};
		} else {
			DEBUGPRINT_NA(DEBUG_ipv6loganon_general, "Input type unknown");
			return (1);
		};
	DEBUGSECTION_END

	/* proceed input depending on type */	
	switch (inputtype) {
		case FORMAT_ipv6addr:
			retval = addr_to_ipv6addrstruct(token, resultstring, resultstring_length, &ipv6addr);
			break;

		case FORMAT_ipv4addr:
			retval = addr_to_ipv4addrstruct(token, resultstring, resultstring_length, &ipv4addr);
			break;

		case FORMAT_eui64:
			retval = libeui64_addr_to_eui64addrstruct(token, resultstring, resultstring_length, &eui64addr);
			break;

		case FORMAT_macaddr:
			retval = addr_to_macaddrstruct(token, resultstring, resultstring_length, &macaddr);
			break;
	};

	if (retval != 0) {
		fprintf(stderr, "Can't parse string: %s (%s)\n", token, resultstring);
		return 1;
	};

	DEBUGPRINT_WA(DEBUG_ipv6loganon_general, "Token: '%s'", token);

	/***** postprocessing input *****/
	
	DEBUGPRINT_NA(DEBUG_ipv6loganon_general, "Start of postprocessing input");

	if (ipv6addr.flag_valid == 1) {
		/* anonymize IPv6 address according to settings */
		libipv6addr_anonymize(&ipv6addr, &ipv6calc_anon_set);

		/* convert IPv6 address structure to string */
		ipv6addrstruct_to_compaddr(&ipv6addr, resultstring, resultstring_length);

	} else if (ipv4addr.flag_valid == 1) {
		/* anonymize IPv4 address according to settings */
		libipv4addr_anonymize(&ipv4addr, ipv6calc_anon_set.mask_ipv4, ipv6calc_anon_set.method);

		/* convert IPv4 address structure to string */
		libipv4addr_ipv4addrstruct_to_string(&ipv4addr, resultstring, resultstring_length, 0);

	} else if (eui64addr.flag_valid == 1) {
		/* anonymize EUI-64C address according to settings */
		libeui64_anonymize(&eui64addr, &ipv6calc_anon_set);

		/* convert EUI-64 address structure to string */
		libeui64_eui64addrstruct_to_string(&eui64addr, resultstring, resultstring_length, 0);

	} else if (macaddr.flag_valid == 1) {
		/* anonymize MAC address according to settings */
		libmacaddr_anonymize(&macaddr, &ipv6calc_anon_set);

		/* convert MAC address structure to string */
		libmacaddr_macaddrstruct_to_string(&macaddr, resultstring, resultstring_length, 0);

	} else {
		/* probably reverse DNS resolving lookup string, do not touch */
		snprintf(resultstring, resultstring_length, "%s", token);
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
		snprintf(cache_lru_key_token[cache_lru_last - 1], NI_MAXHOST, "%s", token);
		snprintf(cache_lru_value[cache_lru_last - 1], NI_MAXHOST, "%s", resultstring);
		DEBUGPRINT_WA(DEBUG_ipv6loganon_cache, "LRU cache: fill line=%d key_token=%s value=%s", cache_lru_last - 1, cache_lru_key_token[cache_lru_last - 1], cache_lru_value[cache_lru_last - 1]);
	};

	return (0);
};
