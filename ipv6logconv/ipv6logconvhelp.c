/*
 * Project    : ipv6calc
 * File       : ipv6logconvhelp.c
 * Version    : $Id$
 * Copyright  : 2002-2014 by Peter Bieringer <pb (at) bieringer.de>
 * License    : GNU GPL v2
 *
 * Information:
 *  Help library
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipv6logconv.h"
#include "ipv6calctypes.h"
#include "ipv6calccommands.h"
#include "ipv6calchelp.h"
#include "config.h"

/* display info */
void printversion(void) {
	fprintf(stderr, "%s: version %s", PROGRAM_NAME, PACKAGE_VERSION);

	if (feature_reg == 1) {
		fprintf(stderr, " CONV_REG");
	};

	if (feature_ieee == 1) {
		fprintf(stderr, " CONV_IEEE");
	};

	fprintf(stderr, "\n");
};

void printversion_help(void) {
	fprintf(stderr, "\n");

	fprintf(stderr, "Explanation of available(+)/possible(-) feature tokens\n");

	fprintf(stderr, "%-22s%c %s\n", "CONV_REG", (feature_reg == 1)? '+' : '-', "Conversion of IPv4/IPv6 address to Registry tokens");
	fprintf(stderr, "%-22s%c %s\n", "CONV_IEEE", (feature_ieee == 1) ? '+' : '-', "Conversion of included MAC/EUI-64 adress to IEEE (vendor) name");

	fprintf(stderr, "\n");
};

void printcopyright(void) {
        fprintf(stderr, "%s\n", PROGRAM_COPYRIGHT);
};

void ipv6logconv_printinfo(void)  {
	printversion();
	printcopyright();
	fprintf(stderr, "This program converts IPv4/IPv6 addresses in HTTP server log files\n");
	fprintf(stderr, "See '%s -?|-h|--help for online help\n", PROGRAM_NAME);
};

/* print global help */
void ipv6logconv_printhelp(void) {
	printversion();
	printcopyright();

	fprintf(stderr, "\n");
	fprintf(stderr, "This program converts IPv4/IPv6 addresses in HTTP server log files\n");

	printhelp_common(IPV6CALC_HELP_BASIC);

	fprintf(stderr, " Performance options:\n");
	fprintf(stderr, "  [-n|--nocache]            : disable caching\n");
	fprintf(stderr, "  [-c|--cachelimit <value>] : set cache limit\n");
	fprintf(stderr, "                               default: %d\n", cache_lru_limit);
	fprintf(stderr, "                               maximum: %d\n", CACHE_LRU_SIZE);
	fprintf(stderr, " Output options:\n");
	fprintf(stderr, "  [--out <output type>] : specify output type\n");
	fprintf(stderr, "   addrtype       : Address type%s\n", (feature_reg == 0) ? "  (NOT-SUPPORTED)" : "");
	fprintf(stderr, "   ouitype        : OUI (IEEE) type%s\n", (feature_ieee == 0) ? "  (NOT-SUPPORTED)" : "");
	fprintf(stderr, "   ipv6addrtype   : IPv6 address type\n");
	fprintf(stderr, "   any            : any type%s\n", ((feature_reg == 0) || (feature_ieee == 0)) ? "  (NOT-SUPPORTED)" : "");
	fprintf(stderr, "\n");
	if ((feature_reg == 0) || (feature_ieee == 0)) {
			fprintf(stderr, " NOT-SUPPORTED means either database missing or support not compiled-in\n");
	};
	fprintf(stderr, "\n");
	fprintf(stderr, " Takes data from stdin, proceed it to stdout\n");
	fprintf(stderr, "\n");

	return;
};

