/*
 * Project    : ipv6calc
 * File       : addr_to_fulluncompressed.c
 * Version    : $Id: addr_to_fulluncompressed.c,v 1.4 2002/02/25 21:40:18 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Support a function to format a given address to a
 *   full uncompressed one (reverse RFC 1884)
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "libhelp.h"
#include "libipv6calc.h"
#include "addr_to_fulluncompressed.h"

void addr_to_fulluncompressed_printhelp() {
	fprintf(stderr, " %s --addr_to_fulluncompressed [<additional options>] <ipv6addr>[/<prefixlength>]\n", PROGRAM_NAME);
};

void addr_to_fulluncompressed_printhelplong() {
	addr_to_fulluncompressed_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to a full uncompressed one, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1 -> 3ffe:ffff:0100:f101:0000:0000:0000:0001\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  Additional usable options:\n");
	printhelp_mask();
	printhelp_case();
	fprintf(stderr, "\n");
};

/*
 * function formats an given IPv6 address to a full uncompressed format
 *
 * in : *ipv6addrp = pointer to IPv6 address structure, unsigned int formatselector
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int addr_to_fulluncompressed(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, unsigned long command) {
	int retval = 1;

	retval = ipv6addrstruct_to_fulluncompaddr(ipv6addrp, resultstring);

	if ((command & CMD_printuppercase) && (retval == 0)) {
		string_to_upcase(resultstring);
	};
   
	return (retval);
};
