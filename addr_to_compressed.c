/*
 * Project    : ipv6calc
 * File       : addr_to_compressed.c
 * Version    : $Id: addr_to_compressed.c,v 1.4 2002/02/25 21:40:18 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Support a function to format a given address to the compressed one (RFC 1884)
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "libipv6calc.h"
#include "librfc1884.h"
#include "libhelp.h"
#include "addr_to_compressed.h"

void addr_to_compressed_printhelp(void) {
	fprintf(stderr, " %s --addr_to_compressed [<additional options>] <ipv6addr>[/<prefixlength>]\n", PROGRAM_NAME);
};

void addr_to_compressed_printhelplong(void) {
	addr_to_compressed_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to a compressed one, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:0100:f101:0000:0000:0000:0001 -> 3ffe:ffff:100:f101::1\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  Additional usable options:\n");
	printhelp_mask();
	printhelp_case();
	fprintf(stderr, "\n");
};
                             
/*
 * function IPv6addr to compressed format
 *
 * in : *ipv6addrp = pointer to IPv6 address structure, command
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int addr_to_compressed(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, unsigned long command) {
	int retval = 1;

	retval = ipv6addrstruct_to_compaddr(ipv6addrp, resultstring); 

	if ((command & CMD_printuppercase) && (retval == 0)) {
		string_to_upcase(resultstring);
	};

	return (retval);
};
