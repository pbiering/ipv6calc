/*
 * Project    : ipv6calc
 * File       : mac_to_eui64.c
 * Version    : $Id: mac_to_eui64.c,v 1.2 2002/02/25 21:18:51 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  A function to format a given MAC address to a EUI-64 identifier
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "libipv6calc.h"
#include "libmac.h"
#include "libeui64.h"
#include "mac_to_eui64.h"

void mac_to_eui64_printhelp() {
	fprintf(stderr, " %s --mac_to_eui64 <MAC_address>\n", PROGRAM_NAME);
};

void mac_to_eui64_printhelplong() {
	mac_to_eui64_printhelp();
	fprintf(stderr, "  Converts a MAC address to a EUI-64 host identifier\n");
	fprintf(stderr, "   e.g. 00:50:BF:06:B4:F5 -> 0250:bfff:fe06:b4f5\n");
};

/* Function converts a MAC address to a EUI-64 host identifier
 *  
 * in : *addrstring = MAC address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "mac_to_eui64"
int mac_to_eui64(char *addrstring, char *resultstring) {
	int retval = 1, result;
	ipv6calc_ipv6addr ipv6addr;
	ipv6calc_macaddr macaddr;

	if ( ipv6calc_debug & DEBUG_mac_to_eui64 ) {
		fprintf(stderr, "%s: Got input %s\n", DEBUG_function_name, addrstring);
	};

	result = mac_to_macaddrstruct(addrstring, resultstring, &macaddr);

	if ( ipv6calc_debug & DEBUG_mac_to_eui64 ) {
		fprintf(stderr, "%s: result of 'mac_to_ipv6addrstruct: %d\n", DEBUG_function_name, result);
	};

	if ( result != 0 ) {
		retval = 1;
		return (retval);
	};
		     
	result = create_eui64_from_mac(&ipv6addr, &macaddr);

	if ( result != 0 ) {
		retval = 1;
		return (retval);
	};

	result = ipv6addrstruct_to_uncompaddrsuffix(&ipv6addr, resultstring);
   
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name
