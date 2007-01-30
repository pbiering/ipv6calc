/*
 * Project    : ipv6calc
 * File       : ipv6loganonhelp.c
 * Version    : $Id: ipv6loganonhelp.c,v 1.1 2007/01/30 17:00:37 peter Exp $
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
	fprintf(stderr, "This program anonymize IPv4/IPv6 addresses in HTTP server log files\n");
	fprintf(stderr, "See '%s -?' for more details\n\n", PROGRAM_NAME);
};

/* print global help */
void ipv6loganon_printhelp(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "\n");
	fprintf(stderr, " General:\n");
	fprintf(stderr, "  [-d|--debug <debug value>] : debug value (bitwise like)\n");
	fprintf(stderr, "  [-q|--quiet] : be more quiet\n");
	fprintf(stderr, "  [-n|--nocache] : disable caching\n");
	fprintf(stderr, "  [-c|--cachelimit <value>] : set cache limit\n");
	fprintf(stderr, "                               default: %d\n", cache_lru_limit);
	fprintf(stderr, "                               maximum: %d\n", CACHE_LRU_SIZE);
	fprintf(stderr, " Output anonymization:\n");
	fprintf(stderr, "  --mask-ipv4 <bits>     : mask IPv4 address [0-32], default 8\n");
	fprintf(stderr, "  --mask-ipv6-net <bits> : mask IPv6 network [0-32], default 16\n");
	fprintf(stderr, "  --mask-eui-id          : mask ID in 48/64 bit EUI-ID\n");
	fprintf(stderr, "  --anonymize-standard   : preset for standard anonymization\n");
	fprintf(stderr, "                            mask-ipv4=8 mask-ipv6-net=16 mask-eui-id\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " Takes data from stdin, proceed it to stdout\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\n");

	return;
};

