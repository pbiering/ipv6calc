/*
 * Project    : ipv6calc
 * File       : libeui64.c
 * Version    : $Id: libeui64.c,v 1.3 2013/04/09 20:09:33 ds6peter Exp $
 * Copyright  : 2001-2013 by Peter Bieringer <pb (at) bieringer.de>
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

static char ChSet[] = "0123456789abcdefABCDEF:- ";

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


/* function 48-bit EUI-64 address to eui64addr_structure
 *
 * in : *addrstring = EUI-64 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int libeui64_addr_to_eui64addrstruct(char *addrstring, char *resultstring, ipv6calc_eui64addr *eui64addrp) {
	int retval = 1, result, i, ccolons = 0, cdashes = 0, cspaces = 0;
	size_t cnt;
	int temp[8];

	/* check length */
	if ( ( strlen(addrstring) < 15 ) || ( strlen(addrstring) > 23 ) ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error in given 64-bit EUI-64 address, has not 15 to 21 chars!");
		retval = 1;
		return (retval);
	};

	/* check for hex chars and ":"/"-"/" " only content */
	cnt = strspn(addrstring, ChSet);
	if ( cnt < strlen(addrstring) ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Illegal character in given EUI-64 address '%s' on position %d (%c)!", addrstring, (int) cnt+1, addrstring[cnt]);
		retval = 1;
		return (retval);
		
	};

	/* count ":" or "-" or " " must be 7 x "-" */
	for (i = 0; i < (int) strlen(addrstring); i++) {
		if (addrstring[i] == ':') {
			ccolons++;
		} else if (addrstring[i] == '-') {
			cdashes++;
		} else if (addrstring[i] == ' ') {
			cspaces++;
		};
	};

	if ( ! ( (ccolons == 7 && cdashes == 0 && cspaces == 0) || (ccolons == 0 && cdashes == 5 && cspaces == 0)  || (ccolons == 0 && cdashes == 0 && cspaces == 7) || (ccolons == 0 && cdashes == 0 && cspaces == 0 && strlen(addrstring) == 16)) ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error, given EUI-64 address '%s' is not valid (number of colons/dashes/spaces is not 5 or number of dashes is not 1)!", addrstring);
		retval = 1;
		return (retval);
	};

	/* scan address into array */
	if ( ccolons == 7 ) {
		result = sscanf(addrstring, "%x:%x:%x:%x:%x:%x:%x:%x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5], &temp[6], &temp[7]);
	} else if ( cdashes == 7 ) {
		result = sscanf(addrstring, "%x-%x-%x-%x-%x-%x-%x-%x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5], &temp[6], &temp[7]);
	} else if ( cspaces == 7 ) {
		result = sscanf(addrstring, "%x %x %x %x %x %x %x %x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5], &temp[6], &temp[7]);
	} else if ( cdashes == 0 ) {
		result = sscanf(addrstring, "%2x%2x%2x%2x%2x%2x%2x%2x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5], &temp[6], &temp[7]);
	} else {
		snprintf(resultstring, NI_MAXHOST - 1, "Error, unexpected failure on scanning EUI-64 address '%s'!", addrstring);
		retval = 1;
		return (retval);
	};

	if ( result != 8 ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error splitting address %s, got %d items instead of 8!", addrstring, result);
		retval = 1;
		return (retval);
	};

	/* check address words range */
	for ( i = 0; i <= 7; i++ ) {
		if ( ( temp[i] < 0x0 ) || ( temp[i] > 0xff ) )    {
			snprintf(resultstring, NI_MAXHOST - 1, "Error, given EUI-64 address '%s' is not valid on position %d!", addrstring, i);
			retval = 1;
			return (retval);
		};
	};  

	/* copy address */
	for ( i = 0; i <= 7; i++ ) {
		eui64addrp->addr[i] = (uint8_t) temp[i];
	};  

	eui64addrp->flag_valid = 1;

   	retval = 0;	
	return (retval);
};

