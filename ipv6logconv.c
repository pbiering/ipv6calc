/*
 * Project    : ipv6calc
 * File       : ipv6logconv.c
 * Version    : $Id: ipv6logconv.c,v 1.2 2002/03/16 00:39:03 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Dedicated program for logfile conversions
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <getopt.h> 

#include "ipv6calc.h"
#include "libipv6calc.h"
#include "ipv6calctypes.h"
#include "ipv6calcoptions.h"
#include "ipv6calchelp.h"

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
	char resultstring[LINEBUFFER] = "";
	char resultstring2[NI_MAXHOST] = "";
	char resultstring3[NI_MAXHOST] = "";
	int retval = 1, i, j, lop;
	unsigned long int command = 0;
	int bit_start = 0, bit_end = 0;

	/* new option style storage */	
	long int inputtype = -1, outputtype = -1;
	
	/* convert storage */
	long int action = -1;

	/* format options storage */
	int formatoptions = 0;

	if (argc <= 1) {
		printinfo();
		exit(EXIT_FAILURE);
	};

	/* Fetch the command-line arguments. */
	while ((i = getopt_long(argc, argv, ipv6calc_shortopts, ipv6calc_longopts, &lop)) != EOF) {
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

			/* format options */
			case FORMATOPTION_printcompressed + FORMATOPTION_HEAD:
	       			formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case FORMATOPTION_printuncompressed + FORMATOPTION_HEAD:
	       			formatoptions |= FORMATOPTION_printuncompressed;
				break;
				
			case FORMATOPTION_printfulluncompressed + FORMATOPTION_HEAD:
	       			formatoptions |= FORMATOPTION_printfulluncompressed;
				break;
				
			case FORMATOPTION_printprefix + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_printprefix;
				break;
				
			case FORMATOPTION_printsuffix + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_printsuffix;
				break;
				
			case FORMATOPTION_maskprefix + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_maskprefix;
				break;
				
			case FORMATOPTION_masksuffix + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_masksuffix;
				break;
				
			case 'l':	
			case FORMATOPTION_printlowercase + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_printlowercase;
				break;
				
			case 'u':	
			case FORMATOPTION_printuppercase + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_printuppercase;
				break;
				
			case FORMATOPTION_printstart + FORMATOPTION_HEAD:
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_start = atoi(optarg);
					formatoptions |= FORMATOPTION_printstart;
				} else {
					fprintf(stderr, " Argument of option 'printstart' is out or range (1-128): %d\n", atoi(optarg));
					exit(EXIT_FAILURE);
				};
				break;
				
			case FORMATOPTION_printend + FORMATOPTION_HEAD:
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_end = atoi(optarg);
					formatoptions |= FORMATOPTION_printend;
				} else {
					fprintf(stderr, " Argument of option 'printend' is out or range (1-128): %d\n", atoi(optarg));
					exit(EXIT_FAILURE);
				};
				break;
				
			/* new options */
			case CMD_inputtype:
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got input string: %s\n", DEBUG_function_name, optarg);
				};

				if (strcmp(optarg, "-?") == 0) {
					inputtype = -2;
					command = CMD_printhelp;
					break;
				};
				
				inputtype = ipv6calctypes_checktype(optarg);
				
				if (inputtype < 0) {
					fprintf(stderr, " Input option is unknown: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
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
			printhelp_outputtypes(inputtype);
			exit(EXIT_FAILURE);
		} else if (inputtype == -2) {
			printhelp_inputtypes();
			exit(EXIT_FAILURE);
		} else if (action == -2) {
			printhelp_actiontypes();
			exit(EXIT_FAILURE);
		};
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Debug value:%lx  command:%lx  inputtype:%lx   outputtype:%lx  action:%lx  formatoptions:%x\n", (unsigned long) ipv6calc_debug, command, (unsigned long) inputtype, (unsigned long) outputtype, (unsigned long) action, formatoptions); 
	};
	
	/* do work depending on selection */
	if (command == CMD_printversion) {
		printversion();
		exit(EXIT_FAILURE);
	};

	if (command == CMD_printhelp) {
		printhelp();
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
	
	fprintf(stderr, "Waiting for log lines on stdin\n");

	while (1 == 1) {
		/* read line from stdin */
		charptr = fgets(linebuffer, LINEBUFFER, stdin);
		
		if (charptr == NULL) {
			/* end of input */
			break;
		};

		linecounter++;

		if (strlen(linebuffer) >= LINEBUFFER) {
			fprintf(stderr, "Line too long: %d\n", linecounter);
			continue;
		};
		
		if (strlen(linebuffer) == 0) {
			fprintf(stderr, "Line empty: %d\n", linecounter);
			continue;
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

	return (0);
};
#undef DEBUG_function_name


/*
 * Convert token
 */
#define DEBUG_function_name "ipv6logconv/converttoken"
static int converttoken(char *resultstring, const char *token, const long int outputtype) {
	long int inputtype = -1;
	int retval = 1, i, j, lop;
	unsigned long int command = 0;
	int bit_start = 0, bit_end = 0;
	int typeinfo;

	char resultstring2[NI_MAXHOST] = "";
	char resultstring3[NI_MAXHOST] = "";

	/* format options storage */
	int formatoptions = 0;


	/* used structures */
	ipv6calc_ipv6addr ipv6addr, ipv6addr2, ipv6addr3, ipv6addr4;
	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_macaddr  macaddr;


	if (strlen(token) == 0) {
		return (1);
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Token: '%s'\n", token);
	};

	/* autodetection */
	inputtype = libipv6calc_autodetectinput(token);

	if (inputtype >= 0) {
		for (i = 0; i < (int) (sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0])); i++) {
			if (inputtype == ipv6calc_formatstrings[i].number) {
				fprintf(stderr, "found type: %s\n", ipv6calc_formatstrings[i].token);
				break;
			};
		};
	} else {
		fprintf(stderr, "no result!\n");
	};

	/* proceed input depending on type */	
	switch (inputtype) {
		case FORMAT_ipv6addr:
			retval = addr_to_ipv6addrstruct(token, resultstring, &ipv6addr);
			break;

		case FORMAT_ipv4addr:
			retval = addr_to_ipv4addrstruct(token, resultstring, &ipv4addr);
			break;

		default:
			fprintf(stderr, " Input-type isn't implemented\n");
			return (1);
	};

	/***** postprocessing input *****/
	
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Start of postprocessing input\n", DEBUG_function_name);
	};

	if (ipv6addr.flag_valid == 1) {
		/* mask bits */
		if ((formatoptions & (FORMATOPTION_maskprefix | FORMATOPTION_masksuffix)) != 0) {
			if (ipv6addr.flag_prefixuse == 1) {
				if ((formatoptions & FORMATOPTION_maskprefix) != 0) {
					ipv6addrstruct_maskprefix(&ipv6addr);
				} else if ((formatoptions & FORMATOPTION_masksuffix) != 0) {
					ipv6addrstruct_masksuffix(&ipv6addr);
				};
			} else {
				fprintf(stderr, " Error: mask option used without specifying a prefix length\n");
				exit(EXIT_FAILURE);
			};
		};
		
		/* start bit */
		if ((formatoptions & FORMATOPTION_printstart) != 0) {
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Set bit start to: %d\n", DEBUG_function_name, bit_start);
			};
			ipv6addr.bit_start = (unsigned short) bit_start;
			ipv6addr.flag_startend_use = 1;
		} else {
			ipv6addr.bit_start = 1;
		};
		
		/* end bit */
		if ((formatoptions & FORMATOPTION_printend) != 0) {
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Set bit end to: %d\n", DEBUG_function_name, bit_end);
			};
			ipv6addr.bit_end = (unsigned short) bit_end;
			ipv6addr.flag_startend_use = 1;
		} else {
			/* default */
			ipv6addr.bit_end = 128;
		};
		
		/* prefix+suffix */
		if ((formatoptions & (FORMATOPTION_printprefix | FORMATOPTION_printsuffix)) != 0) {
			if ( ipv6addr.flag_prefixuse == 0 ) {
				fprintf(stderr, " Error: missing prefix length for printing prefix/suffix\n");
				exit(EXIT_FAILURE);
			} else {
				if ( ipv6addr.flag_startend_use == 0 ) {
					/* only print[prefix|suffix] */
					if ((formatoptions & FORMATOPTION_printprefix) != 0) {
						ipv6addr.bit_start = 1;
						ipv6addr.bit_end = ipv6addr.prefixlength;
						ipv6addr.flag_startend_use = 1;
					} else if ((formatoptions & FORMATOPTION_printsuffix) != 0) {
						ipv6addr.bit_start = ipv6addr.prefixlength + 1;
						ipv6addr.bit_end = 128;
						ipv6addr.flag_startend_use = 1;
					};
				} else {
					/* mixed */
					if ((formatoptions & FORMATOPTION_printprefix) != 0) {
						if ( ipv6addr.prefixlength < ipv6addr.bit_start ) {
							fprintf(stderr, " Error: prefix length '%d' lower than given start bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_start);
							exit(EXIT_FAILURE);
						} else if ( ipv6addr.prefixlength >= ipv6addr.bit_end ) {
							fprintf(stderr, " Error: prefix length '%d' higher than given end bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_end);
							exit(EXIT_FAILURE);
						} else {
							ipv6addr.bit_end = ipv6addr.prefixlength;
						};
					} else if ((formatoptions & FORMATOPTION_printsuffix) != 0) {
						if ( ipv6addr.prefixlength >= ipv6addr.bit_end ) {
							fprintf(stderr, " Error: prefix length '%d' higher than or eqal to given end bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_end);
							exit(EXIT_FAILURE);
						} else if (ipv6addr.prefixlength >= ipv6addr.bit_start) {
							fprintf(stderr, " Error: prefix length '%d' higher than or equal to given start bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_start);
							exit(EXIT_FAILURE);
						} else {
							ipv6addr.bit_start = ipv6addr.prefixlength + 1;
						};
					};
				};
			};
		};

		/* check start/end */
		if ( ipv6addr.flag_startend_use == 1 ) {
			if ( ipv6addr.bit_start > ipv6addr.bit_end ) {
				fprintf(stderr, " Error: start bit bigger than end bit\n");
				exit(EXIT_FAILURE);
			} else if ( ipv6addr.bit_start == ipv6addr.bit_end ) {
				fprintf(stderr, " Error: start bit equal to end bit\n");
				exit(EXIT_FAILURE);
			};
		};
	};

	switch (outputtype) {
		case FORMAT_addrtype:
			if (ipv6addr.flag_valid == 1) {
				sprintf(resultstring, "IPv6");
			} else if (ipv4addr.flag_valid == 1) {
				sprintf(resultstring, "IPv4");
			};
			break;

		case FORMAT_ipv6addr:
			if (ipv6addr.flag_valid != 1) { return(1); };

			if ((formatoptions & FORMATOPTION_printuncompressed) != 0) {
				retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, formatoptions);
			} else if ((formatoptions & FORMATOPTION_printfulluncompressed) != 0) {
				retval = libipv6addr_ipv6addrstruct_to_fulluncompaddr(&ipv6addr, resultstring, formatoptions);
			} else {
				retval = librfc1884_ipv6addrstruct_to_compaddr(&ipv6addr, resultstring, formatoptions);
			};
			break;
			
		case FORMAT_ipv4addr:
			if (ipv4addr.flag_valid != 1) { return(1); };

			retval = libipv4addr_ipv4addrstruct_to_string(&ipv4addr, resultstring, formatoptions);
			break;
			
		case FORMAT_oui:
			if (ipv6addr.flag_valid != 1) { return(1); };

			/* check whether address has a OUI ID */
			if ( ipv6addr_gettype(&ipv6addr) & (IPV6_ADDR_LINKLOCAL | IPV6_ADDR_SITELOCAL | IPV6_NEW_ADDR_AGU | IPV6_NEW_ADDR_6BONE | IPV6_NEW_ADDR_6TO4) == 0 )  { return (1); };


			retval = libieee_get_vendor_string(resultstring, ipv6addr_getoctett(&ipv6addr, 8) ^0x02, ipv6addr_getoctett(&ipv6addr, 9), ipv6addr_getoctett(&ipv6addr, 10) );
			break;
			
		case FORMAT_ipv6addrtype:
			if (ipv6addr.flag_valid != 1) { return(1); };

			typeinfo = ipv6addr_gettype(&ipv6addr);
		       	if ( typeinfo & IPV6_ADDR_LINKLOCAL != 0 ) {
				sprintf(resultstring, "link-local");
			} else if ( typeinfo & IPV6_ADDR_SITELOCAL != 0 ) {
				sprintf(resultstring, "site-local");
			} else if ( typeinfo & IPV6_NEW_ADDR_6BONE != 0 ) {
				sprintf(resultstring, "6bone-global");
			} else if ( typeinfo & IPV6_NEW_ADDR_6TO4 != 0 ) {
				sprintf(resultstring, "6to4-global");
			} else if ( typeinfo & IPV6_NEW_ADDR_PRODUCTIVE != 0 ) {
				sprintf(resultstring, "productive-global");
			};
			break;

		default:
			fprintf(stderr, " Outputtype isn't implemented\n");
			return (1);
	};

	return (0);
};
#undef DEBUG_function_name

