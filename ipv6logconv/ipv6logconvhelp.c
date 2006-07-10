/*
 * Project    : ipv6calc
 * File       : ipv6logconvhelp.c
 * Version    : $Id: ipv6logconvhelp.c,v 1.8 2006/07/10 11:38:19 peter Exp $
 * Copyright  : 2002-2005 by Peter Bieringer <pb (at) bieringer.de>
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
	fprintf(stderr, "%s: version %s\n", PROGRAM_NAME, PACKAGE_VERSION);
};

void printcopyright(void) {
        fprintf(stderr, "%s\n", PROGRAM_COPYRIGHT);
};

void ipv6logconv_printinfo(void)  {
	printversion();
	printcopyright();
	fprintf(stderr, "This program converts IPv4/IPv6 addresses in HTTP server log files\n");
	fprintf(stderr, "See '%s -?' for more details\n\n", PROGRAM_NAME);
};

/* print global help */
void ipv6logconv_printhelp(void) {
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
	fprintf(stderr, " Output:\n");
	fprintf(stderr, "  [--out <output type>] : specify output type\n");
	fprintf(stderr, "   addrtype       : Address type\n");
	fprintf(stderr, "   ouitype        : OUI (IEEE) type\n");
	fprintf(stderr, "   ipv6addrtype   : IPv6 address type\n");
	fprintf(stderr, "   any            : any type\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " Takes data from stdin, proceed it to stdout\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\n");

	return;
};

