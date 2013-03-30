/*
 * Project    : ipv6calc
 * File       : ipv6loganonhelp.c
 * Version    : $Id: ipv6loganonhelp.c,v 1.9 2013/03/30 18:03:45 ds6peter Exp $
 * Copyright  : 2007-2013 by Peter Bieringer <pb (at) bieringer.de>
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
	fprintf(stderr, "This program anonymizes IPv4/IPv6 addresses in e.g. HTTP server log files\n");
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

	printhelp_action_dispatcher(ACTION_anonymize, 1);
	fprintf(stderr, "\n");
	fprintf(stderr, " Takes data from stdin, outputs the processed data to stdout (default)\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\n");

	return;
};

