/*
 * Project    : ipv6calc
 * File       : libifinet6.c
 * Version    : $Id: libifinet6.c,v 1.1 2002/03/01 23:27:25 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function to converts a address string like shown in 
 *   Linux /proc/net/if_inet6 to IPv6 address structure
 *
 * some hints taken from ifconfig.c (net-tools)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipv6calc.h"
#include "libifinet6.h"

/*
 * function formats an given Linux /proc/net/if_inet6 format to normal one
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libifinet6/ifinet6_to_ipv6addrstruct"
int libifinet6_ifinet6_to_ipv6addrstruct(char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1;
	char tempstring[NI_MAXHOST];
	char addr6p[8][5]; 

	if (ipv6calc_debug) {
		fprintf(stderr, "%s: Got input %s\n", DEBUG_function_name, addrstring);
	};
	
	/* simple test */
	if ( strlen(addrstring) != 32 ) {
		sprintf(resultstring, "Given hex string '%s' has not 32 chars!", addrstring);
		retval = 1;
		return (retval);
	};

	/* scan address into array */
	retval = sscanf(addrstring, "%4s%4s%4s%4s%4s%4s%4s%4s\n", addr6p[0], addr6p[1], addr6p[2], addr6p[3], addr6p[4], addr6p[5], addr6p[6], addr6p[7]);
	if ( retval != 8 ) {
		sprintf(resultstring, "Error splitting string %s, got only %d items!", addrstring, retval);
		retval = 1;
		return (retval);
	};

	/* print out into temp string */
	sprintf(tempstring, "%s:%s:%s:%s:%s:%s:%s:%s", addr6p[0], addr6p[1], addr6p[2], addr6p[3], addr6p[4], addr6p[5], addr6p[6], addr6p[7]); 
					 
	if (ipv6calc_debug) {
		fprintf(stderr, "%s: Filled: %s\n", DEBUG_function_name, tempstring);
	};

	/* fill structure */
	retval = addr_to_ipv6addrstruct(tempstring, resultstring, ipv6addrp);

	return (retval);
};
