/*
 * Project    : ipv6calc/ipv6logstats
 * File       : ipv6logstatshelp.c
 * Version    : $Id: ipv6logstatshelp.c,v 1.12 2013/11/04 20:30:50 ds6peter Exp $
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
	fprintf(stderr, "%s: version %s", PROGRAM_NAME, PACKAGE_VERSION);

	if (feature_reg == 1) {
		fprintf(stderr, " STAT_REG");
	};

	if (feature_cc == 1) {
		fprintf(stderr, " STAT_CC");
	};

	if (feature_as == 1) {
		fprintf(stderr, " STAT_AS");
	};

	fprintf(stderr, "\n");
};

void printversion_verbose(int level_verbose) {
	printversion();
	fprintf(stderr, "\n");
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

	fprintf(stderr, " Takes web server log data (or any other data which has IPv4/v6 address in first column)\n");
	fprintf(stderr, "   from stdin and print statistics table/list (depending on option) to stdout\n");

	fprintf(stderr, "\n");

	printhelp_common();

	fprintf(stderr, "\n");
	fprintf(stderr, "  [-u|--unknown]             : print unknown IP addresses to stderr\n");
	fprintf(stderr, "  [-c|--colums]              : print statistics in colums (1)\n");
	fprintf(stderr, "  [-n|--noheader]            : don't print header in colums mode (1)\n");
	fprintf(stderr, "  [-o|--onlyheader]          : print only header in colums mode (1)\n");
	fprintf(stderr, "  [-p|--prefix <token>]      : print token as prefix (1)\n");
	fprintf(stderr, "  [-q|--quiet]               : be more quiet\n");
	fprintf(stderr, "  [-s|--simple]              : disable extended statistic (CountryCode/ASN)\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " (1) unsupported for CountryCode & ASN statistics\n");
	fprintf(stderr, "\n");

	return;
};

