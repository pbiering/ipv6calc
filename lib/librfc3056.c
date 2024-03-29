/*
 * Project    : ipv6calc
 * File       : librfc3056.c
 * Version    : $Id$
 * Copyright  : 2001-2021 by Peter Bieringer <pb (at) bieringer.de>
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

	ipv6addrp->flag_prefixuse = ipv4addrp->flag_prefixuse;
	if ((ipv4addrp->flag_prefixuse == 1) && (ipv4addrp->prefixlength < 32)) {
		ipv6addrp->prefixlength = 48 - 32 + ipv4addrp->prefixlength;
		ipv6addrstruct_maskprefix(ipv6addrp);
	} else {
		ipv6addrp->prefixlength = 48;
	};
	ipv6addrp->flag_valid = 1;
	ipv6addr_settype(ipv6addrp); /* Set typeinfo */

	retval = 0;
	return (retval);
};


/*
 * function converts an IPv6 address to an IPv4 one (reverse RFC 3056)
 *  
 * in : *ipv6addrp = IPv6 address
 * out: *ipv4addrp = IPv4 address
 * ret: ==0: ok, !=0: error
 */
int librfc3056_ipv6addr_to_ipv4addr(ipv6calc_ipv4addr *ipv4addrp, const ipv6calc_ipv6addr *ipv6addrp, char* resultstring, const size_t resultstring_length) {
	int retval = 1;
	unsigned int i;

	/* check scope */
	if ( (ipv6addrp->typeinfo & IPV6_NEW_ADDR_6TO4) == 0 ) {
		snprintf(resultstring, resultstring_length, "IPv6 address is not a 6to4 one!");
		return(retval);	
	};

	/* clear IPv4 structure */
	ipv4addr_clear(ipv4addrp);

	/* set IPv4 address */
	for (i = 0; i < 4; i++) {
		ipv4addr_setoctet(ipv4addrp, i, (unsigned int) ipv6addr_getoctet(ipv6addrp, i + 2));
	};

	ipv4addrp->flag_prefixuse = ipv6addrp->flag_prefixuse;
	if ((ipv6addrp->flag_prefixuse == 1) && (ipv6addrp->prefixlength < 48)) {
		ipv4addrp->prefixlength = 32 - 48 + ipv6addrp->prefixlength;
		ipv4addrstruct_maskprefix(ipv4addrp);
	} else {
		ipv4addrp->prefixlength = 32;
	};
	ipv4addrp->flag_valid = 1;
	ipv4addr_settype(ipv4addrp, 1); /* Set typeinfo */

	retval = 0;
	return (retval);
};
