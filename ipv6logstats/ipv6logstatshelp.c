/*
 * Project    : ipv6calc/ipv6logstats
 * File       : ipv6logstatshelp.c
 * Version    : $Id: ipv6logstatshelp.c,v 1.1 2003/06/15 12:17:49 peter Exp $
 * Copyright  : 2003 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Help library
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipv6logstats.h"
#include "version.h"
#include "ipv6calctypes.h"
#include "ipv6calccommands.h"
#include "ipv6calchelp.h"

/* display info */
void printversion(void) {
	fprintf(stderr, "%s: version %s\n", PROGRAM_NAME, PROGRAM_VERSION);
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
	fprintf(stderr, " Takes data from stdin, proceed it to stdout\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\n");

	return;
};

