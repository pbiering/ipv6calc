/*
 * Project    : ipv6calc
 * File       : addr_to_bitstring.c
 * Version    : $Id: addr_to_bitstring.c,v 1.4 2002/02/25 21:40:18 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb@bieringer.de>
 *
 * Information:
 *  Function to format a given address to a bitstring label
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "libhelp.h"
#include "librfc2874.h"
#include "addr_to_bitstring.h"

void addr_to_bitstring_printhelp(void) {
	fprintf(stderr, " %s --addr_to_bitstring|-b [<additional options>] <ipv6addr>[/<prefixlength>]\n", PROGRAM_NAME);
}; 

void addr_to_bitstring_printhelplong(void) {
	addr_to_bitstring_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to a bitstring label for use with DNS, e.g.\n");
	fprintf(stderr, "   3ffe:ffff::1    -> \\[x3ffeffff000000000000000000000001/128].ip6.arpa.\n");
	fprintf(stderr, "   3ffe:ffff::1/64 -> \\[x3ffeffff000000000000000000000001/64].ip6.arpa.\n");
	fprintf(stderr, "   --printsuffix 3ffe:ffff::1/64 -> \\[x0000000000000001/64]\n");
	fprintf(stderr, "   --printprefix 3ffe:ffff::1/64 -> \\[x3ffeffff00000000/64]\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  Additional usable options:\n");
	printhelp_print();
	printhelp_printstartend();
	printhelp_mask();
	printhelp_case();
	fprintf(stderr, "\n");
};

/*
 * function formats an given IPv6 address as bitstring label
 *
 * in : *ipv6addrp = pointer to IPv6 address structure, command
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "addr_to_bitstring"
int addr_to_bitstring(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, long int command) {
	int retval = 1, result;

	/* get native bitstring */
	result = librfc2874_addr_to_bitstring(ipv6addrp, resultstring, command);
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
