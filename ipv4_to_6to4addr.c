/*
 * Project    : ipv6calc
 * File       : ipv4_to_6to4addr.c
 * Version    : $Id: ipv4_to_6to4addr.c,v 1.1 2002/02/27 23:07:14 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  A function to convert a given IPv4 address to IPv6to4 address (RFC 3056)
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "libipv6calc.h"
#include "libipv4addr.h"
#include "librfc3056.h"
#include "librfc1884.h"
#include "ipv4_to_6to4addr.h"

void ipv4_to_6to4addr_printhelp(void) {
	fprintf(stderr, " %s --ipv4_to_6to4addr <IPv4 address>\n", PROGRAM_NAME);
};

void ipv4_to_6to4addr_printhelplong(void) {
	ipv4_to_6to4addr_printhelp();
	fprintf(stderr, "  Converts a IPv4 address to a IPv6to4 one (RFC 3056), e.g.\n");
	fprintf(stderr, "   11.12.13.14 -> 2002:0b0c:0d0e::\n");
};

/* Function converts a IPv4 address to a IPv6to4 one
 *  
 * in : *addrstring = IPv4 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "ipv4_to_6to4addr"
int ipv4_to_6to4addr(char *addrstring, char *resultstring) {
	int retval = 1, result;
	ipv6calc_ipv6addr ipv6addr;
	ipv6calc_ipv4addr ipv4addr;

	if ( ipv6calc_debug & DEBUG_ipv4_to_6to4addr) {
		fprintf(stderr, "%s: Got input %s\n", DEBUG_function_name, addrstring);
	};

	result = addr_to_ipv4addrstruct(addrstring, resultstring, &ipv4addr);

	if ( ipv6calc_debug & DEBUG_ipv4_to_6to4addr) {
		fprintf(stderr, "%s: result of 'addr_to_ipv4addrstruct': %d\n", DEBUG_function_name,result);
	};

	if ( result != 0 ) {
		retval = 1;
		return (retval);
	};

	ipv6addr_clearall(&ipv6addr);
	
	result = ipv4addr_to_ipv6to4addr(&ipv6addr, &ipv4addr);
	if ( result != 0 ) {
		sprintf(resultstring, "%s: unexpected result from 'ipv4addr_to_ipv6to4addr'", DEBUG_function_name);
		retval = 1;
		return (retval);
	};

	result = ipv6addrstruct_to_compaddr(&ipv6addr, resultstring);	
   
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name
