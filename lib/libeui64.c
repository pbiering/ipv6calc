/*
 * Project    : ipv6calc
 * File       : libeui64.c
 * Version    : $Id: libeui64.c,v 1.2 2007/01/31 16:21:47 peter Exp $
 * Copyright  : 2001-2007 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library EUI-64 identifier handling
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libeui64.h"

#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "libipv6addr.h"

/* function MAC address to EUI format
 *
 * in : macaddrp
 * out: ipv6addrp
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libeui64/create_eui64_from_mac"
int create_eui64_from_mac(ipv6calc_ipv6addr *ipv6addrp, ipv6calc_macaddr *macaddrp) {
	int retval = 1;

	if ( ipv6calc_debug != 0 ) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};

	/* clear IPv6 structure */
	ipv6addr_clear(ipv6addrp);

	/* create EUI-64 from MAC-48 */
	ipv6addrp->in6_addr.s6_addr[ 8] = macaddrp->addr[0] ^ 0x02;
   	ipv6addrp->in6_addr.s6_addr[ 9] = macaddrp->addr[1];
   	ipv6addrp->in6_addr.s6_addr[10] = macaddrp->addr[2];
   	ipv6addrp->in6_addr.s6_addr[11] = 0xff;
	ipv6addrp->in6_addr.s6_addr[12] = 0xfe;
   	ipv6addrp->in6_addr.s6_addr[13] = macaddrp->addr[3];
   	ipv6addrp->in6_addr.s6_addr[14] = macaddrp->addr[4];
   	ipv6addrp->in6_addr.s6_addr[15] = macaddrp->addr[5];

	ipv6addrp->prefixlength = 64;
	ipv6addrp->flag_prefixuse = 0;
	ipv6addrp->scope = 0;
	
	ipv6addrp->flag_valid = 1;
	
   	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name
