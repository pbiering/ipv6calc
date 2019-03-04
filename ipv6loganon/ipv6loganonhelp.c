/*
 * Project    : ipv6calc
 * File       : ipv6loganonhelp.c
 * Version    : $Id$
 * Copyright  : 2007-2019 by Peter Bieringer <pb (at) bieringer.de>
 * License    : GNU GPL v2
 *
 * Information:
 *  Help library
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipv6loganon.h"
#include "ipv6calctypes.h"
#include "ipv6calccommands.h"
#include "ipv6calchelp.h"
#include "config.h"

/* display info */
void printversion(void) {
	fprintf(stderr, "%s: version %s", PROGRAM_NAME, PACKAGE_VERSION);

	if (feature_zeroize == 1) {
		fprintf(stderr, " ANON_ZEROISE");
	};

	if (feature_anon == 1) {
		fprintf(stderr, " ANON_ANONYMIZE");
	};

	if (feature_kp == 1) {
		fprintf(stderr, " ANON_KEEP-TYPE-ASN-CC");
	};

	if (feature_kg == 1) {
		fprintf(stderr, " ANON_KEEP-TYPE-GEONAMEID");
	};

	fprintf(stderr, "\n");
};

void printversion_help(void) {
	fprintf(stderr, "\n");

	fprintf(stderr, "Explanation of available(x)/possible(o) feature tokens\n");

	fprintf(stderr, "%-25s%c %s\n", "ANON_ZEROISE", (feature_zeroize == 1)? 'x' : 'o', "Anonymization method 'zeroize'");
	fprintf(stderr, "%-25s%c %s\n", "ANON_ANONYMIZE", (feature_anon == 1) ? 'x' : 'o', "Anonymization method 'anonymize'");
	fprintf(stderr, "%-25s%c %s\n", "ANON_KEEP-TYPE-ASN-CC", (feature_kp == 1) ? 'x' : 'o', "Anonymization method 'keep-type-asn-cc'");
	fprintf(stderr, "%-25s%c %s\n", "ANON_KEEP-TYPE-GEONAMEID", (feature_kg == 1) ? 'x' : 'o', "Anonymization method 'keep-type-geonameid'");

	fprintf(stderr, "\n");
};

void printcopyright(void) {
        fprintf(stderr, "%s\n", PROGRAM_COPYRIGHT);
};

void ipv6loganon_printinfo(void)  {
	printversion();
	printcopyright();
	fprintf(stderr, "This program anonymizes IPv4/IPv6 addresses in HTTP server log files\n");
	fprintf(stderr, "See '%s -?|-h|--help for online help\n", PROGRAM_NAME);
};

/* print global help */
void ipv6loganon_printhelp(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "\n");
	fprintf(stderr, "This program anonymizes IPv4/IPv6 addresses in e.g. HTTP server log files\n");
	
	printhelp_common(IPV6CALC_HELP_ALL);

	fprintf(stderr, "  [-w|--write]               : write output to file instead of stdout\n");
	fprintf(stderr, "  [-a|--append]              : append output to file instead of stdout\n");
	fprintf(stderr, "  [-f|--flush]               : flush output after each line\n");
	fprintf(stderr, "  [-V|--verbose]             : be verbose\n");
	fprintf(stderr, "  [-n|--nocache]             : disable caching\n");
	fprintf(stderr, "  [-c|--cachelimit <value>]  : set cache limit\n");
	fprintf(stderr, "                               default: %d\n", cache_lru_limit);
	fprintf(stderr, "                               maximum: %d\n", CACHE_LRU_SIZE);

	printhelp_action_dispatcher(ACTION_anonymize, 1);

	fprintf(stderr, "\n");
	fprintf(stderr, " Takes data from stdin, outputs the processed data to stdout (default)\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\n");

	return;
};
