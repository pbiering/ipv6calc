/*
 * Project    : ipv6calc
 * File       : addr_to_ip6int.c
 * Version    : $Id: addr_to_ip6int.c,v 1.5 2002/02/25 21:40:18 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function to format a given address to reverse nibble-by-nibble ip6.int|arpa format
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "libipv6calc.h"
#include "libhelp.h"
#include "addr_to_ip6int.h"
#include "librfc1886.h"

void addr_to_ip6int_printhelp(void) {
	fprintf(stderr, " %s --addr_to_ip6int|-r|--addr_to_ip6arpa|-a [<additional options>] <ipv6addr>[/<prefixlength>]\n", PROGRAM_NAME);
}; 

void addr_to_ip6int_printhelplong(void) {
	addr_to_ip6int_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to a dot separated reverse nibble format for use with DNS, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1\n    -> 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int.\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1/64\n    -> 1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int.\n");
	fprintf(stderr, "  or if 'ip6.arpa' was selected:\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1/64 -> 1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.arpa.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  Additional usable options:\n");
	printhelp_print();
	printhelp_printstartend();
	printhelp_mask();
	printhelp_case();
	fprintf(stderr, "\n");
};

/*
 * function formats an given IPv6 address to the reverse nibble format used by DNS zone files
 *
 * in : *ipv6addrp = pointer to IPv6 address structure, command
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "addr_to_ip6int"
int addr_to_ip6int(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, long int command) {
	int retval = 1, result;

	/* get native reverse nibble string */
	result = librfc1886_addr_to_nibblestring(ipv6addrp, resultstring, command);
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
