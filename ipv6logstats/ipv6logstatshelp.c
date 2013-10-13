/*
 * Project    : ipv6calc/ipv6logstats
 * File       : ipv6logstatshelp.c
 * Version    : $Id: ipv6logstatshelp.c,v 1.9 2013/10/13 16:18:44 ds6peter Exp $
 * Copyright  : 2003-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Help library
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipv6logstats.h"
#include "ipv6calctypes.h"
#include "ipv6calccommands.h"
#include "ipv6calchelp.h"
#include "config.h"

#include "../databases/lib/libipv6calc_db_wrapper.h"

/* display info */
void printversion(void) {
	char resultstring[NI_MAXHOST] = "";

	libipv6calc_db_wrapper_features(resultstring, sizeof(resultstring));

	fprintf(stderr, "%s: version %s", PROGRAM_NAME, PACKAGE_VERSION);
	fprintf(stderr, " %s", resultstring);

	if (feature_cc == 1) {
		fprintf(stderr, " STAT_CC");
	};

	if (feature_cc == 1) {
		fprintf(stderr, " STAT_AS");
	};

	fprintf(stderr, "\n");
};

void printversion_verbose(int level_verbose) {
	printversion();

	ipv6calc_print_features_verbose(level_verbose);
};

void printcopyright(void) {
        fprintf(stderr, "%s\n", PROGRAM_COPYRIGHT);
};

void ipv6logstats_printinfo(void)  {
	printversion();
	printcopyright();
	fprintf(stderr, "This program creates a statistics of client IPv4/IPv6 addresses from HTTP server log files\n");
	fprintf(stderr, "See '%s -?' for more details\n\n", PROGRAM_NAME);
};

/* print global help */
void ipv6logstats_printhelp(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "\n");
	fprintf(stderr, " General:\n");
	fprintf(stderr, "  [-d|--debug <debug value>] : debug value (bitwise like)\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  [-u|--unknown]             : print unknown IP addresses to stderr\n");
	fprintf(stderr, "  [-c|--colums]              : print statistics in colums (1)\n");
	fprintf(stderr, "  [-n|--noheader]            : don't print header in colums mode (1)\n");
	fprintf(stderr, "  [-o|--onlyheader]          : print only header in colums mode (1)\n");
	fprintf(stderr, "  [-p|--prefix <token>]      : print token as prefix (1)\n");
	fprintf(stderr, "  [-q|--quiet]               : be more quiet\n");
	fprintf(stderr, "  [-v|--version]             : version information (2)\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " (1) unsupported for CountryCode & ASN statistics\n");
	fprintf(stderr, " (2) two additinal levels of verbosity supported by using more than one time\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " Takes web server log data (or any other data which has IPv4/v6 address in first column)\n");
	fprintf(stderr, "   from stdin and print statistics table/list (depending on option) to stdout\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\n");

	return;
};

