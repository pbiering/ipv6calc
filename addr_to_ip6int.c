/*
 * Project    : ipv6calc
 * File       : addr_to_ip6int.c
 * Version    : $Id: addr_to_ip6int.c,v 1.3 2002/02/23 11:07:44 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb@bieringer.de>
 *
 * Information:
 *  Function to format a given address to reverse nibble-by-nibble ip6.int|arpa format
 *
 * Intention from the Perl program "ip6_int" written by Keith Owens <kaos@ocs.com.au>
 * some hints taken from ifconfig.c (net-tools)
 * 
 * Credits to:
 *  Keith Owens <kaos at ocs dot com dot au>
 *	net-tools authors
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"

void addr_to_ip6int_printhelp() {
	fprintf(stderr, " %s --addr_to_ip6int|-r|--addr_to_ip6arpa|-a [--uppercase|--lowercase] ipv6addr[/prefixlength]\n", PROGRAM_NAME);
}; 

void addr_to_ip6int_printhelplong() {
	addr_to_ip6int_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to a dot separated reverse nibble format for use with DNS, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1\n    -> 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int.\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1/64 -> 1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int.\n\n");
	fprintf(stderr, "  or if ip6.arpa was selected:\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1/64 -> 1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.arpa.\n\n");
};

/* function formats an given IPv6 address to the reverse nibble format used by DNS zone files
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "addr_to_ip6int"
int addr_to_ip6int(char *addrstring, char *resultstring, long int command) {
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
		if (command & CMD_printuppercase) {
			sprintf(tempstring, "%s%X.", resultstring, nibble);
		} else {
			sprintf(tempstring, "%s%x.", resultstring, nibble);
		};
		sprintf(resultstring, "%s", tempstring);
	};

	switch(command & CMD_MAJOR_MASK) {
	    case CMD_addr_to_ip6int:
		if (command & CMD_printuppercase) {
			sprintf(tempstring, "%sIP6.INT.", resultstring);
		} else {
			sprintf(tempstring, "%sip6.int.", resultstring);
		};
		break;
	    case CMD_addr_to_ip6arpa:
		if (command & CMD_printuppercase) {
			sprintf(tempstring, "%sIP6.ARPA.", resultstring);
		} else {
			sprintf(tempstring, "%sip6.arpa.", resultstring);
		};
		break;
	    default:
		sprintf(resultstring, "%s: strange error occurs during selection of 'ip6.int' or 'ip6.arpa'\n", DEBUG_function_name);
		retval = 1;
		return (retval);
	};

	sprintf(resultstring, "%s", tempstring);
		
	if ( ipv6calc_debug & DEBUG_addr_to_ip6int ) {
		fprintf(stderr, "%s: Print out: %s\n", DEBUG_function_name, resultstring);
	};

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name
