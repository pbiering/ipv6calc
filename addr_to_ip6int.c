/*
 * addr_to_ip6int:
 *  Function to format a given address to reverse ip6.int format
 *
 * Version:		$Id: addr_to_ip6int.c,v 1.1 2001/10/07 14:47:40 peter Exp $
 * 
 * Author:		Peter Bieringer <pb@bieringer.de>
 *
 * intention from the Perl program "ip6_int" written by Keith Owens <kaos@ocs.com.au>
 * some hints taken from ifconfig.c (net-tools)
 * 
 * Credits to:
 *  Keith Owens <kaos@ocs.com.au>
 *	net-tools authors
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"

void addr_to_ip6int_printhelp() {
	fprintf(stderr, " %s --addr_to_ip6int|-r  ipv6addr[/prefixlength]\n", PROGRAM_NAME);
}; 

void addr_to_ip6int_printhelplong() {
	addr_to_ip6int_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to a dot separated reverse nibble format for use with DNS\n");
	fprintf(stderr, "   e.g. 3ffe:400:100:f101::1\n    -> 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int\n");
	fprintf(stderr, "   e.g. 3ffe:400:100:f101::1/64 -> 1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int\n\n");
};

/* function formats an given IPv6 address to the reverse nibble format used by DNS zone files
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "addr_to_ip6int"
int addr_to_ip6int(char *addrstring, char *resultstring) {
	int retval = 1, result;
	ipv6calc_ipv6addr ipv6addr;
	unsigned int nibble;
	int noctett, nbit, nnibble;
	char tempstring[NI_MAXHOST];

	if ( ipv6calc_debug & DEBUG_addr_to_ip6int ) {
		fprintf(stderr, "%s: Got input %s\n", DEBUG_function_name, addrstring);
	};

	result = addr_to_ipv6addrstruct(addrstring, resultstring, &ipv6addr);
	
	if ( ipv6calc_debug & DEBUG_addr_to_ip6int ) {
		fprintf(stderr, "%s: result of 'addr_to_ipv6addrstruct': %d\n", DEBUG_function_name, result);
	};
			 
	if ( result != 0 ) {
		retval = 1;
		return (retval);
	};
	
	/* print out nibble format */
	/* 127 is lowest bit, 0 is highest bit */
	sprintf(resultstring, "%s", "");
	for (nbit = 127; nbit >= 0; nbit = nbit - 4) {
		if ( ipv6addr.flag_prefixuse == 1 ) {
			/* must test for prefix length match */
			if ( (nbit + 1) > ipv6addr.prefixlength ) {
				/* skip nibble */
				continue;
		   	};
		};
		
		/* calculate octett (8 bit) */
		noctett = (nbit & 0x78) >> 3;
		
		/* calculate nibble */
		nnibble = (nbit & 0x04) >> 2;

		/* extract nibble */
		nibble = ( ipv6addr.in6_addr.s6_addr[noctett] & ( 0xf << (4 * (1 - nnibble)) ) ) >> ( 4 * (1 - nnibble));
		
		if ( ipv6calc_debug & DEBUG_addr_to_ip6int ) {
			fprintf(stderr, "%s: bit: %d = noctett: %d, nnibble: %d, octett: %02x, value: %x\n", DEBUG_function_name, nbit, noctett, nnibble, ipv6addr.in6_addr.s6_addr[noctett], nibble);
		};
		sprintf(tempstring, "%s%x.", resultstring, nibble);
		sprintf(resultstring, "%s", tempstring);
	};

	sprintf(tempstring, "%sip6.int", resultstring);
	sprintf(resultstring, "%s", tempstring);
		
	if ( ipv6calc_debug & DEBUG_addr_to_ip6int ) {
		fprintf(stderr, "%s: Print out: %s\n", DEBUG_function_name, resultstring);
	};

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name
