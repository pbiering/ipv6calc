/*
 * Project    : ipv6calc/ipv6logstats
 * File       : ipv6logstatshelp.c
 * Version    : $Id: ipv6logstatshelp.c,v 1.15 2014/07/22 06:00:41 ds6peter Exp $
 * Copyright  : 2003-2014 by Peter Bieringer <pb (at) bieringer.de>
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

void printversion_help(void) {
	fprintf(stderr, "\n");

	fprintf(stderr, "Explanation of available(+)/possible(-) feature tokens\n");

	fprintf(stderr, "%-22s%c %s\n", "STAT_REG", (feature_reg == 1)? '+' : '-', "Statistics by Registry");
	fprintf(stderr, "%-22s%c %s\n", "STAT_CC", (feature_cc == 1) ? '+' : '-', "Statistics by CountryCode");
	fprintf(stderr, "%-22s%c %s\n", "STAT_AS", (feature_as == 1) ? '+' : '-', "Statistics by Autonomous System Number");

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
	fprintf(stderr, "See '%s -?|-h|--help for online help\n", PROGRAM_NAME);
};

/* print global help */
void ipv6logstats_printhelp(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "\n");

	fprintf(stderr, " Takes web server log data (or any other data which has IPv4/v6 address in first column)\n");
	fprintf(stderr, "   from stdin and print statistics table/list (depending on option) to stdout\n");

	printhelp_common(IPV6CALC_HELP_ALL);

	fprintf(stderr, "  [-u|--unknown]             : print unknown IP addresses to stderr\n");
	fprintf(stderr, "  [-c|--columns]             : print statistics in columns (1)\n");
	fprintf(stderr, "  [-N|--column-numbers]      : print column numbers\n");
	fprintf(stderr, "  [-n|--noheader]            : don't print header in columns mode (1)\n");
	fprintf(stderr, "  [-o|--onlyheader]          : print only header in columns mode (1)\n");
	fprintf(stderr, "  [-p|--prefix <token>]      : print token as prefix (1)\n");
	fprintf(stderr, "  [-s|--simple]              : disable extended statistic (CountryCode/ASN)\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " (1) unsupported for CountryCode & ASN statistics\n");
	fprintf(stderr, "\n");

	return;
};

