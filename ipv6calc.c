/*
 * Project    : ipv6calc
 * File       : ipv6calc.c
 * Version    : $Id: ipv6calc.c,v 1.7 2002/02/27 08:07:59 peter Exp $
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

#include "addr_to_ip6int.h"
#include "addr_to_bitstring.h"
#include "addr_to_compressed.h"
#include "addr_to_uncompressed.h"
#include "addr_to_fulluncompressed.h"
#include "addr_to_ifinet6.h"
#include "addr_to_base85.h"
#include "base85_to_addr.h"
#include "ifinet6_to_compressed.h"
#include "eui64_to_privacy.h"
#include "mac_to_eui64.h"
#include "showinfo.h"

long int ipv6calc_debug = 0;

/* display info */
void printversion()  {
	fprintf(stderr, "%s  version: %s\n", PROGRAM_NAME, PROGRAM_VERSION);
};

void printcopyright()  {
	fprintf(stderr, "%s\n", PROGRAM_COPYRIGHT);
};

void printinfo()  {
	printversion();
	printcopyright();
	fprintf(stderr, "This program formats and calculates IPv6 addresses\n");
	fprintf(stderr, "See '%s -?' for more details\n\n", PROGRAM_NAME);
};

/* display help */
void printhelp() {
	printversion();
	printcopyright();
	fprintf(stderr, "Usage: (see '%s <command> -?' for more help)\n", PROGRAM_NAME);
	fprintf(stderr, " [-d|--debug <debugvalue>]\n");

	addr_to_ip6int_printhelp();
	addr_to_bitstring_printhelp();
	addr_to_compressed_printhelp();
	addr_to_uncompressed_printhelp();
	addr_to_fulluncompressed_printhelp();
	addr_to_ifinet6_printhelp();
	ifinet6_to_compressed_printhelp();
	addr_to_base85_printhelp();
	base85_to_addr_printhelp();
	mac_to_eui64_printhelp();
	eui64_to_privacy_printhelp();
	showinfo_printhelp();

	return;
};

/**************************************************/
/* main */
#define DEBUG_function_name "ipv6calc/main"
int main(int argc,char *argv[]) {
	char resultstring[NI_MAXHOST] = "";
	int retval = 1, i, lop;
	unsigned long int command = 0;
	unsigned short bit_start = 0, bit_end = 0;
	ipv6calc_ipv6addr ipv6addr;

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
		{ "showinfo", 0, 0, CMD_showinfo },
		{ "machine_readable", 0, 0, CMD_machinereadable },
		{ "show_types", 0, 0, CMD_showtypes },

		/* format options */
		{ "printprefix", 0, 0, CMD_printprefix },
		{ "printsuffix", 0, 0, CMD_printsuffix },
		{ "maskprefix",  0, 0, CMD_maskprefix },
		{ "masksuffix",  0, 0, CMD_masksuffix },
		{ "uppercase",   0, 0, CMD_printuppercase },
		{ "lowercase",   0, 0, CMD_printlowercase },
		{ "printstart",  1, 0, CMD_printstart },
		{ "printend",    1, 0, CMD_printend },

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

			/* command options */
			case 'r':
			case CMD_addr_to_ip6int:
				command |= CMD_addr_to_ip6int;
				break;

			case 'a':
			case CMD_addr_to_ip6arpa:
				command |= CMD_addr_to_ip6arpa;
				break;

			case 'b':
			case CMD_addr_to_bitstring:
				command |= CMD_addr_to_bitstring;
				break;
				
			case CMD_addr_to_compressed:
				command |= CMD_addr_to_compressed;
				break;
				
			case CMD_addr_to_uncompressed:
				command |= CMD_addr_to_uncompressed;
				break;
				
			case CMD_addr_to_base85:
				command |= CMD_addr_to_base85;
				break;
				
			case CMD_base85_to_addr:
				command |= CMD_base85_to_addr;
				break;

			case CMD_mac_to_eui64:
				command |= CMD_mac_to_eui64;
				break;
				
			case CMD_addr_to_fulluncompressed:
				command |= CMD_addr_to_fulluncompressed;
				break;
				
			case CMD_addr_to_ifinet6:
				command |= CMD_addr_to_ifinet6;
				break;

			case CMD_ifinet6_to_compressed:
				command |= CMD_ifinet6_to_compressed;
				break;

			case CMD_eui64_to_privacy:
				command |= CMD_eui64_to_privacy;
				break;

			case 'i':
			case CMD_showinfo:
				command |= CMD_showinfo;
				break;

			/* format options */
			case CMD_printprefix:
				command |= CMD_printprefix;
				break;
				
			case CMD_printsuffix:
				command |= CMD_printsuffix;
				break;
				
			case CMD_maskprefix:
				command |= CMD_maskprefix;
				break;
				
			case CMD_masksuffix:
				command |= CMD_masksuffix;
				break;
			
			case 'l':	
			case CMD_printlowercase:
				command |= CMD_printlowercase;
				break;
				
			case 'u':	
			case CMD_printuppercase:
				command |= CMD_printuppercase;
				break;
				
			case 'm':	
			case CMD_machinereadable:
				command |= CMD_machinereadable;
				break;

			case CMD_showtypes:
				command |= CMD_showtypes;
				break;

			case CMD_printstart:
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_start = atoi(optarg);
					command |= CMD_printstart;
				} else {
					fprintf(stderr, " Argument of option 'printstart' is out or range (1-128): %d\n", atoi(optarg));
					exit (1);
				};
				break;
				
			case CMD_printend:
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_end = atoi(optarg);
					command |= CMD_printend;
				} else {
					fprintf(stderr, " Argument of option 'printend' is out or range (1-128): %d\n", atoi(optarg));
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

/*	fprintf(stderr, "Command value: %lx\n", command); */
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Debug value: %lx\n", ipv6calc_debug); 
	};
	
	/* do work depending on selection */
	if (command & CMD_printversion) {
		printversion();
		exit(1);
	};

	if (command & CMD_MAJOR_MASK) {	
		if (command & CMD_MAJOR_ADDR_INPUT) {
			/* normal IPv6 address needed as argument */
			if (argc >= 1) {
				if (ipv6calc_debug) {
					fprintf(stderr, "%s: Got input %s\n", DEBUG_function_name, argv[0]);
				};
				
				/* get IPv6 address */
				retval = addr_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);

				if (ipv6calc_debug) {
					fprintf(stderr, "%s: result of 'addr_to_ipv6addrstruct': %d\n", DEBUG_function_name, retval);
				};

				if (retval != 0) {
					fprintf(stderr, "%s\n", resultstring);
					exit (1);
				};
				
				/* mask bits */
				if (command & (CMD_maskprefix | CMD_masksuffix)) {
					if (ipv6addr.flag_prefixuse == 1) {
						if (command & CMD_maskprefix) {
							ipv6addrstruct_maskprefix(&ipv6addr);
						} else if (command & CMD_masksuffix) {
							ipv6addrstruct_masksuffix(&ipv6addr);
						};
					} else {
						fprintf(stderr, " Error: mask option used without specifying a prefix length\n");
						exit (1);
					};
				};

				/* start bit */
				if ( command & CMD_printstart ) {
					if (ipv6calc_debug) {
						fprintf(stderr, "%s: Set bit start to: %d\n", DEBUG_function_name, bit_start);
					};
					ipv6addr.bit_start = bit_start;
					ipv6addr.flag_startend_use = 1;
				} else {
					ipv6addr.bit_start = 1;
				};
				
				/* end bit */
				if ( command & CMD_printend ) {
					if (ipv6calc_debug) {
						fprintf(stderr, "%s: Set bit end to: %d\n", DEBUG_function_name, bit_end);
					};
					ipv6addr.bit_end = bit_end;
					ipv6addr.flag_startend_use = 1;
				} else {
					/* default */
					ipv6addr.bit_end = 128;
				};

				/* prefix */
				if ( command & (CMD_printprefix | CMD_printsuffix) ) {
					if ( ipv6addr.flag_prefixuse == 0 ) {
						fprintf(stderr, " Error: missing prefix length for printing prefix/suffix\n");
						exit (1);
					} else {
						if ( ipv6addr.flag_startend_use == 0 ) {
							/* only print[prefix|suffix] */
							if ( command & CMD_printprefix ) {
								ipv6addr.bit_start = 1;
								ipv6addr.bit_end = ipv6addr.prefixlength;
								ipv6addr.flag_startend_use = 1;
							} else if ( command & CMD_printsuffix ) {
								ipv6addr.bit_start = ipv6addr.prefixlength + 1;
								ipv6addr.bit_end = 128;
								ipv6addr.flag_startend_use = 1;
							};
						} else {
							/* mixed */
							if ( command & CMD_printprefix ) {
								if ( ipv6addr.prefixlength < ipv6addr.bit_start ) {
									fprintf(stderr, " Error: prefix length '%d' lower than given start bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_start);
									exit (1);
								} else if ( ipv6addr.prefixlength >= ipv6addr.bit_end ) {
									fprintf(stderr, " Error: prefix length '%d' higher than given end bit number '%d'\n", ipv6addr.prefixlength, ipv6addr.bit_end);
									exit (1);
								} else {
									ipv6addr.bit_end = ipv6addr.prefixlength;
								};
							} else if ( command & CMD_printsuffix ) {
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
		};
		
		switch (command & CMD_MAJOR_MASK) {
			case CMD_addr_to_ip6int:
			case CMD_addr_to_ip6arpa:
				if ((argc < 1) || (command & CMD_printhelp)) {
					addr_to_ip6int_printhelplong();
					exit(1);
				};
				retval = addr_to_ip6int(&ipv6addr, resultstring, command);
				break;

			case CMD_addr_to_bitstring:
				if ((argc < 1) || (command & CMD_printhelp)) {
					addr_to_bitstring_printhelplong();
					exit(1);
				};
				retval = addr_to_bitstring(&ipv6addr, resultstring, command);
				break;
				
			case CMD_addr_to_compressed:
				if ((argc < 1) || (command & CMD_printhelp)) {
					addr_to_compressed_printhelplong();
					exit(1);
				};
				retval = addr_to_compressed(&ipv6addr, resultstring, command);
				break;
				
			case CMD_addr_to_uncompressed:
				if ((argc < 1) || (command & CMD_printhelp)) {
					addr_to_uncompressed_printhelplong();
					exit(1);
				};
				retval = addr_to_uncompressed(&ipv6addr, resultstring, command);
				break;
				
			case CMD_addr_to_fulluncompressed:
				if ((argc < 1) || (command & CMD_printhelp)) {
					addr_to_fulluncompressed_printhelplong();
					exit(1);
				};
				retval = addr_to_fulluncompressed(&ipv6addr, resultstring, command);
				break;
				
			case CMD_addr_to_base85:
				if ((argc < 1) || (command & CMD_printhelp)) {
					addr_to_base85_printhelplong();
					exit(1);
				};
				retval = addr_to_base85(&ipv6addr, resultstring);
				break;
				
			case CMD_base85_to_addr:
				if ((argc < 1) || (command & CMD_printhelp)) {
					if ((argc < 1) && ! (command & CMD_printhelp)) {
						fprintf(stderr, "missing argument!\n\n");
					};
					base85_to_addr_printhelplong();
					exit(1);
				};
				retval = base85_to_addr(argv[0], resultstring);
				break;
				
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
				
			case CMD_addr_to_ifinet6:
				if ((argc < 1) || (command & CMD_printhelp)) {
					addr_to_ifinet6_printhelplong();
					exit(1);
				};
				retval = addr_to_ifinet6(&ipv6addr, resultstring);
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
				
			case CMD_showinfo:
				if ((argc < 1 && ! (command & CMD_showtypes)) || (command & CMD_printhelp)) {
					showinfo_printhelplong();
					exit(1);
				};
				retval = showinfo(&ipv6addr, command);
				break;
		}; /* end switch */

		/* print result */
		if (retval == 0) {
			fprintf(stdout, "%s\n", resultstring);
		} else {
			fprintf(stderr, "%s\n", resultstring);
		};
	} else {
		if (command & CMD_printhelp) {
			printhelp();
		};
	}; /* end else */
	
	exit(retval);
};
#undef DEBUG_function_name
