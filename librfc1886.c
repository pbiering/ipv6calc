/*
 * Project    : ipv6calc
 * File       : librfc1886.c
 * Version    : $Id: librfc1886.c,v 1.4 2002/03/03 11:01:54 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  RFC 1886 conform reverse nibble format string
 *
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
#include <string.h>
#include <stdlib.h>

#include "ipv6calc.h"
#include "ipv6calctypes.h"
#include "libipv6addr.h"
#include "libipv6calc.h"
#include "librfc1886.h"


/*
 * function converts IPv6addr_structure to a reverse nibble format string
 *
 * in : *ipv6addrp = IPv6 address structure
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "librfc1886/addr_to_nibblestring"
int librfc1886_addr_to_nibblestring(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, unsigned long int formatoptions, char* domain) {
	int retval = 1;
	unsigned int nibble;
	int noctett, nbit, nnibble, bit_start, bit_end;
	char tempstring[NI_MAXHOST];
	
	if (ipv6calc_debug & DEBUG_librfc1886) {
		fprintf(stderr, "%s: flag_prefixuse %d\n", DEBUG_function_name, (*ipv6addrp).flag_prefixuse);
	};

	if ( !(formatoptions & (FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_printstart | FORMATOPTION_printend)) && (*ipv6addrp).flag_prefixuse ) {
		/* simulate old behavior */
		bit_start = 1;
		bit_end = (*ipv6addrp).prefixlength;
		if (ipv6calc_debug & DEBUG_librfc1886) {
			fprintf(stderr, "%s: simulate old behavior\n", DEBUG_function_name);
		};
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
	
	if (ipv6calc_debug & DEBUG_librfc1886) {
		fprintf(stderr, "%s: start bit %d  end bit %d\n", DEBUG_function_name, bit_start, bit_end);
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
		
		if ( ipv6calc_debug & DEBUG_librfc1886 ) {
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
		sprintf(tempstring, "%s%s", resultstring, domain);
	};

	sprintf(resultstring, "%s", tempstring);

	if (formatoptions & FORMATOPTION_printuppercase) {
		string_to_upcase(resultstring);
	};
		
	if ( ipv6calc_debug & DEBUG_librfc1886 ) {
		fprintf(stderr, "%s: Print out: %s\n", DEBUG_function_name, resultstring);
	};

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name
