/*
 * Project    : ipv6calc
 * File       : libipaddr.c
 * Version    : $Id$
 * Copyright  : 2014-2015 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library for generic IPv4/6 address handling
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "libipv6addr.h"
#include "libipv4addr.h"
#include "ipv6calctypes.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "librfc1884.h"


/*
 * stores the ipaddr structure in an IPv4/6 string
 *
 * in:  ipaddr = IPv4/6 address structure
 * out: *resultstring = IPv4/6 address (modified)
 * ret: ==0: ok, !=0: error
 */
int libipaddr_ipaddrstruct_to_string(const ipv6calc_ipaddr *ipaddrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	int retval = 1;

	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_ipv6addr ipv6addr;

	switch (ipaddrp->proto) {
	    case IPV6CALC_PROTO_IPV4:
		ipv4addr_clearall(&ipv4addr);
		ipv4addr_setdword(&ipv4addr, ipaddrp->addr[0]);
		libipv4addr_ipv4addrstruct_to_string(&ipv4addr, resultstring, resultstring_length, formatoptions);
		break;

	    case IPV6CALC_PROTO_IPV6:
		ipv6addr_clearall(&ipv6addr);
		ipv6addr_setdword(&ipv6addr, 0, ipaddrp->addr[0]);
		ipv6addr_setdword(&ipv6addr, 1, ipaddrp->addr[1]);
		ipv6addr_setdword(&ipv6addr, 2, ipaddrp->addr[2]);
		ipv6addr_setdword(&ipv6addr, 3, ipaddrp->addr[3]);

		if ((formatoptions & (FORMATOPTION_printuncompressed | FORMATOPTION_printfulluncompressed)) != 0) {
			libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, resultstring_length, formatoptions);
		} else {
			librfc1884_ipv6addrstruct_to_compaddr(&ipv6addr, resultstring, resultstring_length, formatoptions);
		};
		break;

	    default:
		ERRORPRINT_WA("unsupported proto=%d (FIX CODE)", ipaddrp->proto);
		exit(EXIT_FAILURE);
		break;
	};

	DEBUGPRINT_WA(DEBUG_libipaddr, "result string: %s", resultstring);
	
	retval = 0;	
	return (retval);
};


/*
 * function clears the IPv6 structure
 *
 * mod: ipaddrp = pointer to IP address structure
 */
extern void libipaddr_clearall(ipv6calc_ipaddr *ipaddrp) {
        /* Clear IP address */
	ipaddrp->addr[0] = 0;
	ipaddrp->addr[1] = 0;
	ipaddrp->addr[2] = 0;
	ipaddrp->addr[3] = 0;

        /* Clear IP address scope */
        ipaddrp->scope = 0;

	/* Clear proto */
	ipaddrp->proto = 0;

        /* Clear valid flag */
        ipaddrp->flag_valid = 0;

	return;
};
