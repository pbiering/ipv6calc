/*
 * Project    : ipv6calc
 * File       : libhelp.c
 * Version    : $Id: libhelp.c,v 1.2 2002/03/02 17:27:27 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Help libary
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "ipv6calctypes.h"
#include "libhelp.h"


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

/* format option arguments */
void printhelp_print(void) {
	fprintf(stderr, "   --printprefix        : print only prefix of IPv6 address\n");
        fprintf(stderr, "   --printsuffix        : print only suffix of IPv6 address\n");
};

void printhelp_mask(void) {
	fprintf(stderr, "   --maskprefix         : mask IPv6 address with prefix length (clears suffix bits)\n");
        fprintf(stderr, "   --masksuffix         : mask IPv6 address with suffix length (clears prefix bits)\n");
};

void printhelp_case(void) {
	fprintf(stderr, "   --uppercase|-u       : print chars of IPv6 address in upper case\n");
        fprintf(stderr, "   --lowercase|-l       : print chars of IPv6 address in lower case [default]\n");
};

void printhelp_printstartend(void) {
	fprintf(stderr, "   --printstart <1-128> : print part of IPv6 address start from given number\n");
	fprintf(stderr, "   --printend   <1-128> : print part of IPv6 address end at given number\n");
};

void printhelp_doublecommands(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "   Only one command may be specified!\n");
};

void printhelp_missinginputdata(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "   Missing or to few input data given!\n");
};

/* list of input types */
void printhelp_inputtypes(void) {
	int i, j, maxlen = 0;
	char printformatstring[20];
	
	printversion();
	printcopyright();

	/* look for longest type definition */
	for (j = 0; j < sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0]); j++) {
		if (strlen(ipv6calc_formatstrings[j].token) > maxlen) {
			maxlen = strlen(ipv6calc_formatstrings[j].token);
		};
	};

	if (maxlen > 999) {
		fprintf(stderr, " Something going wrong with array 'ipv6calc_formatstrings'!\n");
		exit (1);
	};

	sprintf(printformatstring, "  %%-%ds: %%s\n", maxlen);

	if (ipv6calc_debug) {
		fprintf(stderr, "Format string: %s\n", printformatstring);
	};

	fprintf(stderr, "\n Available input types:\n");

	/* run through matrix */
	for (i = 0; i < sizeof(ipv6calc_formatmatrix) / sizeof(ipv6calc_formatmatrix[0]); i++) {
		if (ipv6calc_debug) {
			fprintf(stderr, "Row %d: %04lx - %04lx\n", i, ipv6calc_formatmatrix[i][0], ipv6calc_formatmatrix[i][1]);
		};
		if (ipv6calc_formatmatrix[i][1] != 0) {
			/* available for input, look for name now */
			for (j = 0; j < sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0]); j++) {
				if (ipv6calc_debug) {
					fprintf(stderr, "Format-Row %d: %04lx - %s - %s\n", j, ipv6calc_formatstrings[j].number, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
				};
				if (ipv6calc_formatstrings[j].number == ipv6calc_formatmatrix[i][0]) {
					fprintf(stderr, printformatstring, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
				};
			};
		};
	};
};


/* list of output types */
void printhelp_outputtypes(long int inputtype) {
	int i, j, maxlen = 0;
	char printformatstring[20];
	
	printversion();
	printcopyright();

	/* look for longest type definition */
	for (j = 0; j < sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0]); j++) {
		if (strlen(ipv6calc_formatstrings[j].token) > maxlen) {
			maxlen = strlen(ipv6calc_formatstrings[j].token);
		};
	};

	if (maxlen > 999) {
		fprintf(stderr, " Something going wrong with array 'ipv6calc_formatstrings'!\n");
		exit (1);
	};

	sprintf(printformatstring, "  %%-%ds: %%s\n", maxlen);

	if (ipv6calc_debug) {
		fprintf(stderr, "Format string: %s\n", printformatstring);
	};

	if (inputtype > 0) {
		fprintf(stderr, "\n Available output types filtered by input type:\n");
	} else {
		fprintf(stderr, "\n Available output types:\n");
	};

	/* run through matrix */
	for (j = 0; j < sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0]); j++) {
		if (ipv6calc_debug) {
			fprintf(stderr, "Format-Row %d: %04lx - %s - %s\n", j, ipv6calc_formatstrings[j].number, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
		};

		for (i = 0; i < sizeof(ipv6calc_formatmatrix) / sizeof(ipv6calc_formatmatrix[0]); i++) {
			if (inputtype > 0) {
				if (ipv6calc_formatmatrix[i][0] != inputtype) {
					/* skip */
					continue;
				};
			};
		
			if (ipv6calc_debug) {
				fprintf(stderr, "Row %d: %04lx - %04lx\n", i, ipv6calc_formatmatrix[i][0], ipv6calc_formatmatrix[i][1]);
			};
			if (ipv6calc_formatmatrix[i][1] & ipv6calc_formatstrings[j].number) {
				/* available for input, look for name now */
				fprintf(stderr, printformatstring, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
				break;
			};
		};

	};
};

/* print global help */
void printhelp() {
	printversion();
	printcopyright();
	fprintf(stderr, "\nGeneral:\n");
	fprintf(stderr, " [-d|--debug <debug value>] ...\n");
	
	fprintf(stderr, "Usage with new style options:\n");
	fprintf(stderr, " %s --inputtype|--intype <input type> --outputtype|--outtype <output type> [<format option> ...] <input data>\n", PROGRAM_NAME);
	fprintf(stderr, "  For more help about input types see:\n");
	fprintf(stderr, "   %s --inputtype|--intype -?\n", PROGRAM_NAME);
	fprintf(stderr, "  For more help about output types see:\n");
	fprintf(stderr, "   %s --outputtype|--outtype -?\n", PROGRAM_NAME);

	fprintf(stderr, "\nOld style options:\n");
	fprintf(stderr, "Usage: (see '%s <command> -?' for more help)\n", PROGRAM_NAME);
	/*addr_to_ip6int_printhelp();*/
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
	ipv4_to_6to4addr_printhelp();
	showinfo_printhelp();

	return;
};
