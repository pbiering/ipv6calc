/*
 * Project    : ipv6calc
 * File       : librfc2874.c
 * Version    : $Id: librfc2874.c,v 1.3 2002/02/25 21:18:51 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  RFC 2874 conform addresses (A6/DNAME) bitstring labels
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ipv6calc.h"
#include "libipv6addr.h"
#include "libipv6calc.h"
#include "librfc2874.h"


/*
 * function converts IPv6addr_structure to a bitstring label
 *
 * in : *ipv6addrp = IPv6 address structure
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "librfc2874/addr_to_bitstring"
int librfc2874_addr_to_bitstring(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, long int command) {
	int retval = 1, result;
	unsigned int nibble;
	int noctett, nbit, nnibble, prefixlength, bit_start, bit_end;
	char tempstring[NI_MAXHOST];

	if ( ipv6calc_debug & DEBUG_librfc2874 ) {
		result = ipv6addrstruct_to_uncompaddr(ipv6addrp, tempstring);
		fprintf(stderr, "%s: got address '%s'\n",  DEBUG_function_name, tempstring);
	};

	if ( (*ipv6addrp).flag_startend_use != 0 ) {
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

	if ( ipv6calc_debug & DEBUG_addr_to_bitstring ) {
		fprintf(stderr, "%s: print from start bit to end bit: %d - %d\n", DEBUG_function_name, bit_start, bit_end);
	};

	/* print out hex string format */
	/* 127 is lowest bit, 0 is highest bit */
	sprintf(resultstring, "%s", "");
	for (nbit = bit_start - 1; nbit <= bit_end - 1; nbit = nbit + 4) {
		/* calculate octett (8 bit) */
		noctett = (nbit & 0x78) >> 3;
		
		/* calculate nibble */
		nnibble = (nbit & 0x04) >> 2;

		/* extract nibble */
		nibble = ( (*ipv6addrp).in6_addr.s6_addr[noctett] & ( 0xf << (4 * (1 - nnibble)) ) ) >> ( 4 * (1 - nnibble));
		
		if ( ipv6calc_debug & DEBUG_addr_to_bitstring ) {
			fprintf(stderr, "%s: bit: %d = noctett: %d, nnibble: %d, octett: %02x, value: %x\n", DEBUG_function_name, nbit, noctett, nnibble, (*ipv6addrp).in6_addr.s6_addr[noctett], nibble);
		};

		sprintf(tempstring, "%s%x", resultstring, nibble);
		sprintf(resultstring, "%s", tempstring);
	};

	/* add begin and end of label */
	if ((*ipv6addrp).flag_startend_use) {
		prefixlength = bit_end - bit_start + 1;
	} else {
		prefixlength = 128;
	};
	
	if ( bit_start != 1 ) {
		sprintf(tempstring, "%s/%d]", resultstring, prefixlength);
	} else {
		sprintf(tempstring, "%s/%d].ip6.arpa.", resultstring, prefixlength);
	};
	
	if ( command & CMD_printuppercase ) {
		string_to_upcase(tempstring);
	};

	sprintf(resultstring, "\\[x%s", tempstring);

	retval = 0;

	return (retval);
};
#undef DEBUG_function_name
