/*
 * Project    : ipv6calc
 * File       : addr_to_uncompressed.c
 * Version    : $Id: addr_to_uncompressed.c,v 1.4 2002/02/25 21:40:18 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb@bieringer.de>
 *
 * Information:
 *  Support a function to format a given address to an
 *   uncompressed one (reverse RFC 1884)
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "libhelp.h"
#include "addr_to_uncompressed.h"
#include "libipv6calc.h"

void addr_to_uncompressed_printhelp(void) {
	fprintf(stderr, " %s --addr_to_uncompressed [<additional options>] <ipv6addr>[/<prefixlength>]>\n", PROGRAM_NAME);
};

void addr_to_uncompressed_printhelplong(void) {
	addr_to_uncompressed_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to an uncompressed one, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1 -> 3ffe:ffff:100:f101:0:0:0:1\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1/64 -> 3ffe:ffff:100:f101:0:0:0:1/64\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  Additional usable options:\n");
	printhelp_mask();
	printhelp_case();
	fprintf(stderr, "\n");
};

/* function formats an given IPv6 address to an uncompressed format
 *  not touching prefix length
 *
 * in : *ipv6addrp = pointer to IPv6 address structure, command
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int addr_to_uncompressed(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, unsigned long command) {
	int retval = 1;

	if ( command & CMD_printprefix ) { 	
		retval = ipv6addrstruct_to_uncompaddrprefix(ipv6addrp, resultstring);
	} else if ( command & CMD_printsuffix ) {
		retval = ipv6addrstruct_to_uncompaddrsuffix(ipv6addrp, resultstring);
	} else {
		retval = ipv6addrstruct_to_uncompaddr(ipv6addrp, resultstring);
	};

	if ((command & CMD_printuppercase) && (retval == 0)) {
		string_to_upcase(resultstring);
	};
   
	return (retval);
};
