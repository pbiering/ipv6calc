/*
 * Project    : ipv6calc
 * File       : ipv6logconv.c
 * Version    : $Id: ipv6logconv.c,v 1.37 2014/08/28 07:17:43 ds6peter Exp $
 * Copyright  : 2002-2014 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Dedicated program for logfile conversions
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <getopt.h> 
#include <unistd.h>

#include "ipv6logconv.h"
#include "libipv6calcdebug.h"
#include "libipv6calc.h"
#include "ipv6calccommands.h"
#include "ipv6calctypes.h"
#include "ipv6logconvoptions.h"
#include "ipv6calchelp.h"
#include "ipv6logconvhelp.h"
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

long int ipv6calc_debug = 0; // ipv6calc_debug usage ok
int flag_nocache = 0;

/* supported output types:
 *  ipv6addr
 *  ipv4addr
 *  addrtype : IPv4 | IPv6
 */

/* prototypes */
static int converttoken(char *result, const size_t resultstring_length, const char *token, const long int outputtype, const int flag_skipunknown);
static void lineparser(const long int outputtype);


/* LRU cache */

#define CACHE_LRU_SIZE 200

static int      cache_lru_max = 0;
static int      cache_lru_last = 0;
static char     cache_lru_key_token[CACHE_LRU_SIZE][NI_MAXHOST];
static long int cache_lru_key_outputtype[CACHE_LRU_SIZE];
static char     cache_lru_value[CACHE_LRU_SIZE][NI_MAXHOST];
static long int cache_lru_statistics[CACHE_LRU_SIZE];

int feature_reg = 0;
int feature_ieee = 0;

/**************************************************/
/* main */
int main(int argc,char *argv[]) {
	int i, lop, result;
	unsigned long int command = 0;

	cache_lru_limit = 20; /* optimum */

	/* new option style storage */	
	uint32_t inputtype  = FORMAT_undefined, outputtype = FORMAT_undefined;
	
	/* convert storage */
	long int action = -1;

	/* options */
	struct option longopts[MAXLONGOPTIONS];
	char   shortopts[NI_MAXHOST] = "";
	int    longopts_maxentries = 0;
	extern int optopt;

	/* check for UID */
	if (getuid() == 0) {
		printversion();
		fprintf(stderr, " DON'T RUN THIS PROGRAM AS root USER!\n");
		fprintf(stderr, " This program uses insecure C string handling functions and is not full audited\n");
		fprintf(stderr, "  therefore parsing insecure and unchecked input like logfiles isn't a good choice\n");
		exit(EXIT_FAILURE);
	};

	/* initialize debug value from environment for bootstrap debugging */
	ipv6calc_debug_from_env(); // ipv6calc_debug usage ok

	/* add options */
	ipv6calc_options_add_common_basic(shortopts, sizeof(shortopts), longopts, &longopts_maxentries);
	ipv6calc_options_add(shortopts, sizeof(shortopts), longopts, &longopts_maxentries, ipv6logconv_shortopts, ipv6logconv_longopts, MAXENTRIES_ARRAY(ipv6logconv_longopts));

	if (argc <= 1) {
		ipv6logconv_printinfo();
		exit(EXIT_FAILURE);
	};

	/* Fetch the command-line arguments. */
	while ((i = getopt_long(argc, argv, shortopts, longopts, &lop)) != EOF) {
		DEBUGPRINT_WA(DEBUG_ipv6logconv_general, "Parsing option: 0x%08x", i);

		if ((i == '?') && (optopt != 0)) {
			exit(EXIT_FAILURE);
		};

		/* catch common options */
		result = ipv6calcoptions_common_basic(i, optarg, longopts);
		if (result == 0) {
			// found
			continue;
		};

		switch (i) {
			case -1:
				break;

			case 'v':
				command |= CMD_printversion;
				break;

			case 'h':
			case '?':
				command |= CMD_printhelp;
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

			case CMD_outputtype:
				DEBUGPRINT_WA(DEBUG_ipv6logconv_general, "Got output string: %s", optarg);

				if ( (strcmp(optarg, "-?") == 0) || (strcmp(optarg, "-h") == 0) || (strcmp(optarg, "--help") == 0) ) {
					command = CMD_printexamples;
					break;
				};
				
				outputtype = ipv6calctypes_checktype(optarg);
				if (outputtype == FORMAT_undefined) {
					fprintf(stderr, " Output option is unknown: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				break;

			case CMD_printexamples:
				command = CMD_printexamples;
				break;

			default:
				fprintf(stderr, "Usage: (see '%s --command -?|-h|--help' for more help)\n", PROGRAM_NAME);
				ipv6logconv_printhelp();
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

	/* check for basic database support */
	if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_REGISTRY | IPV6CALC_DB_IPV6_TO_REGISTRY | IPV6CALC_DB_CC_TO_REGISTRY) == 1) {
		feature_reg = 1;
	};

	if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IEEE_TO_INFO) == 1) {
		feature_ieee = 1;
	};

	/* do work depending on selection */
	if ((command & CMD_printversion) != 0) {
		printversion();

		if ((command & CMD_printhelp) != 0) {
			printversion_help();
		};

		exit(EXIT_SUCCESS);
	};

	/* print help handling */
	if (command == CMD_printhelp) {
		ipv6logconv_printhelp();
		exit(EXIT_FAILURE);
        } else if (command == CMD_printexamples) {
		printhelp_output_dispatcher(outputtype);
		exit(EXIT_FAILURE);
	};

	DEBUGPRINT_WA(DEBUG_ipv6logconv_general, "Debug value:%lx  command:%lx  inputtype:%lx   outputtype:%lx  action:%lx", (unsigned long) ipv6calc_debug, command, (unsigned long) inputtype, (unsigned long) outputtype, (unsigned long) action); // ipv6calc_debug usage ok
	
	if ((outputtype == FORMAT_any) && ((feature_reg == 0) || (feature_ieee == 0))) {
		fprintf(stderr, "Basic databases are missing for conversion and outputtype 'any'\n");
		exit(EXIT_FAILURE);
	} else if ((outputtype == FORMAT_ouitype) && (feature_ieee == 0)) {
		fprintf(stderr, "Basic databases are missing for conversion and outputtype 'ouitype'\n");
		exit(EXIT_FAILURE);
	} else if ((outputtype == FORMAT_addrtype) && (feature_reg == 0)) {
		fprintf(stderr, "Basic databases are missing for conversion and outputtype 'addrtype'\n");
		exit(EXIT_FAILURE);
	};

	/* call lineparser */
	lineparser(outputtype);

	exit(EXIT_SUCCESS);
};


/*
 * Line parser
 */
static void lineparser(const long int outputtype) {
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
		
		DEBUGPRINT_WA(DEBUG_ipv6logconv_processing, "Line counter: %d", linecounter);

		if (strlen(linebuffer) >= LINEBUFFER) {
			fprintf(stderr, "Line too long: %d\n", linecounter);
			continue;
		};
		
		if (strlen(linebuffer) == 0) {
			fprintf(stderr, "Line empty: %d\n", linecounter);
			continue;
		};
		
		DEBUGPRINT_WA(DEBUG_ipv6logconv_processing, "Got line: '%s'", linebuffer);

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
		
		DEBUGPRINT_WA(DEBUG_ipv6logconv_processing, "Token 1: '%s'", token);
		
		/* call converter now */
		if ( outputtype == FORMAT_any ) {
			retval = converttoken(resultstring, sizeof(resultstring), charptr, FORMAT_addrtype, 0);
		} else {
			retval = converttoken(resultstring, sizeof(resultstring), charptr, outputtype, 1);
		};

		if (retval != 0) {
			continue;
		};
		
		/* print result */
		printf("%s", resultstring);

		if (outputtype == FORMAT_any) {
			DEBUGPRINT_NA(DEBUG_ipv6logconv_processing, "Format is 'any', so look for next tokens");
			
			/* look for next token */
			charptr = strtok_r(NULL, " \t\n", ptrptr);

			if ( charptr == NULL ) {
				fprintf(stderr, "Line contains no 2nd token: %d\n", linecounter);
				goto END_line;
			};
			if ( strlen(charptr) >=  LINEBUFFER) {
				fprintf(stderr, "Line too strange: %d\n", linecounter);
				goto END_line;
			};

			DEBUGPRINT_WA(DEBUG_ipv6logconv_processing, "Token 2: '%s'", charptr);
		
			/* 	
			retval = converttoken(resultstring, token, FORMAT_addrtype, 0);
			printf(" %s", resultstring);
			*/

			/* skip this token */
			printf(" %s", charptr);
			
			/* look for next token */
			charptr = strtok_r(NULL, " \t\n", ptrptr);

			if ( charptr == NULL ) {
				fprintf(stderr, "Line contains no 3rd token: %d\n", linecounter);
				continue;
			};
			if ( strlen(charptr) >=  LINEBUFFER) {
				fprintf(stderr, "Line too strange: %d\n", linecounter);
				continue;
			};
			
			DEBUGPRINT_WA(DEBUG_ipv6logconv_processing, "Token 3: '%s'", charptr);
			retval = converttoken(resultstring, sizeof(resultstring), token, FORMAT_ouitype, 0);
			/* print result */
			printf(" %s", resultstring);
		};

END_line:
		if ((*ptrptr != NULL) && (strlen(*ptrptr) > 0)) {
			printf(" %s", *ptrptr);
		} else {;
			printf("\n");
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
 * Convert token
 */
static int converttoken(char *resultstring, const size_t resultstring_length, const char *token, const long int outputtype, const int flag_skipunknown) {
	long int inputtype = -1;
	int retval = 1, i, registry;
	uint32_t typeinfo, typeinfo_test;
	char tempstring[NI_MAXHOST];
	ipv6calc_macaddr macaddr;

	/* used structures */
	ipv6calc_ipv6addr ipv6addr;
	ipv6calc_ipv4addr ipv4addr;

	DEBUGPRINT_WA(DEBUG_ipv6logconv_processing, "Token: '%s'", token);

       	/* clear resultstring */
	resultstring[0] = '\0';

	if (strlen(token) == 0) {
		return (1);
	};

	/* use cache ? */
	if (flag_nocache == 0 && cache_lru_max > 0) {
		/* check last seen one first */
		DEBUGPRINT_WA(DEBUG_ipv6logconv_processing, "LRU cache: look for key=%s", token);

		if (cache_lru_key_outputtype[cache_lru_last - 1] == outputtype) {
			if (strcmp(cache_lru_key_token[cache_lru_last - 1], token) == 0) {
				snprintf(resultstring, resultstring_length, "%s", cache_lru_value[cache_lru_last - 1]);
				cache_lru_statistics[0]++;
				DEBUGPRINT_WA(DEBUG_ipv6logconv_processing, "LRU cache: hit last line=%d key_token=%s key_outputtype=%lx value=%s", cache_lru_last - 1, token, outputtype, resultstring);
				return (0);
			};
		} else {
			/* run backwards to first entry */
			if (cache_lru_last > 1) {
				for (i = cache_lru_last - 1; i > 0; i--) {
					if (cache_lru_key_outputtype[i - 1] == outputtype) {
						if (strcmp(cache_lru_key_token[i - 1], token) == 0) {
							snprintf(resultstring, resultstring_length, "%s", cache_lru_value[i - 1]);
							cache_lru_statistics[cache_lru_last - i]++;
							DEBUGPRINT_WA(DEBUG_ipv6logconv_processing, "LRU cache: hit line=%d key_token=%s key_outputtype=%lx value=%s", i - 1, token, outputtype, resultstring);
							return (0);
						};
					};
				};
			};
			/* round robin */ 
			if (cache_lru_last < cache_lru_max) {
				for (i = cache_lru_max; i > cache_lru_last; i--) {
					if (cache_lru_key_outputtype[i - 1] == outputtype) {
						if (strcmp(cache_lru_key_token[i - 1], token) == 0) {
							snprintf(resultstring, resultstring_length, "%s", cache_lru_value[i - 1]);
							cache_lru_statistics[cache_lru_max - i + cache_lru_last]++;
							DEBUGPRINT_WA(DEBUG_ipv6logconv_processing, "LRU cache: hit line=%d key_token=%s key_outputtype=%lx value=%s", i - 1, token, outputtype, resultstring);
							return (0);
						};
					};
				};
			};
		};
	};


	/* set addresses to invalid */
	ipv6addr.flag_valid = 0;
	ipv4addr.flag_valid = 0;
	
	/* autodetection */
	inputtype = libipv6calc_autodetectinput(token);

	DEBUGSECTION_BEGIN(DEBUG_ipv6logconv_processing)
		if (inputtype >= 0) {
			for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_formatstrings); i++) {
				if (inputtype == ipv6calc_formatstrings[i].number) {
					DEBUGPRINT_WA(DEBUG_ipv6logconv_processing, "Found type: %s", ipv6calc_formatstrings[i].token);
				};
				break;
			};
		} else {
			DEBUGPRINT_NA(DEBUG_ipv6logconv_processing, "Input type unknown");
			return (1);
		};
	DEBUGSECTION_END

	/* proceed input depending on type */	
	switch (inputtype) {
		case FORMAT_ipv6addr:
			retval = addr_to_ipv6addrstruct(token, resultstring, sizeof(resultstring), &ipv6addr);
			break;

		case FORMAT_ipv4addr:
			retval = addr_to_ipv4addrstruct(token, resultstring, sizeof(resultstring), &ipv4addr);
			break;
	};

	DEBUGPRINT_WA(DEBUG_ipv6logconv_processing, "Token: '%s'", token);

	/***** postprocessing input *****/

	DEBUGPRINT_NA(DEBUG_ipv6logconv_processing, "Start of postprocessing input");

	switch (outputtype) {
		case FORMAT_addrtype:
			if (ipv6addr.flag_valid == 1) {
				DEBUGPRINT_NA(DEBUG_ipv6logconv_processing, "is IPv6 address");
				snprintf(resultstring, resultstring_length, "ipv6-addr.addrtype.ipv6calc");

				/* check for registry */
				typeinfo = ipv6addr_gettype(&ipv6addr);

				/* scope of IPv6 address */
				/* init retval */
				for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addrtypestrings); i++) {
					if ( ipv6calc_ipv6addrtypestrings[i].number == IPV6_ADDR_ANY ) {
						retval = i;
						break;
					};
				};
				typeinfo_test = typeinfo & (IPV6_NEW_ADDR_AGU | IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_ADDR_MAPPED | IPV6_ADDR_COMPATv4 | IPV6_ADDR_ULUA);
				if ( typeinfo_test != 0 ) {
					/* get string */
					for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addrtypestrings); i++) {
						if ( (typeinfo_test & ipv6calc_ipv6addrtypestrings[i].number) != 0 ) {
							retval = i;
							break;
						};
					};
				};
				snprintf(tempstring, sizeof(tempstring), "%s.%s", ipv6calc_ipv6addrtypestrings[retval].token, resultstring);
				snprintf(resultstring, resultstring_length,"%s", tempstring);

				/* type of global IPv6 address */
				/* init retval */
				for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addrtypestrings); i++) {
					if ( ipv6calc_ipv6addrtypestrings[i].number == IPV6_ADDR_ANY ) {
						retval = i;
						break;
					};
				};
				typeinfo_test = typeinfo & (IPV6_NEW_ADDR_6TO4 | IPV6_NEW_ADDR_6BONE | IPV6_NEW_ADDR_PRODUCTIVE);
				if ( typeinfo_test != 0 ) {
					if ((typeinfo & IPV6_NEW_ADDR_TEREDO) != 0) {
						snprintf(tempstring, sizeof(tempstring), "teredo.%s", resultstring);
						snprintf(resultstring, resultstring_length, "%s", tempstring);
					} else {
						/* get string */
						for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv6addrtypestrings); i++) {
							if ( (typeinfo_test & ipv6calc_ipv6addrtypestrings[i].number) != 0 ) {
								retval = i;
								break;
							};
						};
						snprintf(tempstring, sizeof(tempstring), "%s.%s", ipv6calc_ipv6addrtypestrings[retval].token, resultstring);
						snprintf(resultstring, resultstring_length, "%s", tempstring);
					};
				};

				/* registry of IPv6 address */
				if ( ( (typeinfo & (IPV6_NEW_ADDR_6BONE | IPV6_NEW_ADDR_PRODUCTIVE) ) != 0) && ( (typeinfo & (IPV6_NEW_ADDR_TEREDO)) == 0)) {
					registry = libipv6addr_registry_num_by_addr(&ipv6addr);
					snprintf(tempstring, sizeof(tempstring), "%s.%s", libipv6calc_registry_string_by_num(registry), resultstring);
					snprintf(resultstring, resultstring_length, "%s", tempstring);
				} else if ( (typeinfo & (IPV6_NEW_ADDR_6TO4 | IPV6_ADDR_MAPPED | IPV6_ADDR_COMPATv4 | IPV6_NEW_ADDR_TEREDO)) != 0 ) {
					DEBUGPRINT_NA(DEBUG_ipv6logconv_processing, "IPv6 has IPv4 included");

					/* fill IPv4 address */
					if ( (typeinfo & (IPV6_ADDR_MAPPED | IPV6_ADDR_COMPATv4)) != 0 ) {
						for (i = 0; i <= 3; i++) {
							ipv4addr_setoctet(&ipv4addr, i, ipv6addr_getoctet(&ipv6addr, i + 12));
						};
					} else if ( (typeinfo & IPV6_NEW_ADDR_6TO4) != 0 ) {
						for (i = 0; i <= 3; i++) {
							ipv4addr_setoctet(&ipv4addr, i, ipv6addr_getoctet(&ipv6addr, i + 2));
						};
					} else if ( (typeinfo & IPV6_NEW_ADDR_TEREDO) != 0 ) {
						for (i = 0; i <= 3; i++) {
							ipv4addr_setoctet(&ipv4addr, i, ipv6addr_getoctet(&ipv6addr, i + 12) ^ 0xff);
						};
					} else {
						/* normally never happen */
						fprintf(stderr, "%s/%s: Error occurs at IPv6->IPv4 address extraction!\n", __FILE__, __func__);
						exit(EXIT_FAILURE);
					};

					ipv4addr.scope = ipv4addr_gettype(&ipv4addr);

					/* IPv4 registry */
					registry = libipv4addr_registry_num_by_addr(&ipv4addr);
					snprintf(tempstring, sizeof(tempstring), "%s.%s", libipv6calc_registry_string_by_num(registry), resultstring);
					snprintf(resultstring, resultstring_length, "%s", tempstring);
				};
			} else if (ipv4addr.flag_valid == 1) {
				snprintf(resultstring, resultstring_length, "ipv4-addr.addrtype.ipv6calc");

				/* IPv4 registry */
				registry = libipv4addr_registry_num_by_addr(&ipv4addr);
				snprintf(tempstring, sizeof(tempstring), "%s.%s", libipv6calc_registry_string_by_num(registry), resultstring);
				snprintf(resultstring, resultstring_length, "%s", tempstring);
			} else {
				snprintf(resultstring, resultstring_length, "reverse-lookup-successful.addrtype.ipv6calc");
			};
			break;

		case FORMAT_ouitype:
			if (ipv6addr.flag_valid != 1) {
				if (flag_skipunknown != 0) {
					return (1);
				} else {
					snprintf(resultstring, resultstring_length, "not-ipv6.ouitype.ipv6calc");
					return (0);
				};
		       	};

			typeinfo = ipv6addr_gettype(&ipv6addr);

			/* check whether address has a OUI ID */
			if ( (( typeinfo & (IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_NEW_ADDR_6BONE | IPV6_NEW_ADDR_6TO4 | IPV6_ADDR_ULUA)) == 0) && ((typeinfo & (IPV6_NEW_ADDR_TEREDO)) == 0) )  {
				if (flag_skipunknown != 0) {
					return (1);
				} else {
					snprintf(resultstring, resultstring_length, "unresolvable.ouitype.ipv6calc");
					return (0);
				};
		       	};

			if ((ipv6addr_getoctet(&ipv6addr, 8) & 0x02) != 0) {
				macaddr.addr[0] = ipv6addr_getoctet(&ipv6addr, 8) ^0x02;
				macaddr.addr[1] = ipv6addr_getoctet(&ipv6addr, 9);
				macaddr.addr[2] = ipv6addr_getoctet(&ipv6addr, 10);
				macaddr.addr[3] = ipv6addr_getoctet(&ipv6addr, 13);
				macaddr.addr[4] = ipv6addr_getoctet(&ipv6addr, 14);
				macaddr.addr[5] = ipv6addr_getoctet(&ipv6addr, 15);

				retval = libipv6calc_db_wrapper_ieee_vendor_string_short_by_macaddr(resultstring, sizeof(resultstring), &macaddr);
				if (retval != 0) {
					if (flag_skipunknown != 0) {
						return (1);
					} else {
						snprintf(resultstring, resultstring_length, "unresolvable.ouitype.ipv6calc");
						return (0);
					};
				};
				if (strlen(resultstring) == 0) {
					snprintf(resultstring, resultstring_length, "unknown.ouitype.ipv6calc");
				} else {
					snprintf(tempstring, sizeof(tempstring), "%s.ouitype.ipv6calc", resultstring);
					snprintf(resultstring, resultstring_length, "%s", tempstring);
				};
			} else {
				if ( (typeinfo & IPV6_NEW_ADDR_6TO4_MICROSOFT) != 0 ) {
					snprintf(resultstring, resultstring_length, "6to4-microsoft.ouitype.ipv6calc");
				} else if ( (typeinfo & IPV6_NEW_ADDR_IID_ISATAP) != 0 ) {
					snprintf(resultstring, resultstring_length, "ISATAP.ouitype.ipv6calc");
				} else if ( (typeinfo & IPV6_NEW_ADDR_IID_RANDOM) != 0 ) {
					snprintf(resultstring, resultstring_length, "local-scope-random.ouitype.ipv6calc");
				} else if ( (typeinfo & IPV6_NEW_ADDR_TEREDO) != 0 ) {
					snprintf(resultstring, resultstring_length, "local-scope-teredo.ouitype.ipv6calc");
				} else {
					snprintf(resultstring, resultstring_length, "local-scope.ouitype.ipv6calc");
				};
			};
			break;
			
		case FORMAT_ipv6addrtype:
			if (ipv6addr.flag_valid != 1) {
				if (flag_skipunknown != 0) {
					return (1);
				} else {
					snprintf(resultstring, resultstring_length, "not-ipv6.ipv6addrtype.ipv6calc");
					return (0);
				};
		       	};

			typeinfo = ipv6addr_gettype(&ipv6addr);

		       	if ( (typeinfo & IPV6_ADDR_LINKLOCAL) != 0 ) {
				snprintf(resultstring, resultstring_length, "link-local.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_ADDR_SITELOCAL) != 0 ) {
				snprintf(resultstring, resultstring_length, "site-local.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_ADDR_ULUA) != 0 ) {
				snprintf(resultstring, resultstring_length, "unique-local.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_NEW_ADDR_TEREDO) != 0 ) {
				if ( (typeinfo & IPV6_NEW_ADDR_6BONE) != 0 ) {
					snprintf(resultstring, resultstring_length, "teredo.6bone-global.ipv6addrtype.ipv6calc");
				} else {
					snprintf(resultstring, resultstring_length, "teredo.unknown-global.ipv6addrtype.ipv6calc");
				};
			} else if ( (typeinfo & IPV6_NEW_ADDR_6BONE) != 0 ) {
				snprintf(resultstring, resultstring_length, "6bone-global.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_NEW_ADDR_6TO4) != 0 ) {
				snprintf(resultstring, resultstring_length, "6to4-global.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_NEW_ADDR_PRODUCTIVE) != 0 ) {
				snprintf(resultstring, resultstring_length, "productive-global.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_ADDR_MAPPED) != 0 ) {
				snprintf(resultstring, resultstring_length, "mapped-ipv4.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_ADDR_COMPATv4) != 0 ) {
				snprintf(resultstring, resultstring_length, "compat-ipv4.ipv6addrtype.ipv6calc");
			} else {
				snprintf(resultstring, resultstring_length, "unknown-ipv6.ipv6addrtype.ipv6calc");
			};
			break;

		default:
			fprintf(stderr, " Outputtype isn't implemented\n");
			return (1);
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
		cache_lru_key_outputtype[cache_lru_last - 1] = outputtype;
		snprintf(cache_lru_value[cache_lru_last - 1], NI_MAXHOST - 1, "%s", resultstring);
		DEBUGPRINT_WA(DEBUG_ipv6logconv_processing, "LRU cache: fill line=%d key_token=%s key_outputtype=%lx value=%s", cache_lru_last - 1, cache_lru_key_token[cache_lru_last - 1], cache_lru_key_outputtype[cache_lru_last - 1], cache_lru_value[cache_lru_last - 1]);
	};

	return (0);
};
