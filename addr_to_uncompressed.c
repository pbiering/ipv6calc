/*
 * addr_to_uncompressed:
 *  Support a function to format a given address to an
 *   uncompressed one (reverse RFC 1884)
 *
 * Version:		$Id: addr_to_uncompressed.c,v 1.1.1.1 2001/10/07 14:47:40 peter Exp $
 * 
 * Author:		Peter Bieringer <pb@bieringer.de>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"

void addr_to_uncompressed_printhelp() {
	fprintf(stderr, " %s --addr_to_uncompressed [--maskprefix|--masksuffix] [--printprefix|printsuffix] ipv6addr[/prefixlength]\n", PROGRAM_NAME);
};

void addr_to_uncompressed_printhelplong() {
	addr_to_uncompressed_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to an uncompressed one\n");
	fprintf(stderr, "   e.g. 3ffe:400:100:f101::1 -> 3ffe:400:100:f101:0:0:0:1\n");
	fprintf(stderr, "   e.g. 3ffe:400:100:f101::1/64 -> 3ffe:400:100:f101:0:0:0:1/64\n\n");
	fprintf(stderr, "  Also it's possible to mask prefix or suffix bits or\n");
	fprintf(stderr, "   print prefix or suffix only\n");
};

/* function formats an given IPv6 address to an uncompressed format
 *  not touching prefix length
 *
 * in : *addrstring = IPv6 address, unsigned int formatselector
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int addr_to_uncompressed(char *addrstring, char *resultstring, unsigned int formatselector) {
	int retval = 1, result;
	ipv6calc_ipv6addr ipv6addr;

#ifdef DEBUG_addr_to_uncompressed
	fprintf(stderr, "addr_to_uncompressed(: Got input %s\n", addrstring);
#endif

	result = addr_to_ipv6addrstruct(addrstring, resultstring, &ipv6addr);
#ifdef DEBUG_addr_to_uncompressed
	fprintf(stderr, "addr_to_uncompressed: result of 'addr_to_ipv6addrstruct': %d\n", result);
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

	if (formatselector & CMD_printprefix) { 	
		result = ipv6addrstruct_to_uncompaddrprefix(&ipv6addr, resultstring);
	} else if (formatselector & CMD_printsuffix) {
		result = ipv6addrstruct_to_uncompaddrsuffix(&ipv6addr, resultstring);
	} else {
		result = ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring);
	};
   
	retval = 0;
	return (retval);
};
