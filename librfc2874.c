/*
 * Project    : ipv6calc
 * File       : librfc2874.c
 * Version    : $Id: librfc2874.c,v 1.1 2002/02/19 21:41:18 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb@bieringer.de>
 *
 * Information:
 *  RFC 2874 conform addresses (A6/DNAME) bitstring labels
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ipv6calc.h"
#include "libipv6addr.h"


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
	int noctett, nbit, nnibble;
	char tempstring[NI_MAXHOST];

	if ( ipv6calc_debug & DEBUG_librfc2874 ) {
		result = ipv6addrstruct_to_uncompaddr(ipv6addrp, tempstring);
		fprintf(stderr, "%s: got address '%s'\n",  DEBUG_function_name, tempstring);
	};

	/* mask bits if selected */
	if ( (*ipv6addrp).flag_prefixuse == 1 ) {
		if (command & CMD_printsuffix) {
			ipv6addrstruct_masksuffix(ipv6addrp);
		} else {
			ipv6addrstruct_maskprefix(ipv6addrp);
		};
	};

	/* print out hex string format */
	/* 127 is lowest bit, 0 is highest bit */
	sprintf(resultstring, "%s", "");
	for (nbit = 0; nbit <= 127; nbit = nbit + 4) {
		if ( (*ipv6addrp).flag_prefixuse == 1 ) {
			if (command & CMD_printsuffix) {
				/* must test for prefix length match */
				if ( (nbit + 3) < (*ipv6addrp).prefixlength ) {
					/* skip nibble */
					continue;
		   		};
			} else {
				/* must test for prefix length match */
				if ( (nbit + 1) > (*ipv6addrp).prefixlength ) {
					/* skip nibble */
					continue;
		   		};
			};
		};
		
		/* calculate octett (8 bit) */
		noctett = (nbit & 0x78) >> 3;
		
		/* calculate nibble */
		nnibble = (nbit & 0x04) >> 2;

		/* extract nibble */
		nibble = ( (*ipv6addrp).in6_addr.s6_addr[noctett] & ( 0xf << (4 * (1 - nnibble)) ) ) >> ( 4 * (1 - nnibble));
		
		if ( ipv6calc_debug & DEBUG_addr_to_bitstring ) {
			fprintf(stderr, "%s: bit: %d = noctett: %d, nnibble: %d, octett: %02x, value: %x\n", DEBUG_function_name, nbit, noctett, nnibble, (*ipv6addrp).in6_addr.s6_addr[noctett], nibble);
		};
		if (command & CMD_printuppercase) {
			sprintf(tempstring, "%s%X", resultstring, nibble);
		} else {
			sprintf(tempstring, "%s%x", resultstring, nibble);
		};
		sprintf(resultstring, "%s", tempstring);
	};

	/* add begin and end of label */
	if ( (*ipv6addrp).flag_prefixuse == 1 ) {
		if (command & CMD_printsuffix) {
			sprintf(tempstring, "\\[x%s/%d]", resultstring, 128 - (*ipv6addrp).prefixlength);
		} else {
			sprintf(tempstring, "\\[x%s/%d]", resultstring, (*ipv6addrp).prefixlength);
		};
	};

	if (command & CMD_printuppercase) {
		sprintf(resultstring, "%s.IP6.ARPA", tempstring);
	} else {
		sprintf(resultstring, "%s.ip6.arpa", tempstring);
	};

	return (retval);
};
#undef DEBUG_function_name
