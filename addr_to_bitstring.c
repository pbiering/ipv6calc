/*
 * Project    : ipv6calc
 * File       : addr_to_bitstring.c
 * Version    : $Id: addr_to_bitstring.c,v 1.1 2002/02/19 21:41:17 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb@bieringer.de>
 *
 * Information:
 *  Function to format a given address to bitstring labels
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"

void addr_to_bitstring_printhelp() {
	fprintf(stderr, " %s --addr_to_bitstring|-b [--uppercase|--lowercase] ipv6addr[/prefixlength]\n", PROGRAM_NAME);
}; 

void addr_to_bitstring_printhelplong() {
	addr_to_bitstring_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to a bitstring label for use with DNS\n");
/*	fprintf(stderr, "   e.g. 3ffe:400:100:f101::1\n    -> 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int\n");
	fprintf(stderr, "   e.g. 3ffe:400:100:f101::1/64 -> 1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int\n\n");
	fprintf(stderr, "  or if ip6.arpa was selected:\n");
	fprintf(stderr, "   e.g. 3ffe:400:100:f101::1/64 -> 1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.arpa\n\n");*/
};

/* function formats an given IPv6 address to the reverse nibble format used by DNS zone files
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "addr_to_bitstring"
int addr_to_bitstring(char *addrstring, char *resultstring, long int command) {
	int retval = 1, result;
	ipv6calc_ipv6addr ipv6addr;

	if ( ipv6calc_debug & DEBUG_addr_to_bitstring ) {
		fprintf(stderr, "%s: Got input %s\n", DEBUG_function_name, addrstring);
	};

	result = addr_to_ipv6addrstruct(addrstring, resultstring, &ipv6addr);
	
	if ( ipv6calc_debug & DEBUG_addr_to_bitstring ) {
		fprintf(stderr, "%s: result of 'addr_to_ipv6addrstruct': %d\n", DEBUG_function_name, result);
	};
	if ( result != 0 ) {
		retval = 1;
		return (retval);
	};

	result = librfc2874_addr_to_bitstring(&ipv6addr, resultstring, command);
	if ( result != 0 ) {
		retval = 1;
		return (retval);
	};
		
	if ( ipv6calc_debug & DEBUG_addr_to_bitstring ) {
		fprintf(stderr, "%s: Print out: %s\n", DEBUG_function_name, resultstring);
	};

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name
