/*
 * Project    : ipv6calc
 * File       : ipv6loganon.c
 * Version    : $Id: ipv6loganon.c,v 1.1 2007/01/30 17:00:37 peter Exp $
 * Copyright  : 2007 by Peter Bieringer <pb (at) bieringer.de>
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
int flag_quiet = 0;
int flag_nocache = 0;

/* default values */
int mask_ipv4 = 24;
int mask_ipv6_net = 16;
int mask_eui_id = 1;


/* prototypes */
static int anonymizetoken(char *result, const char *token, const int flag_skipunknown);
static void lineparser(const long int outputtype);


/* LRU cache */

#define CACHE_LRU_SIZE 200

static int      cache_lru_max = 0;
static int      cache_lru_last = 0;
int      cache_lru_limit = 20; /* optimum */
static char     cache_lru_key_token[CACHE_LRU_SIZE][NI_MAXHOST];
static char     cache_lru_value[CACHE_LRU_SIZE][NI_MAXHOST];
static long int cache_lru_statistics[CACHE_LRU_SIZE];


/**************************************************/
/* main */
#define DEBUG_function_name "ipv6loganon/main"
int main(int argc,char *argv[]) {
	int i, lop;
	unsigned long int command = 0;


	/* new option style storage */	
	uint32_t inputtype  = FORMAT_undefined, outputtype = FORMAT_undefined;
	
	/* anonert storage */
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
		ipv6loganon_printinfo();
		exit(EXIT_FAILURE);
	};

	/* Fetch the command-line arguments. */
	while ((i = getopt_long(argc, argv, ipv6loganon_shortopts, ipv6loganon_longopts, &lop)) != EOF) {
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

			case CMD_LA_PRESET_STANDARD:
				mask_ipv4 = 24;
				mask_ipv6_net = 16;
				mask_eui_id = 1;
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
		ipv6loganon_printhelp();
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
#define DEBUG_function_name "ipv6loganon/lineparser"
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
		
		/* call anonymizer now */
		retval = anonymizetoken(resultstring, charptr, 1);

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
			retval = anonerttoken(resultstring, token, FORMAT_addrtype, 0);
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
			retval = anonymizetoken(resultstring, token, 0);
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
 * Anonymize IPv4 address
 */
#define DEBUG_function_name "ipv6loganon/anonymizeipv4address"
void anonymizeipv4address(ipv6calc_ipv4addr *ipv4addrp) {
	/* anonymize IPv4 address according to settings */

	if (mask_ipv4 == 0) {
		/* clear IPv4 address: 0.0.0.0 */
		ipv4addr_clear(ipv4addrp);
	} else if (mask_ipv4 == 32) {
		/* nothing to do */
	} else if (mask_ipv4 < 1 || mask_ipv4 > 31) {
		/* should not happen here */
		fprintf(stderr, "%s: Value for 'mask_ipv4' has an unexpected illegal value!\n", DEBUG_function_name);
		exit(EXIT_FAILURE);
	} else {
		/* mask IPv4 address */
		ipv4addr_setdword(ipv4addrp, ipv4addr_getdword(ipv4addrp) & (0xffffffffu << (32 - mask_ipv4)));
	};
	return;
};
#undef DEBUG_function_name

/*
 * Anonymize token
 */
#define DEBUG_function_name "ipv6loganon/anonymizetoken"
static int anonymizetoken(char *resultstring, const char *token, const int flag_skipunknown) {
	long int inputtype = -1;
	int retval = 1, i;
	uint32_t typeinfo;

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

		if (strcmp(cache_lru_key_token[cache_lru_last - 1], token) == 0) {
			snprintf(resultstring, LINEBUFFER - 1, cache_lru_value[cache_lru_last - 1]);
			cache_lru_statistics[0]++;
			if (ipv6calc_debug & 0x4) {
				fprintf(stderr, "LRU cache: hit last line=%d key_token=%s value=%s\n", cache_lru_last - 1, token, resultstring);
			};
			return (0);
		};
		/* run backwards to first entry */
		if (cache_lru_last > 1) {
			for (i = cache_lru_last - 1; i > 0; i--) {
				if (strcmp(cache_lru_key_token[i - 1], token) == 0) {
					snprintf(resultstring, LINEBUFFER - 1, cache_lru_value[i - 1]);
					cache_lru_statistics[cache_lru_last - i]++;
					if (ipv6calc_debug & 0x4) {
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
					snprintf(resultstring, LINEBUFFER - 1, cache_lru_value[i - 1]);
					cache_lru_statistics[cache_lru_max - i + cache_lru_last]++;
					if (ipv6calc_debug & 0x4) {
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
		/* anonymize IPv4 address according to settings */

		/* check for type */
		typeinfo = ipv6addr_gettype(&ipv6addr);

		if ( (typeinfo & IPV6_NEW_ADDR_6TO4) != 0 ) {
			/* extract IPv4 address */
			for (i = 0; i <= 3; i++) {
				ipv4addr_setoctett(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctett(&ipv6addr, (unsigned int) 2 + i));
			};

			anonymizeipv4address(&ipv4addr);

			/* store back */
			for (i = 0; i <= 3; i++) {
				ipv6addr_setoctett(&ipv6addr, (unsigned int) 2 + i, (unsigned int) ipv4addr_getoctett(&ipv4addr, (unsigned int) i));
			};
		 };

		if ( (typeinfo & IPV6_NEW_ADDR_TEREDO) != 0 ) {
			/* extract Teredo client IPv4 address */
			for (i = 0; i <= 3; i++) {
				ipv4addr_setoctett(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctett(&ipv6addr, (unsigned int) 12 + i) ^ 0xff);
			};

			anonymizeipv4address(&ipv4addr);

			/* store back */
			for (i = 0; i <= 3; i++) {
				ipv6addr_setoctett(&ipv6addr, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctett(&ipv4addr, (unsigned int) i) ^ 0xff);
			};
		};


		/* Interface identifier included */
		if ( ( ((typeinfo & (IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_ADDR_ULUA )) != 0) || ((typeinfo & (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) == (IPV6_ADDR_LOOPBACK | IPV6_NEW_ADDR_SOLICITED_NODE)) ) && ((typeinfo & (IPV6_NEW_ADDR_TEREDO)) == 0) ) {
			if (ipv6addr_getoctett(&ipv6addr, 11) == 0xff && ipv6addr_getoctett(&ipv6addr, 12) == 0xfe) {
				/* EUI-48 */
				if (mask_eui_id == 1) {
					/* mask unique ID */
					ipv6addr_setoctett(&ipv6addr, 13, 0x0u);
					ipv6addr_setoctett(&ipv6addr, 14, 0x0u);
					ipv6addr_setoctett(&ipv6addr, 15, 0x0u);
				};
			} else {
				/* Check for global EUI-64 */
				if ( (ipv6addr_getoctett(&ipv6addr, 8) & 0x02) != 0 ) {
					if (mask_eui_id == 1) {
						/* mask unique ID */
						ipv6addr_setoctett(&ipv6addr, 11, 0x0u);
						ipv6addr_setoctett(&ipv6addr, 12, 0x0u);
						ipv6addr_setoctett(&ipv6addr, 13, 0x0u);
						ipv6addr_setoctett(&ipv6addr, 14, 0x0u);
						ipv6addr_setoctett(&ipv6addr, 15, 0x0u);
					};
				} else {
					if ( (typeinfo & IPV6_NEW_ADDR_SOLICITED_NODE) != 0 ) {
						if (mask_eui_id == 1) {
							/* mask unique ID */
							ipv6addr_setoctett(&ipv6addr, 13, 0x0u);
							ipv6addr_setoctett(&ipv6addr, 14, 0x0u);
							ipv6addr_setoctett(&ipv6addr, 15, 0x0u);
						};
					} else if ( (typeinfo & IPV6_NEW_ADDR_ISATAP) != 0 )  {
						for (i = 0; i <= 3; i++) {
							ipv4addr_setoctett(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctett(&ipv6addr, (unsigned int) (i + 12)));
						};

						anonymizeipv4address(&ipv4addr);

						/* store back */
						for (i = 0; i <= 3; i++) {
							ipv6addr_setoctett(&ipv6addr, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctett(&ipv4addr, (unsigned int) i));
						};
					} else if ( ( ( (typeinfo & IPV6_ADDR_LINKLOCAL) != 0) && (ipv6addr_getdword(&ipv6addr, 2) == 0 && ipv6addr_getword(&ipv6addr, 6) != 0)) )   {
						/* fe80:: must have 0000:0000:xxxx:yyyy where xxxx > 0 */
						for (i = 0; i <= 3; i++) {
							ipv4addr_setoctett(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctett(&ipv6addr, (unsigned int) (i + 12)));
						};

						anonymizeipv4address(&ipv4addr);

						/* store back */
						for (i = 0; i <= 3; i++) {
							ipv6addr_setoctett(&ipv6addr, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctett(&ipv4addr, (unsigned int) i));
						};
					} else {
						if ((typeinfo & IPV6_NEW_ADDR_6TO4_MICROSOFT) != 0) {
							/* extract IPv4 address */
							for (i = 0; i <= 3; i++) {
								ipv4addr_setoctett(&ipv4addr, (unsigned int) i, (unsigned int) ipv6addr_getoctett(&ipv6addr, (unsigned int) (i + 12)));
							};

							anonymizeipv4address(&ipv4addr);

							/* store back */
							for (i = 0; i <= 3; i++) {
								ipv6addr_setoctett(&ipv6addr, (unsigned int) 12 + i, (unsigned int) ipv4addr_getoctett(&ipv4addr, (unsigned int) i));
							};
						} else {
							/* Identifier has local scope */
						};
					};
				};
			};
		};

		libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, 0);

	} else if (ipv4addr.flag_valid == 1) {
		anonymizeipv4address(&ipv4addr);

		/* convert IPv4 address structure to string */
		libipv4addr_ipv4addrstruct_to_string(&ipv4addr, resultstring, 0);
	} else {
		/* probably reverse DNS resolving lookup string, do not touch */
		snprintf(resultstring, LINEBUFFER - 1, token);
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
		snprintf(cache_lru_key_token[cache_lru_last - 1], NI_MAXHOST - 1, token);
		snprintf(cache_lru_value[cache_lru_last - 1], NI_MAXHOST - 1, resultstring);
		if (ipv6calc_debug & 0x4) {
			fprintf(stderr, "LRU cache: fill line=%d key_token=%s value=%s\n", cache_lru_last - 1, cache_lru_key_token[cache_lru_last - 1], cache_lru_value[cache_lru_last - 1]);
		};
	};

	return (0);
};
#undef DEBUG_function_name

