/*
 * Project    : ipv6calc
 * File       : addr_to_bitstring.c
 * Version    : $Id: addr_to_bitstring.c,v 1.2 2002/02/23 11:07:44 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb@bieringer.de>
 *
 * Information:
 *  Function to format a given address to bitstring labels
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"

void addr_to_bitstring_printhelp() {
	fprintf(stderr, " %s --addr_to_bitstring|-b [--printsuffix] [--uppercase|--lowercase] ipv6addr[/prefixlength]\n", PROGRAM_NAME);
}; 

void addr_to_bitstring_printhelplong() {
	addr_to_bitstring_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to a bitstring label for use with DNS, e.g.\n");
	fprintf(stderr, "   3ffe:ffff::1    -> \\[x3ffeffff000000000000000000000001/128].ip6.arpa.\n");
	fprintf(stderr, "   3ffe:ffff::1/64 -> \\[x3ffeffff00000000/64].ip6.arpa.\n");
	fprintf(stderr, "   --printsuffix 3ffe:ffff::1/64 -> \\[x0000000000000001/64]\n\n");
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

	/* get native bitstring */
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
