/*
 * Project    : ipv6calc
 * File       : ipv6logconvhelp.c
 * Version    : $Id: ipv6logconvhelp.c,v 1.4 2003/11/21 09:36:58 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Help library
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipv6logconv.h"
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
	fprintf(stderr, " Output:\n");
	fprintf(stderr, "  [--out <output type>] : specify output type\n");
	fprintf(stderr, "  Available output types:  --out -?\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " Takes data from stdin, proceed it to stdout\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\n");

	return;
};

