/*
 * addr_to_compressed:
 * Support a function to format a given address to the compressed one
 * 	(RFC 1884)
 *
 * Version:		$Id: addr_to_compressed.c,v 1.1.1.1 2001/10/07 14:47:40 peter Exp $
 * 
 * Author:		Peter Bieringer <pb@bieringer.de>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "librfc1884.h"

void addr_to_compressed_printhelp() {
	fprintf(stderr, " %s --addr_to_compressed ipv6addr[/prefixlength]\n", PROGRAM_NAME);
};

void addr_to_compressed_printhelplong() {
	addr_to_compressed_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to a compressed one\n");
	fprintf(stderr, "   e.g. 3ffe:0400:0100:f101:0000:0000:0000:0001 -> 3ffe:400:100:f101::1\n\n");
};
                             
/* function IPv6addr to compressed format
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int addr_to_compressed(char *addrstring, char *resultstring) {
	int retval = 1, result;
	ipv6calc_ipv6addr ipv6addr;

	result = addr_to_ipv6addrstruct(addrstring, resultstring, &ipv6addr);
#ifdef DEBUG_addr_to_compressed
	fprintf(stderr, "addr_to_compressed: result of 'addr_to_ipv6addrstruct': %d\n", result);
#endif
		 
	if ( result != 0 ) {
		retval = 1;
		return (retval);
	};

	result = ipv6addrstruct_to_compaddr(&ipv6addr, resultstring); 
	retval = 0;
	return (retval);
};

