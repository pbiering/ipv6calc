/*
 * Project    : ipv6calc
 * File       : addr_to_ip6int.c
 * Version    : $Id: addr_to_ip6int.c,v 1.4 2002/02/25 21:18:50 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function to format a given address to reverse nibble-by-nibble ip6.int|arpa format
 *
 * Intention from the Perl program "ip6_int" written by Keith Owens <kaos at ocs dot com dot au>
 * some hints taken from ifconfig.c (net-tools)
 * 
 * Credits to:
 *  Keith Owens <kaos at ocs dot com dot au>
 *	net-tools authors
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "libipv6calc.h"
#include "libhelp.h"
#include "addr_to_ip6int.h"

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

/* function formats an given IPv6 address to the reverse nibble format used by DNS zone files
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "addr_to_ip6int"
int addr_to_ip6int(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, long int command) {
	int retval = 1;
	unsigned int nibble;
	int noctett, nbit, nnibble, bit_start, bit_end;
	char tempstring[NI_MAXHOST];

	if ( !(command & (CMD_printprefix | CMD_printsuffix | CMD_printstart | CMD_printend)) && (*ipv6addrp).flag_prefixuse ) {
		/* simulate old behavior */
		bit_start = 1;
		bit_end = (*ipv6addrp).prefixlength;
	} else if ( (*ipv6addrp).flag_startend_use != 0 ) {
		/* check start and end */
		if ( ((*ipv6addrp).bit_start - 1) & 0x03 ) {
			sprintf(resultstring, "Start bit number '%d' not dividable by 4 aren't supported because of non unique representation", ((*ipv6addrp).bit_start));
			retval = 1;
			return (retval);
		};
		if ( (*ipv6addrp).bit_end & 0x03 ) {
			sprintf(resultstring, "End bit number '%d' not dividable by 4 aren't supported because of non unique representation", (*ipv6addrp).bit_end);
			retval = 1;
			return (retval);
		};

		bit_start = (*ipv6addrp).bit_start;
		bit_end = (*ipv6addrp).bit_end;
	} else {
		bit_start = 1;
		bit_end = 128;
	};

	/* print out nibble format */
	/* 127 is lowest bit, 0 is highest bit */
	sprintf(resultstring, "%s", "");
	for (nbit = bit_end - 1; nbit >= bit_start - 1; nbit = nbit - 4) {
		/* calculate octett (8 bit) */
		noctett = (nbit & 0x78) >> 3;
		
		/* calculate nibble */
		nnibble = (nbit & 0x04) >> 2;

		/* extract nibble */
		nibble = ( (*ipv6addrp).in6_addr.s6_addr[noctett] & ( 0xf << (4 * (1 - nnibble)) ) ) >> ( 4 * (1 - nnibble));
		
		if ( ipv6calc_debug & DEBUG_addr_to_ip6int ) {
			fprintf(stderr, "%s: bit: %d = noctett: %d, nnibble: %d, octett: %02x, value: %x\n", DEBUG_function_name, nbit, noctett, nnibble, (*ipv6addrp).in6_addr.s6_addr[noctett], nibble);
		};

		sprintf(tempstring, "%s%x", resultstring, nibble);
		if (nbit < bit_start) {
			sprintf(resultstring, "%s", tempstring);
		} else {
			sprintf(resultstring, "%s.", tempstring);
		};
	};

	if (bit_start == 1) {
		switch(command & CMD_MAJOR_MASK) {
		    case CMD_addr_to_ip6int:
			sprintf(tempstring, "%sip6.int.", resultstring);
			break;
		    case CMD_addr_to_ip6arpa:
			sprintf(tempstring, "%sip6.arpa.", resultstring);
			break;
		};
	};

	sprintf(resultstring, "%s", tempstring);

	if (command & CMD_printuppercase) {
		string_to_upcase(resultstring);
	};
		
	if ( ipv6calc_debug & DEBUG_addr_to_ip6int ) {
		fprintf(stderr, "%s: Print out: %s\n", DEBUG_function_name, resultstring);
	};

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name
