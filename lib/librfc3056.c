/*
 * Project    : ipv6calc
 * File       : librfc3056.c
 * Version    : $Id: librfc3056.c,v 1.8 2011/09/16 18:05:13 peter Exp $
 * Copyright  : 2001-2003 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library for conversions defined in RFC 3056
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libipv6calcdebug.h"
#include "libipv6calc.h"
#include "librfc3056.h"


/*
 * function converts an IPv4 address to an IPv6to4 one (RFC 3056)
 *  
 * in : *ipv4addrp = IPv4 address
 * out: *ipv6addrp = IPv6 address
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "librfc3056/ipv4addr_to_ipv6to4addr"
int librfc3056_ipv4addr_to_ipv6to4addr(ipv6calc_ipv6addr *ipv6addrp, const ipv6calc_ipv4addr *ipv4addrp) {
	int retval = 1;
	unsigned int i;

	/* clear IPv6 structure */
	ipv6addr_clear(ipv6addrp);

	/* set prefix */
	ipv6addr_setword(ipv6addrp, 0, 0x2002);

	/* set IPv4 address */
	for (i = 0; i <=3; i++) {
		ipv6addr_setoctet(ipv6addrp, i + 2, (unsigned int) ipv4addr_getoctet(ipv4addrp, i));
	};

	ipv6addrp->prefixlength = 48;
	ipv6addrp->flag_prefixuse = 0;
	ipv6addrp->flag_valid = 1;
	ipv6addrp->scope = ipv6addr_gettype(ipv6addrp);

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * function converts an IPv6 address to an IPv4 one (reverse RFC 3056)
 *  
 * in : *ipv6addrp = IPv6 address
 * out: *ipv4addrp = IPv4 address
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "librfc3056/ipv6addr_to_ipv4addr"
int librfc3056_ipv6addr_to_ipv4addr(ipv6calc_ipv4addr *ipv4addrp, const ipv6calc_ipv6addr *ipv6addrp, char* resultstring) {
	int retval = 1;
	unsigned int i;

	/* check scope */
	if ( (ipv6addrp->scope & IPV6_NEW_ADDR_6TO4) == 0 ) {
		snprintf(resultstring, NI_MAXHOST - 1, "IPv6 address is not a 6to4 one!");
		return(retval);	
	};

	/* clear IPv4 structure */
	ipv4addr_clear(ipv4addrp);

	/* set IPv4 address */
	for (i = 0; i < 4; i++) {
		ipv4addr_setoctet(ipv4addrp, i, (unsigned int) ipv6addr_getoctet(ipv6addrp, i + 2));
	};

	ipv4addrp->prefixlength = 32;
	ipv4addrp->flag_prefixuse = 0;
	ipv4addrp->flag_valid = 1;
	ipv4addrp->scope = ipv4addr_gettype(ipv4addrp);

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name
