/*
 * Project    : ipv6calc
 * File       : ipv6calc.c
 * Version    : $Id: ipv6calc.c,v 1.11 2002/03/02 17:27:27 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Central program (main)
 *  This program print out different formats of an given IPv6 address
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <getopt.h> 
#include <unistd.h> 
#include "ipv6calc.h"
#include "libipv6calc.h"
#include "ipv6calctypes.h"

#include "addr_to_ip6int.h"
#include "addr_to_compressed.h"
#include "addr_to_uncompressed.h"
#include "addr_to_fulluncompressed.h"
#include "addr_to_ifinet6.h"
#include "addr_to_base85.h"
#include "eui64_to_privacy.h"
#include "mac_to_eui64.h"
#include "ipv4_to_6to4addr.h"
#include "showinfo.h"

#include "librfc1884.h"

#include "librfc1924.h"
#include "base85_to_addr.h" /* going obsolete */

#include "libifinet6.h"
#include "ifinet6_to_compressed.h" /* going obsolete */

#include "librfc2874.h"
#include "addr_to_bitstring.h" /* going obsolete */

long int ipv6calc_debug = 0;


/**************************************************/
/* main */
#define DEBUG_function_name "ipv6calc/main"
int main(int argc,char *argv[]) {
	char resultstring[NI_MAXHOST] = "";
	char tempstring[NI_MAXHOST] = "";
	int retval = 1, i, lop;
	unsigned long int command = 0;
	unsigned short bit_start = 0, bit_end = 0;
	ipv6calc_ipv6addr ipv6addr;
	long int inputtype = -1, outputtype = -1;
	unsigned int formatoptions = 0;

	/* define short options */
	char *shortopts = "vh?rmabd:iul";

	struct option longopts[] = {
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{"debug", 1, 0, 'd'},
		
		/* command options */
		{"addr2ip6_int", 0, 0, CMD_addr_to_ip6int },
		{"addr_to_ip6int", 0, 0, CMD_addr_to_ip6int },

		{"addr2ip6_arpa", 0, 0, CMD_addr_to_ip6arpa },
		{"addr_to_ip6arpa", 0, 0, CMD_addr_to_ip6arpa },
		
		{"addr_to_bitstring", 0, 0, CMD_addr_to_bitstring },

		{"addr2compaddr", 0, 0, CMD_addr_to_compressed },
		{"addr_to_compressed", 0, 0, CMD_addr_to_compressed },
		{"addr2uncompaddr", 0, 0, CMD_addr_to_uncompressed },
		{"addr_to_uncompressed", 0, 0, CMD_addr_to_uncompressed },
		
		{"addr_to_base85", 0, 0, CMD_addr_to_base85 },
		{"base85_to_addr", 0, 0, CMD_base85_to_addr },

		{"mac_to_eui64", 0, 0, CMD_mac_to_eui64 },
		
		{ "addr2fulluncompaddr", 0, 0, CMD_addr_to_fulluncompressed },
		{ "addr_to_fulluncompressed", 0, 0, CMD_addr_to_fulluncompressed },
		{ "addr2if_inet6", 0, 0, CMD_addr_to_ifinet6 },
		{ "addr_to_ifinet6", 0, 0, CMD_addr_to_ifinet6 },
		{ "if_inet62addr", 0, 0, CMD_ifinet6_to_compressed },
		{ "ifinet6_to_compressed", 0, 0, CMD_ifinet6_to_compressed },

		{ "eui64_to_privacy", 0, 0, CMD_eui64_to_privacy },
		
		{ "ipv4_to_6to4addr", 0, 0, CMD_ipv4_to_6to4addr },
		
		{ "showinfo", 0, 0, CMD_showinfo },
		{ "machine_readable", 0, 0, CMD_machinereadable },
		{ "show_types", 0, 0, CMD_showtypes },

		/* format options */
		{ "maskprefix"           , 0, 0, FORMATOPTION_maskprefix + FORMATOPTION_HEAD },
		{ "masksuffix"           , 0, 0, FORMATOPTION_masksuffix + FORMATOPTION_HEAD },
		
		{ "uppercase"            , 0, 0, FORMATOPTION_printuppercase + FORMATOPTION_HEAD },
		{ "lowercase"            , 0, 0, FORMATOPTION_printlowercase + FORMATOPTION_HEAD },
		
		{ "printstart"           , 1, 0, FORMATOPTION_printstart + FORMATOPTION_HEAD },
		{ "printend"             , 1, 0, FORMATOPTION_printend + FORMATOPTION_HEAD },
		
		{ "printprefix"          , 0, 0, FORMATOPTION_printprefix + FORMATOPTION_HEAD },
		{ "printsuffix"          , 0, 0, FORMATOPTION_printsuffix + FORMATOPTION_HEAD },
		
		{ "printcompressed"      , 0, 0, FORMATOPTION_printcompressed       + FORMATOPTION_HEAD },
		{ "printuncompressed"    , 0, 0, FORMATOPTION_printuncompressed     + FORMATOPTION_HEAD },
		{ "printfulluncompressed", 0, 0, FORMATOPTION_printfulluncompressed + FORMATOPTION_HEAD },

		/* type options */
		{ "inputtype" , 1, 0, CMD_inputtype  },
		{ "intype"    , 1, 0, CMD_inputtype  },
		{ "outputtype", 1, 0, CMD_outputtype },
		{ "outtype"   , 1, 0, CMD_outputtype },

		{NULL, 0, 0, 0}
	};                

	if (argc <= 1) {
		printinfo();
		exit(1);
	};

	/* Fetch the command-line arguments. */
	while ((i = getopt_long(argc, argv, shortopts, longopts, &lop)) != EOF) {
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

			/* backward compatibility commands */
			case 'r':
			case CMD_addr_to_ip6int:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_revnibbles_int;
				break;

			case 'a':
			case CMD_addr_to_ip6arpa:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_revnibbles_arpa;
				break;

			case 'b':
			case CMD_addr_to_bitstring:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_bitstring;
				break;
				
			case CMD_addr_to_compressed:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case CMD_addr_to_uncompressed:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printuncompressed;
				break;
				
			case CMD_addr_to_base85:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_base85;
				break;
				
			case CMD_base85_to_addr:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_base85;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printuncompressed;
				break;

			case CMD_mac_to_eui64:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_mac;
				outputtype = FORMAT_eui64;
				break;
				
			case CMD_addr_to_fulluncompressed:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printfulluncompressed;
				break;
				
			case CMD_addr_to_ifinet6:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ifinet6;
				break;

			case CMD_ifinet6_to_compressed:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(1); };
				inputtype = FORMAT_ifinet6;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printcompressed;
				break;


			/* still special handled commands */
			case CMD_eui64_to_privacy:
				command |= CMD_eui64_to_privacy;
				break;

			case CMD_ipv4_to_6to4addr:
				command |= CMD_ipv4_to_6to4addr;
				break;

			case 'i':
			case CMD_showinfo:
				command |= CMD_showinfo;
				break;

			/* format options - old style */
			case 'm':	
			case CMD_machinereadable:
				command |= CMD_machinereadable;
				break;

			case CMD_showtypes:
				command |= CMD_showtypes;
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
					exit (1);
				};
				break;
				
			case FORMATOPTION_printend + FORMATOPTION_HEAD:
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_end = atoi(optarg);
					formatoptions |= FORMATOPTION_printend;
				} else {
					fprintf(stderr, " Argument of option 'printend' is out or range (1-128): %d\n", atoi(optarg));
					exit (1);
				};
				break;


			/* type options */
			case CMD_inputtype:
				if (ipv6calc_debug) {
					fprintf(stderr, "%s: Got input type: %s\n", DEBUG_function_name, optarg);
				};

				if (strcmp(optarg, "-?") == 0) {
					command |= CMD_printhelp;
					inputtype = -2;
					break;
				};
				
				inputtype = ipv6calctypes_checktype(optarg);
				
				if (inputtype < 0) {
					fprintf(stderr, " Input type is unknown: %s\n", optarg);
					exit (1);
				};
				break;	
				
			case CMD_outputtype:
				if (ipv6calc_debug) {
					fprintf(stderr, "%s: Got output type: %s\n", DEBUG_function_name, optarg);
				};
				if (strcmp(optarg, "-?") == 0) {
					outputtype = -2;
					command |= CMD_printhelp;
					break;
				};
				
				outputtype = ipv6calctypes_checktype(optarg);
				if (outputtype < 0) {
					fprintf(stderr, " Output type is unknown: %s\n", optarg);
					exit (1);
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
	if (command & CMD_printhelp) {
		if (outputtype == -2) {
			printhelp_outputtypes(inputtype);
			exit (1);
		} else if (inputtype == -2) {
			printhelp_inputtypes();
			exit (1);
		};
	};

/*	fprintf(stderr, "Command value: %lx\n", command); */
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Debug value: %lx  command: %lx\n  inputtype: %lx   outputtype: %lx", ipv6calc_debug, command, inputtype, outputtype); 
	};
	
	/* do work depending on selection */
	if (command & CMD_printversion) {
		printversion();
		exit(1);
	};

	if (command & CMD_printhelp) {
		printhelp();
		exit (1);
	};
	
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: Got input %s\n", DEBUG_function_name, argv[0]);
	};

	/**************** input type handling ************/
	switch (inputtype) {
		case -1:
			/* old implementation */
			goto OUTPUT_type;
			break;

		case FORMAT_ipv6addr:
			if (argc < 1) { printhelp_missinginputdata(); exit(1); };
			retval = addr_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
			argc--;
			break;

		case FORMAT_base85:
			if (argc < 1) { printhelp_missinginputdata(); exit(1); };
			retval = base85_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
			argc--;
			break;

		case FORMAT_ifinet6:
			if (argc < 1) { printhelp_missinginputdata(); exit(1); };
			if (argc < 2) {
				retval = libifinet6_ifinet6_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
				argc--;
			} else {
				retval = libifinet6_ifinet6_withprefixlength_to_ipv6addrstruct(argv[0], argv[1], resultstring, &ipv6addr);
				argc -= 2;
			};
			break;
			
		default:
			fprintf(stderr, " Inputtype isn't implemented\n");
			exit (1);
			break;
	};


	/* postprocessing input */

	if (ipv6addr.flag_valid == 1) {
		/* mask bits */
		if (formatoptions & (FORMATOPTION_maskprefix | FORMATOPTION_masksuffix)) {
			if (ipv6addr.flag_prefixuse == 1) {
				if (formatoptions & FORMATOPTION_maskprefix) {
					ipv6addrstruct_maskprefix(&ipv6addr);
				} else if (formatoptions & FORMATOPTION_masksuffix) {
					ipv6addrstruct_masksuffix(&ipv6addr);
				};
			} else {
				fprintf(stderr, " Error: mask option used without specifying a prefix length\n");
				exit (1);
			};
		};
		
		/* start bit */
		if (formatoptions & FORMATOPTION_printstart) {
			if (ipv6calc_debug) {
				fprintf(stderr, "%s: Set bit start to: %d\n", DEBUG_function_name, bit_start);
			};
			ipv6addr.bit_start = bit_start;
			ipv6addr.flag_startend_use = 1;
		} else {
			ipv6addr.bit_start = 1;
		};
		
		/* end bit */
		if (formatoptions & FORMATOPTION_printend ) {
			if (ipv6calc_debug) {
				fprintf(stderr, "%s: Set bit end to: %d\n", DEBUG_function_name, bit_end);
			};
			ipv6addr.bit_end = bit_end;
			ipv6addr.flag_startend_use = 1;
		} else {
			/* default */
			ipv6addr.bit_end = 128;
		};
		
		/* prefix+suffix */
		if (formatoptions & (FORMATOPTION_printprefix | FORMATOPTION_printsuffix) ) {
			if ( ipv6addr.flag_prefixuse == 0 ) {
				fprintf(stderr, " Error: missing prefix length for printing prefix/suffix\n");
				exit (1);
			} else {
				if ( ipv6addr.flag_startend_use == 0 ) {
					/* only print[prefix|suffix] */
					if (formatoptions & FORMATOPTION_printprefix ) {
						ipv6addr.bit_start = 1;
						ipv6addr.bit_end = ipv6addr.prefixlength;
						ipv6addr.flag_startend_use = 1;
					} else if (formatoptions & FORMATOPTION_printsuffix ) {
						ipv6addr.bit_start = ipv6addr.prefixlength + 1;
						ipv6addr.bit_end = 128;
						ipv6addr.flag_startend_use = 1;
					};
				} else {
					/* mixed */
					if (formatoptions & FORMATOPTION_printprefix) {
						if ( ipv6addr.prefixlength < ipv6addr.bit_start ) {
							fprintf(stderr, " Error: prefix length '%d' lower than given start bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_start);
							exit (1);
						} else if ( ipv6addr.prefixlength >= ipv6addr.bit_end ) {
							fprintf(stderr, " Error: prefix length '%d' higher than given end bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_end);
							exit (1);
						} else {
							ipv6addr.bit_end = ipv6addr.prefixlength;
						};
					} else if (formatoptions & FORMATOPTION_printsuffix) {
						if ( ipv6addr.prefixlength >= ipv6addr.bit_end ) {
							fprintf(stderr, " Error: prefix length '%d' higher than or eqal to given end bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_end);
							exit (1);
						} else if ( ipv6addr.prefixlength >= ipv6addr.bit_start ) {
							fprintf(stderr, " Error: prefix length '%d' higher than or equal to given start bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_start);
							exit (1);
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
				exit (1);
			} else if ( ipv6addr.bit_start == ipv6addr.bit_end ) {
				fprintf(stderr, " Error: start bit equal to end bit\n");
				exit (1);
			};
		};
	};

	
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: result of 'inputtype': %d\n", DEBUG_function_name, retval);
	};

	if (retval != 0) {
		fprintf(stderr, "%s\n", resultstring);
		exit (1);
	};

OUTPUT_type: /* temporary solutions */

	if ( ipv6calc_debug & DEBUG_librfc2874 ) {
		retval = ipv6addrstruct_to_uncompaddr(&ipv6addr, tempstring);
		fprintf(stderr, "%s: got address '%s' (formatoptions: %04lx)\n",  DEBUG_function_name, tempstring, formatoptions);
	};
	
	/* output type */
	switch (outputtype) {
		case -1:
			/* old implementation */
			break;

		case FORMAT_base85:
			retval = addr_to_base85(&ipv6addr, resultstring);
			break;
				
		case FORMAT_bitstring:
			if (ipv6calc_debug) {
				fprintf(stderr, "%s: Call 'librfc2874_addr_to_bitstring'\n", DEBUG_function_name);
			};
			retval = librfc2874_addr_to_bitstring(&ipv6addr, resultstring, formatoptions);
			break;
				
		case FORMAT_revnibbles_int:
		case FORMAT_revnibbles_arpa:
			/* temporary workaround for different formats */
			switch (outputtype) {
				case FORMAT_revnibbles_int:
					retval = librfc1886_addr_to_nibblestring(&ipv6addr, resultstring, formatoptions, "ip6.int.");
					break;
				case FORMAT_revnibbles_arpa:
					retval = librfc1886_addr_to_nibblestring(&ipv6addr, resultstring, formatoptions, "ip6.arpa.");
					break;
			};
			break;
			
		case FORMAT_ifinet6:
			retval = addr_to_ifinet6(&ipv6addr, resultstring);
			break;

		case FORMAT_ipv6addr:
			if (formatoptions & FORMATOPTION_printuncompressed) {
				retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, formatoptions);
			} else if (formatoptions & FORMATOPTION_printfulluncompressed) {
				retval = libipv6addr_ipv6addrstruct_to_fulluncompaddr(&ipv6addr, resultstring, formatoptions);
			} else {
				retval = librfc1884_ipv6addrstruct_to_compaddr(&ipv6addr, resultstring, formatoptions);
			};
			break;

		default:
			fprintf(stderr, " Outputtype isn't implemented\n");
			exit (1);
			break;
	};
	if (outputtype != -1 ) {
		goto RESULT_print;
	};

	/* <- old behavior */	
		switch (command & CMD_MAJOR_MASK) {
			case CMD_mac_to_eui64:
				if ((argc < 1) || (command & CMD_printhelp)) {
					if ((argc < 1) && ! (command & CMD_printhelp)) {
						fprintf(stderr, "missing argument!\n\n");
					};
					mac_to_eui64_printhelplong();
					exit(1);
				};
				retval = mac_to_eui64(argv[0], resultstring);
				break;
				
			case CMD_ifinet6_to_compressed:
				if ((argc < 1) || (command & CMD_printhelp)) {
					if ((argc < 1) && ! (command & CMD_printhelp)) {
						fprintf(stderr, "missing argument!\n\n");
					};
					ifinet6_to_compressed_printhelplong();
					exit(1);
				};
				if (argc < 2) {
					retval = ifinet6_to_compressed(argv[0], resultstring, command);
				} else {
					/* additional prefixlength conversion */
					retval = ifinet6_to_compressedwithprefixlength(argv[0], argv[1], resultstring, command);
				};	
				break;

			case CMD_eui64_to_privacy:
				if ((argc < 2) || (command & CMD_printhelp)) {
					if ((argc < 2) && ! (command & CMD_printhelp)) {
						fprintf(stderr, "missing argument!\n\n");
					};
					eui64_to_privacy_printhelplong();
					exit(1);
				};
				retval = eui64_to_privacy(argv[0], argv[1], resultstring);
				break;
				
			case CMD_ipv4_to_6to4addr:
				if ((argc < 1) || (command & CMD_printhelp)) {
					if ((argc < 2) && ! (command & CMD_printhelp)) {
						fprintf(stderr, "missing argument!\n\n");
					};
					ipv4_to_6to4addr_printhelplong();
					exit(1);
				};
				retval = ipv4_to_6to4addr(argv[0], resultstring);
				break;
				
				
			case CMD_showinfo:
				if ((argc < 1 && ! (command & CMD_showtypes)) || (command & CMD_printhelp)) {
					showinfo_printhelplong();
					exit(1);
				};
				retval = showinfo(&ipv6addr, command);
				break;
		}; /* end switch */
RESULT_print:
		/* print result */
		if (retval == 0) {
			fprintf(stdout, "%s\n", resultstring);
		} else {
			fprintf(stderr, "%s\n", resultstring);
		};
	exit(retval);
};
#undef DEBUG_function_name
