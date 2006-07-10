/*
 * Project    : ipv6calc/ipv6logstats
 * File       : ipv6logstatshelp.c
 * Version    : $Id: ipv6logstatshelp.c,v 1.3 2006/07/10 11:38:19 peter Exp $
 * Copyright  : 2003-2006 by Peter Bieringer <pb (at) bieringer.de>
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

/* display info */
void printversion(void) {
	fprintf(stderr, "%s: version %s\n", PROGRAM_NAME, PACKAGE_VERSION);
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
	fprintf(stderr, "  [-c|--colums]              : print statistics in colums\n");
	fprintf(stderr, "  [-n|--noheader]            : don't print header in colums mode\n");
	fprintf(stderr, "  [-o|--onlyheader]          : print only header in colums mode\n");
	fprintf(stderr, "  [-p|--prefix <token>]      : print token as prefix\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " Takes http log data from stdin and print a table on output\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\n");

	return;
};

