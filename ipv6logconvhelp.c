/*
 * Project    : ipv6calc
 * File       : ipv6logconvhelp.c
 * Version    : $Id: ipv6logconvhelp.c,v 1.1 2002/03/16 23:37:36 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Help library
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "ipv6calctypes.h"
#include "ipv6calcoptions.h"
#include "ipv6calchelp.h"

#define PROGRAM_NAME_ipv6logconv	"ipv6logconv"

/* display info */
void ipv6logconv_printversion(void) {
	fprintf(stderr, "\n");
	fprintf(stderr, "%s: version %s\n", PROGRAM_NAME_ipv6logconv, PROGRAM_VERSION);
};

void ipv6logconv_printinfo(void)  {
	ipv6logconv_printversion();
	printcopyright();
	fprintf(stderr, "This program converts IPv4/IPv6 addresses in HTTP server log files\n");
	fprintf(stderr, "See '%s -?' for more details\n\n", PROGRAM_NAME_ipv6logconv);
};

/* print global help */
void ipv6logconv_printhelp(void) {
	ipv6logconv_printversion();
	printcopyright();
	fprintf(stderr, "\n");
	fprintf(stderr, " General:\n");
	fprintf(stderr, "  [-d|--debug <debug value>] : debug value (bitwise like)\n");
	fprintf(stderr, " Output:\n");
	fprintf(stderr, "  [--out <output type>] : specify output type\n");
	fprintf(stderr, "  Available output types:  --out -?\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " Takes data from stdin, proceed it to stdout\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\n");

	return;
};

