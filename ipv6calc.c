/*
 * ipv6calc	This program print out different formats of an given IPv6 address
 *
 * Version:     $Id: ipv6calc.c,v 1.1 2001/10/07 14:47:40 peter Exp $
 * 
 * Author:      Peter Bieringer <pb@bieringer.de>
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <getopt.h> 
#include "ipv6calc.h"

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
	fprintf(stderr, "Usage: (see '%s --command -?' for more help)\n", PROGRAM_NAME);
	fprintf(stderr, " [-d|--debug debugvalue)\n");

	addr_to_ip6int_printhelp();
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

int main(int argc,char *argv[]) {
	char resultstring[NI_MAXHOST];
	int retval = 1, i, lop;
	unsigned long int command = 0;

	/* defined options */
	char *shortopts = "vh?rd:iul";

    struct option longopts[] = {
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{"debug", 1, 0, 'd'},
		
		/* command options */
		{"addr2ip6_int", 0, 0, CMD_addr_to_ip6int },
		{"addr_to_ip6int", 0, 0, CMD_addr_to_ip6int },
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

		/* format options */
		{ "printprefix", 0, 0, CMD_printprefix },
		{ "printsuffix", 0, 0, CMD_printsuffix },
		{ "maskprefix",  0, 0, CMD_maskprefix },
		{ "masksuffix",  0, 0, CMD_masksuffix },
		{ "uppercase",   0, 0, CMD_printuppercase },
		{ "lowercase",   0, 0, CMD_printlowercase },

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
		switch (command & CMD_MAJOR_MASK) {
			case CMD_addr_to_ip6int:
				if ((argc < 1) || (command & CMD_printhelp)) {
					if ((argc < 1) && ! (command & CMD_printhelp)) {
						fprintf(stderr, "missing argument!\n\n");
					};
					addr_to_ip6int_printhelplong();
					exit(1);
				};
				retval = addr_to_ip6int(argv[0], resultstring);
				fprintf(stdout, "%s\n", resultstring);
				break;
				
			case CMD_addr_to_compressed:
				if ((argc < 1) || (command & CMD_printhelp)) {
					if ((argc < 1) && ! (command & CMD_printhelp)) {
						fprintf(stderr, "missing argument!\n\n");
					};
					addr_to_compressed_printhelplong();
					exit(1);
				};
				retval = addr_to_compressed(argv[0], resultstring);
				fprintf(stdout, "%s\n", resultstring);
				break;
				
			case CMD_addr_to_uncompressed:
				if ((argc < 1) || (command & CMD_printhelp)) {
					if ((argc < 1) && ! (command & CMD_printhelp)) {
						fprintf(stderr, "missing argument!\n\n");
					};
					addr_to_uncompressed_printhelplong();
					exit(1);
				};
				retval = addr_to_uncompressed(argv[0], resultstring, command & CMD_FORMAT_MASK);
				fprintf(stdout, "%s\n", resultstring);
				break;
				
			case CMD_addr_to_fulluncompressed:
				if ((argc < 1) || (command & CMD_printhelp)) {
					if ((argc < 1) && ! (command & CMD_printhelp)) {
						fprintf(stderr, "missing argument!\n\n");
					};
					addr_to_fulluncompressed_printhelplong();
					exit(1);
				};
				retval = addr_to_fulluncompressed(argv[0], resultstring, command & CMD_FORMAT_MASK);
				fprintf(stdout, "%s\n", resultstring);
				break;
				
			case CMD_addr_to_base85:
				if ((argc < 1) || (command & CMD_printhelp)) {
					if ((argc < 1) && ! (command & CMD_printhelp)) {
						fprintf(stderr, "missing argument!\n\n");
					};
					addr_to_base85_printhelplong();
					exit(1);
				};
				retval = addr_to_base85(argv[0], resultstring);
				fprintf(stdout, "%s\n", resultstring);
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
				fprintf(stdout, "%s\n", resultstring);
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
				fprintf(stdout, "%s\n", resultstring);
				break;
				
			case CMD_addr_to_ifinet6:
				if ((argc < 1) || (command & CMD_printhelp)) {
					if ((argc < 1) && ! (command & CMD_printhelp)) {
						fprintf(stderr, "missing argument!\n\n");
					};
					addr_to_ifinet6_printhelplong();
					exit(1);
				};
				retval = addr_to_ifinet6(argv[0], resultstring);
				fprintf(stdout, "%s\n", resultstring);
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
					retval = ifinet6_to_compressed(argv[0], resultstring);
				} else {
					/* additional prefixlength conversion */
					retval = ifinet6_to_compressedwithprefixlength(argv[0], argv[1], resultstring);
				};	
				fprintf(stdout, "%s\n", resultstring);
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
				fprintf(stdout, "%s\n", resultstring);
				break;
				
			case CMD_showinfo:
				if ((argc < 1) || (command & CMD_printhelp)) {
					if ((argc < 1) && ! (command & CMD_printhelp)) {
						fprintf(stderr, "missing argument!\n\n");
					};
					showinfo_printhelplong();
					exit(1);
				};
				retval = showinfo(argv[0], resultstring);
				fprintf(stdout, "%s\n", resultstring);
				break;
		}; /* end switch */
	} else {
		if (command & CMD_printhelp) {
			printhelp();
		};
	}; /* end else */
	
	exit(retval);
};
