/*
 * Project    : ipv6calc
 * File       : ipv6logconv.c
 * Version    : $Id: ipv6logconv.c,v 1.7 2002/03/16 19:25:15 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Dedicated program for logfile conversions
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <getopt.h> 
#include <unistd.h>

#include "ipv6calc.h"
#include "libipv6calc.h"
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

/* supported output types:
 *  ipv6addr
 *  ipv4addr
 *  addrtype : IPv4 | IPv6
 */

long int ipv6calc_debug = 0;

/* prototypes */
static int converttoken(char *result, const char *token, const long int outputtype);
static int lineparser(const long int outputtype);

/**************************************************/
/* main */
#define DEBUG_function_name "ipv6logconv/main"
int main(int argc,char *argv[]) {
	int retval = 1, i, lop;
	unsigned long int command = 0;

	/* new option style storage */	
	long int inputtype = -1, outputtype = -1;
	
	/* convert storage */
	long int action = -1;

	/* check for UID */
	if (getuid() == 0) {
		ipv6logconv_printversion();
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
			case 'h':
			case '?':
				command |= CMD_printhelp;
				break;
				
			case 'd':
				ipv6calc_debug = atol(optarg);
				break;

			case CMD_outputtype:
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got output string: %s\n", DEBUG_function_name, optarg);
				};
				if (strcmp(optarg, "-?") == 0) {
					outputtype = -2;
					command = CMD_printhelp;
					break;
				};
				
				outputtype = ipv6calctypes_checktype(optarg);
				if (outputtype < 0) {
					fprintf(stderr, " Output option is unknown: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				break;

			case CMD_printexamples:
				command = CMD_printexamples;
				break;

			default:
				fprintf(stderr, "Usage: (see '%s --command -?' for more help)\n", PROGRAM_NAME);
				printhelp();
				break;
		};
	};
	argv += optind;
	argc -= optind;

	/* print help handling */
	if (command == CMD_printhelp) {
		if (outputtype == -2) {
			printhelp_outputtypes(FORMAT_ipv6logconv);
			exit(EXIT_FAILURE);
		};
        } else if (command == CMD_printexamples) {
		printhelp_output_dispatcher(outputtype);
		exit(EXIT_FAILURE);
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Debug value:%lx  command:%lx  inputtype:%lx   outputtype:%lx  action:%lx\n", (unsigned long) ipv6calc_debug, command, (unsigned long) inputtype, (unsigned long) outputtype, (unsigned long) action); 
	};
	
	/* do work depending on selection */
	if (command == CMD_printversion) {
		ipv6logconv_printversion();
		exit(EXIT_FAILURE);
	};

	if (command == CMD_printhelp) {
		ipv6logconv_printhelp();
		exit(EXIT_FAILURE);
	};
	

	/* call lineparser */
	lineparser(outputtype);

	exit(retval);
};
#undef DEBUG_function_name


/*
 * Line parser
 */
#define DEBUG_function_name "ipv6logconv/lineparser"
static int lineparser(const long int outputtype) {
	char linebuffer[LINEBUFFER];
	char token[LINEBUFFER];
	char resultstring[LINEBUFFER];
	char *charptr;
	int linecounter = 0, retval;
	size_t numsep;
	
	fprintf(stderr, "Expecting log lines on stdin\n");

	while (1 == 1) {
		/* read line from stdin */
		charptr = fgets(linebuffer, LINEBUFFER, stdin);
		
		if (charptr == NULL) {
			/* end of input */
			break;
		};

		linecounter++;

		if (linecounter == 1) {
			fprintf(stderr, "Ok, proceeding stdin...\n");
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

		/* look first token */
		numsep = strcspn(linebuffer, " \t\n");
		if (numsep >=  LINEBUFFER) {
			fprintf(stderr, "Line too strange: %d\n", linecounter);
			continue;
		};
		
		if (numsep == 0) {
			fprintf(stderr, "Line contains no token: %d\n", linecounter);
			continue;
		};
		
		/* copy first token into new buffer) */
		strncpy(token, linebuffer, numsep);
		token[numsep] = '\0';

		/* call converter now */
		retval = converttoken(resultstring, token, outputtype);

		if (retval == 0) {
			/* print result */
			printf("%s", resultstring);

			/* print rest of line, if available */
			if (numsep < strlen(linebuffer)) {
				printf("%s", linebuffer+numsep);
			};
		};
	};

	fprintf(stderr, "...finished\n");
	return (0);
};
#undef DEBUG_function_name


/*
 * Convert token
 */
#define DEBUG_function_name "ipv6logconv/converttoken"
static int converttoken(char *resultstring, const char *token, const long int outputtype) {
	long int inputtype = -1;
	int retval = 1, i;
	int typeinfo;
	char tempstring[NI_MAXHOST];

	/* used structures */
	ipv6calc_ipv6addr ipv6addr;
	ipv6calc_ipv4addr ipv4addr;

       	/* clear resultstring */
	resultstring[0] = '\0';


	if (strlen(token) == 0) {
		return (1);
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Token: '%s'\n", DEBUG_function_name, token);
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

	/***** postprocessing input *****/
	
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Start of postprocessing input\n", DEBUG_function_name);
	};

	switch (outputtype) {
		case FORMAT_addrtype:
			if (ipv6addr.flag_valid == 1) {
				sprintf(resultstring, "ipv6-addr.addrtype.ipv6calc");
			} else if (ipv4addr.flag_valid == 1) {
				sprintf(resultstring, "ipv4-addr.addrtype.ipv6calc");
			} else {
				sprintf(resultstring, "reverse-lookup-successful.addrtype.ipv6calc");
			};
			break;

		case FORMAT_ouitype:
			if (ipv6addr.flag_valid != 1) { return(1); };

			/* check whether address has a OUI ID */
			if ( ( ipv6addr_gettype(&ipv6addr) & (IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_NEW_ADDR_6BONE | IPV6_NEW_ADDR_6TO4)) == 0 )  { return (1); };

			if ((ipv6addr_getoctett(&ipv6addr, 8) & 0x02) != 0) {
				retval = libieee_get_short_vendor_string(resultstring, ipv6addr_getoctett(&ipv6addr, 8) ^0x02, ipv6addr_getoctett(&ipv6addr, 9), ipv6addr_getoctett(&ipv6addr, 10) );
				if (retval != 0) {
					return (1);
				};
				if (strlen(resultstring) == 0) {
					sprintf(resultstring, "unknown.ouitype.ipv6calc");
				} else {
					sprintf(tempstring, "%s.ouitype.ipv6calc", resultstring);
					sprintf(resultstring, "%s", tempstring);
				};
			} else {
				sprintf(resultstring, "local-scope.ouitype.ipv6calc");
			};
			break;
			
		case FORMAT_ipv6addrtype:
			if (ipv6addr.flag_valid != 1) { return(1); };

			typeinfo = ipv6addr_gettype(&ipv6addr);

		       	if ( (typeinfo & IPV6_ADDR_LINKLOCAL) != 0 ) {
				sprintf(resultstring, "link-local.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_ADDR_SITELOCAL) != 0 ) {
				sprintf(resultstring, "site-local.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_NEW_ADDR_6BONE) != 0 ) {
				sprintf(resultstring, "6bone-global.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_NEW_ADDR_6TO4) != 0 ) {
				sprintf(resultstring, "6to4-global.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_NEW_ADDR_PRODUCTIVE) != 0 ) {
				sprintf(resultstring, "productive-global.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_ADDR_MAPPED) ) {
				sprintf(resultstring, "mapped-ipv4.ipv6addrtype.ipv6calc");
			} else if ( (typeinfo & IPV6_ADDR_COMPATv4) ) {
				sprintf(resultstring, "compat-ipv4.ipv6addrtype.ipv6calc");
			} else {
				sprintf(resultstring, "unknown-ipv6.ipv6addrtype.ipv6calc");
			};
			break;

		default:
			fprintf(stderr, " Outputtype isn't implemented\n");
			return (1);
	};

	return (0);
};
#undef DEBUG_function_name

