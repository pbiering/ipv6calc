/*
 * addr_to_fulluncompressed:
 *  Support a function to format a given address to a
 *   full uncompressed one (reverse RFC 1884)
 *
 * Version:		$Id: addr_to_fulluncompressed.c,v 1.1 2001/10/07 14:47:40 peter Exp $
 * 
 * Author:		Peter Bieringer <pb@bieringer.de>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"

void addr_to_fulluncompressed_printhelp() {
	fprintf(stderr, " %s --addr_to_fulluncompressed [--maskprefix|--masksuffix] ipv6addr[/prefixlength]\n", PROGRAM_NAME);
};

void addr_to_fulluncompressed_printhelplong() {
	addr_to_fulluncompressed_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to a full uncompressed one\n");
	fprintf(stderr, "   e.g. 3ffe:400:100:f101::1 -> 3ffe:0400:0100:f101:0000:0000:0000:0001\n\n");
};

/* function formats an given IPv6 address to a full uncompressed format
 *
 * in : *addrstring = IPv6 address, unsigned int formatselector
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int addr_to_fulluncompressed(char *addrstring, char *resultstring, unsigned int formatselector) {
	int retval = 1, result;
	ipv6calc_ipv6addr ipv6addr;

#ifdef DEBUG_addr_to_fulluncompressed
	fprintf(stderr, "addr_to_fulluncompressed: Got input %s\n", addrstring);
#endif

	result = addr_to_ipv6addrstruct(addrstring, resultstring, &ipv6addr);
#ifdef DEBUG_addr_to_fulluncompressed
	fprintf(stderr, "addr_to_fulluncompressed: result of 'addr_to_ipv6addrstruct': %d\n", result);
#endif

	if ( result != 0 ) {
		retval = 1;
		return (retval);
	}; 

	if (formatselector & CMD_maskprefix) {
		ipv6addrstruct_maskprefix(&ipv6addr);
	} else if (formatselector & CMD_masksuffix) {
		ipv6addrstruct_masksuffix(&ipv6addr);
	};
	result = ipv6addrstruct_to_fulluncompaddr(&ipv6addr, resultstring);
   
	retval = 0;
	return (retval);
};
