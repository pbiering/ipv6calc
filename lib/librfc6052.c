/*
 * Project    : ipv6calc
 * File       : librfc6052.c
 * Version    : $Id: librfc6052.c,v 1.1 2015/08/23 09:53:27 ds6peter Exp $
 * Copyright  : 2015-2015 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library for conversions defined in RFC 6052
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libipv6calcdebug.h"
#include "libipv6calc.h"
#include "librfc6052.h"


/*
 * function converts an IPv4 address to an NAT64 IPv6 address (RFC 6052 / 2.1)
 *  
 * in : *ipv4addrp = IPv4 address
 * out: *ipv6addrp = IPv6 address
 * ret: ==0: ok, !=0: error
 */
int librfc6052_ipv4addr_to_ipv6addr(ipv6calc_ipv6addr *ipv6addrp, const ipv6calc_ipv4addr *ipv4addrp) {
	int retval = 1;
	unsigned int i;

	/* clear IPv6 structure */
	ipv6addr_clear(ipv6addrp);

	/* set prefix */
	ipv6addr_setdword(ipv6addrp, 0, 0x0064ff9b);

	/* set IPv4 address */
	for (i = 0; i <=3; i++) {
		ipv6addr_setoctet(ipv6addrp, i + 12, (unsigned int) ipv4addr_getoctet(ipv4addrp, i));
	};

	ipv6addrp->prefixlength = 96;
	ipv6addrp->flag_prefixuse = 0;
	ipv6addrp->flag_valid = 1;
	ipv6addrp->scope = ipv6addr_gettype(ipv6addrp);

	retval = 0;
	return (retval);
};


/*
 * function extracts from a NAT64 address the IPv4 address (reverse RFC 6052 / 2.1)
 *  
 * in : *ipv6addrp = IPv6 address
 * out: *ipv4addrp = IPv4 address
 * ret: ==0: ok, !=0: error
 */
int librfc6052_ipv6addr_to_ipv4addr(ipv6calc_ipv4addr *ipv4addrp, const ipv6calc_ipv6addr *ipv6addrp, char* resultstring, const size_t resultstring_length) {
	int retval = 1;
	unsigned int i;

	/* check scope */
	if ( (ipv6addrp->scope & IPV6_NEW_ADDR_NAT64) == 0 ) {
		snprintf(resultstring, resultstring_length, "IPv6 address is not a NAT64 one!");
		return(retval);	
	};

	/* clear IPv4 structure */
	ipv4addr_clear(ipv4addrp);

	/* set IPv4 address */
	for (i = 0; i < 4; i++) {
		ipv4addr_setoctet(ipv4addrp, i, (unsigned int) ipv6addr_getoctet(ipv6addrp, i + 12));
	};

	ipv4addrp->prefixlength = 32;
	ipv4addrp->flag_prefixuse = 0;
	ipv4addrp->flag_valid = 1;
	ipv4addrp->scope = ipv4addr_gettype(ipv4addrp);

	retval = 0;
	return (retval);
};
