/*
 * Project    : ipv6calc
 * File       : addr_to_ifinet6.c
 * Version    : $Id: addr_to_ifinet6.c,v 1.2 2002/02/23 11:07:44 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb@bieringer.de>
 *
 * Information:
 *  Function to format a given address to a Linux /proc/net/if_inet6 format
 *
 * some hints taken from ifconfig.c (net-tools)
 */

#include <stdio.h>
#include <stdlib.h>
#include "ipv6calc.h"
#include "libipv6addr.h"

void addr_to_ifinet6_printhelp() {
	fprintf(stderr, " %s --addr_to_ifinet6 ipv6addr[/prefixlength]\n", PROGRAM_NAME);
};

void addr_to_ifinet6_printhelplong() {
	addr_to_ifinet6_printhelp();
	fprintf(stderr, "  Converts given IPv6 address to similar format shown in Linux /proc/net/if_inet6:\n    addr scope [prefixlength], e.g.\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1 -> 3ffeffff0100f1010000000000000001 00\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1/64 -> 3ffeffff0100f1010000000000000001 00 40\n\n");
}; 

/* function formats an given IPv6 address to Linux /proc/net/if_inet6 format
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int addr_to_ifinet6(char *addrstring, char *resultstring) {
	int retval = 1, result;
	ipv6calc_ipv6addr ipv6addr; 
	char tempstring[NI_MAXHOST];

	if (ipv6calc_debug & DEBUG_addr_to_ifinet6) {
		fprintf(stderr, "addr_to_ifinet6: got input: %s\n", addrstring);
	};

    result = addr_to_ipv6addrstruct(addrstring, resultstring, &ipv6addr);
	
	if (ipv6calc_debug & DEBUG_addr_to_ifinet6) {
    	fprintf(stderr, "addr_to_ifinet6: result of 'addr_to_ipv6addrstruct': %d\n", result);
	};
   
	if ( result != 0 ) {
		retval = 1;
		return (retval);
	};

	/* print out array */	
    sprintf(tempstring, "%08x%08x%08x%08x %02x",  ipv6addr_getdword(&ipv6addr, 0), ipv6addr_getdword(&ipv6addr, 1),  ipv6addr_getdword(&ipv6addr, 2),  ipv6addr_getdword(&ipv6addr, 3), ipv6addr.scope & IPV6_ADDR_SCOPE_MASK);
	
	if ( ipv6addr.flag_prefixuse == 1 ) {
	    sprintf(resultstring, "%s %02x", tempstring, ipv6addr.prefixlength);
	} else {
	    sprintf(resultstring, "%s", tempstring);
	};

	if (ipv6calc_debug & DEBUG_addr_to_ifinet6) {
		fprintf(stderr, "addr_to_ifinet6: Print out: %s\n", resultstring);
	};

	retval = 0;
	return (retval);
};
