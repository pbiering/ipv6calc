/*
 * Project    : ipv6calc
 * File       : ipv6logconv.c
 * Version    : $Id: ipv6logconv.c,v 1.18 2011/11/03 06:48:11 peter Exp $
 * Copyright  : 2002-2005 by Peter Bieringer <pb (at) bieringer.de>
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
int flag_quiet = 0;
int flag_nocache = 0;

/* supported output types:
 *  ipv6addr
 *  ipv4addr
 *  addrtype : IPv4 | IPv6
 */

/* prototypes */
static int converttoken(char *result, const char *token, const long int outputtype, const int flag_skipunknown);
static void lineparser(const long int outputtype);


/* LRU cache */

#define CACHE_LRU_SIZE 200

static int      cache_lru_max = 0;
static int      cache_lru_last = 0;
int      cache_lru_limit = 20; /* optimum */
static char     cache_lru_key_token[CACHE_LRU_SIZE][NI_MAXHOST];
static long int cache_lru_key_outputtype[CACHE_LRU_SIZE];
static char     cache_lru_value[CACHE_LRU_SIZE][NI_MAXHOST];
static long int cache_lru_statistics[CACHE_LRU_SIZE];


/**************************************************/
/* main */
#define DEBUG_function_name "ipv6logconv/main"
int main(int argc,char *argv[]) {
	int i, lop;
	unsigned long int command = 0;


	/* new option style storage */	
	uint32_t inputtype  = FORMAT_undefined, outputtype = FORMAT_undefined;
	
	/* convert storage */
	long int action = -1;

	/* check for UID */
	if (getuid() == 0) {
		printversion();
		fprintf(stderr, " DON'T RUN THIS PROGRAM AS root USER!\n");
		fprintf(stderr, " This program uses insecure C string handling functions and is not full audited\n");
		fprintf(stderr, "  therefore parsing insecure and unchecked input like logfiles isn't a good choice\n");
		exit(EXIT_FAILURE);
	};

	if (argc <= 1) {
		ipv6logconv_printinfo();
		exit(EXIT_FAILURE);
	};

	/* Fetch the command-line arguments. */
	while ((i = getopt_long(argc, argv, ipv6logconv_shortopts, ipv6logconv_longopts, &lop)) != EOF) {
		switch (i) {
			case -1:
				break;

			case 'v':
				command |= CMD_printversion;
				break;

			case 'q':
				flag_quiet = 1;
				break;

			case 'h':
			case '?':
				command |= CMD_printhelp;
				break;
				
			case 'd':
				ipv6calc_debug = atol(optarg);
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
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got output string: %s\n", DEBUG_function_name, optarg);
				};
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
				printhelp();
				break;
		};
	};
	argv += optind;
	argc -= optind;

	/* print help handling */
	if (command == CMD_printhelp) {
		ipv6logconv_printhelp();
		exit(EXIT_FAILURE);
        } else if (command == CMD_printexamples) {
		printhelp_output_dispatcher(outputtype);
		exit(EXIT_FAILURE);
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Debug value:%lx  command:%lx  inputtype:%lx   outputtype:%lx  action:%lx\n", (unsigned long) ipv6calc_debug, command, (unsigned long) inputtype, (unsigned long) outputtype, (unsigned long) action); 
	};
	
	/* do work depending on selection */
	if (command == CMD_printversion) {
		printversion();
		exit(EXIT_FAILURE);
	};

	/* call lineparser */
	lineparser(outputtype);

	exit(EXIT_SUCCESS);
};
#undef DEBUG_function_name


/*
 * Line parser
 */
#define DEBUG_function_name "ipv6logconv/lineparser"
static void lineparser(const long int outputtype) {
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
		
		/* call converter now */
		if ( outputtype == FORMAT_any ) {
			retval = converttoken(resultstring, charptr, FORMAT_addrtype, 0);
		} else {
			retval = converttoken(resultstring, charptr, outputtype, 1);
		};

		if (retval != 0) {
			continue;
		};
		
		/* print result */
		printf("%s", resultstring);

		if ( outputtype == FORMAT_any ) {
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Format is 'any', so look for next tokens\n", DEBUG_function_name);
			};
			
			/* look for next token */
			charptr = strtok_r(NULL, " \t\n", ptrptr);

			if ( charptr == NULL ) {
				fprintf(stderr, "Line contains no 2nd token: %d\n", linecounter);
				continue;
			};
			if ( strlen(charptr) >=  LINEBUFFER) {
				fprintf(stderr, "Line too strange: %d\n", linecounter);
				continue;
			};

			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Token 2: '%s'\n", DEBUG_function_name, charptr);
			};
		
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
			
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Token 3: '%s'\n", DEBUG_function_name, charptr);
			};
			retval = converttoken(resultstring, token, FORMAT_ouitype, 0);
			/* print result */
			printf(" %s", resultstring);
		};

		/* print rest of line, if available */
		printf(" %s", *ptrptr);
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
 * Convert token
 */
#define DEBUG_function_name "ipv6logconv/converttoken"
static int converttoken(char *resultstring, const char *token, const long int outputtype, const int flag_skipunknown) {
	long int inputtype = -1;
	int retval = 1, i;
	uint32_t typeinfo, typeinfo_test;
	char tempstring[NI_MAXHOST], temp2string[NI_MAXHOST];
	ipv6calc_macaddr macaddr;

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
		if (ipv6calc_debug & 0x4) {
			fprintf(stderr, "LRU cache: look for key=%s\n", token);
		};

		if (cache_lru_key_outputtype[cache_lru_last - 1] == outputtype) {
			if (strcmp(cache_lru_key_token[cache_lru_last - 1], token) == 0) {
				snprintf(resultstring, LINEBUFFER - 1, "%s", cache_lru_value[cache_lru_last - 1]);
				cache_lru_statistics[0]++;
				if (ipv6calc_debug & 0x4) {
					fprintf(stderr, "LRU cache: hit last line=%d key_token=%s key_outputtype=%lx value=%s\n", cache_lru_last - 1, token, outputtype, resultstring);
				};
				return (0);
			};
		} else {
			/* run backwards to first entry */
			if (cache_lru_last > 1) {
				for (i = cache_lru_last - 1; i > 0; i--) {
					if (cache_lru_key_outputtype[i - 1] == outputtype) {
						if (strcmp(cache_lru_key_token[i - 1], token) == 0) {
							snprintf(resultstring, LINEBUFFER - 1, "%s", cache_lru_value[i - 1]);
							cache_lru_statistics[cache_lru_last - i]++;
							if (ipv6calc_debug & 0x4) {
								fprintf(stderr, "LRU cache: hit line=%d key_token=%s key_outputtype=%lx value=%s\n", i - 1, token, outputtype, resultstring);
							};
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
							snprintf(resultstring, LINEBUFFER - 1, "%s", cache_lru_value[i - 1]);
							cache_lru_statistics[cache_lru_max - i + cache_lru_last]++;
							if (ipv6calc_debug & 0x4) {
								fprintf(stderr, "LRU cache: hit line=%d key_token=%s key_outputtype=%lx value=%s\n", i - 1, token, outputtype, resultstring);
							};
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

	switch (outputtype) {
		case FORMAT_addrtype:
			if (ipv6addr.flag_valid == 1) {
				snprintf(resultstring, LINEBUFFER - 1, "ipv6-addr.addrtype.ipv6calc");

				/* check for registry */
				typeinfo = ipv6addr_gettype(&ipv6addr);

				/* scope of IPv6 address */
				/* init retval */
				for (i = 0; i < (size_t) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++) {
					if ( ipv6calc_ipv6addrtypestrings[i].number == IPV6_ADDR_ANY ) {
						retval = i;
						break;
					};
				};
				typeinfo_test = typeinfo & (IPV6_NEW_ADDR_AGU | IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_ADDR_MAPPED | IPV6_ADDR_COMPATv4 | IPV6_ADDR_ULUA);
				if ( typeinfo_test != 0 ) {
					/* get string */
					for (i = 0; i < (size_t) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++) {
						if ( (typeinfo_test & ipv6calc_ipv6addrtypestrings[i].number) != 0 ) {
							retval = i;
							break;
						};
					};
				};
				snprintf(tempstring, sizeof(tempstring) - 1, "%s.%s", ipv6calc_ipv6addrtypestrings[retval].token, resultstring);
				snprintf(resultstring, LINEBUFFER - 1,"%s", tempstring);

				/* type of global IPv6 address */
				/* init retval */
				for (i = 0; i < (size_t) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++) {
					if ( ipv6calc_ipv6addrtypestrings[i].number == IPV6_ADDR_ANY ) {
						retval = i;
						break;
					};
				};
				typeinfo_test = typeinfo & (IPV6_NEW_ADDR_6TO4 | IPV6_NEW_ADDR_6BONE | IPV6_NEW_ADDR_PRODUCTIVE);
				if ( typeinfo_test != 0 ) {
					if ((typeinfo & IPV6_NEW_ADDR_TEREDO) != 0) {
						snprintf(tempstring, sizeof(tempstring) - 1, "teredo.%s", resultstring);
						snprintf(resultstring, LINEBUFFER - 1, "%s", tempstring);
					} else {
						/* get string */
						for (i = 0; i < (size_t) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++) {
							if ( (typeinfo_test & ipv6calc_ipv6addrtypestrings[i].number) != 0 ) {
								retval = i;
								break;
							};
						};
						snprintf(tempstring, sizeof(tempstring) - 1, "%s.%s", ipv6calc_ipv6addrtypestrings[retval].token, resultstring);
						snprintf(resultstring, LINEBUFFER - 1, "%s", tempstring);
					};
				};

				/* registry of IPv6 address */
				if ( ( (typeinfo & (IPV6_NEW_ADDR_6BONE | IPV6_NEW_ADDR_PRODUCTIVE) ) != 0) && ( (typeinfo & (IPV6_NEW_ADDR_TEREDO)) == 0)) {
					retval = libipv6addr_get_registry_string(&ipv6addr, temp2string);
					if ( retval == 0 ) {
						snprintf(tempstring, sizeof(tempstring) - 1, "%s.%s", temp2string, resultstring);
						snprintf(resultstring, LINEBUFFER - 1, "%s", tempstring);
					};
				} else if ( (typeinfo & (IPV6_NEW_ADDR_6TO4 | IPV6_ADDR_MAPPED | IPV6_ADDR_COMPATv4 | IPV6_NEW_ADDR_TEREDO)) != 0 ) {
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
						fprintf(stderr, "%s: Error occurs at IPv6->IPv4 address extraction!\n", DEBUG_function_name);
						exit(EXIT_FAILURE);
					};

					retval = libipv4addr_get_registry_string(&ipv4addr, temp2string);
					if ( retval == 0 ) {
						/* IPv4 registry */
						snprintf(tempstring, sizeof(tempstring) - 1, "%s.%s", temp2string, resultstring);
						snprintf(resultstring, LINEBUFFER - 1, "%s", tempstring);
					};
				};
			} else if (ipv4addr.flag_valid == 1) {
				snprintf(resultstring, LINEBUFFER - 1, "ipv4-addr.addrtype.ipv6calc");

				retval = libipv4addr_get_registry_string(&ipv4addr, temp2string);
				if ( retval == 0 ) {
					/* IPv4 registry */
					snprintf(tempstring, sizeof(tempstring) - 1, "%s.%s", temp2string, resultstring);
					snprintf(resultstring, LINEBUFFER - 1, "%s", tempstring);
				};
			} else {
				snprintf(resultstring, LINEBUFFER - 1, "reverse-lookup-successful.addrtype.ipv6calc");
			};
			break;

		case FORMAT_ouitype:
			if (ipv6addr.flag_valid != 1) {
				if (flag_skipunknown != 0) {
					return (1);
				} else {
					snprintf(resultstring, LINEBUFFER - 1, "not-ipv6.ouitype.ipv6calc");
					return (0);
				};
		       	};

			typeinfo = ipv6addr_gettype(&ipv6addr);

			/* check whether address has a OUI ID */
			if ( (( typeinfo & (IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_NEW_ADDR_6BONE | IPV6_NEW_ADDR_6TO4 | IPV6_ADDR_ULUA)) == 0) && ((typeinfo & (IPV6_NEW_ADDR_TEREDO)) ==0) )  {
				if (flag_skipunknown != 0) {
					return (1);
				} else {
					snprintf(resultstring, LINEBUFFER - 1, "unresolvable.ouitype.ipv6calc");
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

				retval = libieee_get_short_vendor_string(resultstring, &macaddr);
				if (retval != 0) {
					if (flag_skipunknown != 0) {
						return (1);
					} else {
						snprintf(resultstring, LINEBUFFER - 1, "unresolvable.ouitype.ipv6calc");
						return (0);
					};
				};
				if (strlen(resultstring) == 0) {
					snprintf(resultstring, LINEBUFFER - 1, "unknown.ouitype.ipv6calc");
				} else {
					snprintf(tempstring, sizeof(tempstring) - 1, "%s.ouitype.ipv6calc", resultstring);
					snprintf(resultstring, LINEBUFFER - 1, "%s", tempstring);
				};
			} else {
				if ( (typeinfo & IPV6_NEW_ADDR_6TO4_MICROSOFT) != 0 ) {
					snprintf(resultstring, LINEBUFFER - 1, "6to4-microsoft.ouitype.ipv6calc");
				} else if ( (typeinfo & IPV6_NEW_ADDR_ISATAP) != 0 ) {
					snprintf(resultstring, LINEBUFFER - 1, "ISATAP.ouitype.ipv6calc");
				} else {
					snprintf(resultstring, LINEBUFFER - 1, "local-scope.ouitype.ipv6calc");
				};
			};
			break;
			
		case FORMAT_ipv6addrtype:
			if (ipv6addr.flag_valid != 1) {
				if (flag_skipunknown != 0) {
					return (1);
				} else {
					snprintf(resultstring, LINEBUFFER - 1, "not-ipv6.ipv6addrtype.ipv6calc");
					return (0);
				};
		       	};

			typeinfo = ipv6addr_gettype(&ipv6addr);

		       	if ( (typeinfo & IPV6_ADDR_LINKLOCAL) != 0 ) {
				snprintf(resultstring, LINEBUFFER - 1, "link-local.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_ADDR_SITELOCAL) != 0 ) {
				snprintf(resultstring, LINEBUFFER - 1, "site-local.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_ADDR_ULUA) != 0 ) {
				snprintf(resultstring, LINEBUFFER - 1, "unique-local.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_NEW_ADDR_TEREDO) != 0 ) {
				if ( (typeinfo & IPV6_NEW_ADDR_6BONE) != 0 ) {
					snprintf(resultstring, LINEBUFFER - 1, "teredo.6bone-global.ipv6addrtype.ipv6calc");
				} else {
					snprintf(resultstring, LINEBUFFER - 1, "teredo.unknown-global.ipv6addrtype.ipv6calc");
				};
			} else if ( (typeinfo & IPV6_NEW_ADDR_6BONE) != 0 ) {
				snprintf(resultstring, LINEBUFFER - 1, "6bone-global.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_NEW_ADDR_6TO4) != 0 ) {
				snprintf(resultstring, LINEBUFFER - 1, "6to4-global.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_NEW_ADDR_PRODUCTIVE) != 0 ) {
				snprintf(resultstring, LINEBUFFER - 1, "productive-global.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_ADDR_MAPPED) != 0 ) {
				snprintf(resultstring, LINEBUFFER - 1, "mapped-ipv4.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_ADDR_COMPATv4) != 0 ) {
				snprintf(resultstring, LINEBUFFER - 1, "compat-ipv4.ipv6addrtype.ipv6calc");
			} else {
				snprintf(resultstring, LINEBUFFER - 1, "unknown-ipv6.ipv6addrtype.ipv6calc");
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
		if (ipv6calc_debug & 0x4) {
			fprintf(stderr, "LRU cache: fill line=%d key_token=%s key_outputtype=%lx value=%s\n", cache_lru_last - 1, cache_lru_key_token[cache_lru_last - 1], cache_lru_key_outputtype[cache_lru_last - 1], cache_lru_value[cache_lru_last - 1]);
		};
	};

	return (0);
};
#undef DEBUG_function_name

