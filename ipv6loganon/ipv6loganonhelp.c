/*
 * Project    : ipv6calc
 * File       : ipv6loganonhelp.c
 * Version    : $Id: ipv6loganonhelp.c,v 1.7 2011/05/12 10:30:47 peter Exp $
 * Copyright  : 2007 by Peter Bieringer <pb (at) bieringer.de>
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
	fprintf(stderr, "%s: version %s\n", PROGRAM_NAME, PACKAGE_VERSION);
};

void printcopyright(void) {
        fprintf(stderr, "%s\n", PROGRAM_COPYRIGHT);
};

void ipv6loganon_printinfo(void)  {
	printversion();
	printcopyright();
	fprintf(stderr, "This program anonymizes IPv4/IPv6 addresses in HTTP server log files\n");
	fprintf(stderr, "See '%s -?' for more details\n\n", PROGRAM_NAME);
};

/* print global help */
void ipv6loganon_printhelp(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "\n");
	fprintf(stderr, "This program anonymizes IPv4/IPv6 addresses in HTTP server log files\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " General:\n");
	fprintf(stderr, "  [-d|--debug <debug value>] : debug value (bitwise like)\n");
	fprintf(stderr, "  [-w|--write]               : write output to file instead of stdout\n");
	fprintf(stderr, "  [-a|--append]              : append output to file instead of stdout\n");
	fprintf(stderr, "  [-f|--flush]               : flush output after each line\n");
	fprintf(stderr, "  [-V|--verbose]             : be verbose\n");
	fprintf(stderr, "  [-n|--nocache]             : disable caching\n");
	fprintf(stderr, "  [-c|--cachelimit <value>]  : set cache limit\n");
	fprintf(stderr, "                               default: %d\n", cache_lru_limit);
	fprintf(stderr, "                               maximum: %d\n", CACHE_LRU_SIZE);
	fprintf(stderr, " Output anonymization:\n");
	fprintf(stderr, "  --mask-ipv4 <bits>     : mask all IPv4 addresses [0-32], default 24\n");
	fprintf(stderr, "                            even if occurs in IPv6-IID\n");
	fprintf(stderr, "  --mask-ipv6 <bits>     : mask IPv6 prefix [0-64], default 48\n");
	fprintf(stderr, "                            only applied to related address types\n");
	fprintf(stderr, "  --no-mask-iid          : do not mask non-IPv4 based IPv6-IID\n");
	fprintf(stderr, "  --anonymize-standard   : preset for standard anonymization\n");
	fprintf(stderr, "     (default)              mask-ipv4=24 mask-ipv6=48 mask-iid\n");
	fprintf(stderr, "  --anonymize-careful    : preset for careful anonymization\n");
	fprintf(stderr, "                            mask-ipv4=20 mask-ipv6=40 mask-iid\n");
	fprintf(stderr, "  --anonymize-paranoid   : preset for paranoid anonymization\n");
	fprintf(stderr, "                            mask-ipv4=16 mask-ipv6=32 mask-iid\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " Takes data from stdin, outputs the processed data to stdout (default)\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\n");

	return;
};

