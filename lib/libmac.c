/*
 * Project    : ipv6calc
 * File       : libmac.c
 * Version    : $Id: libmac.c,v 1.3 2002/04/04 19:40:27 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function libary for conversions of MAC addresses
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "libipv6calc.h"
#include "libmac.h"

static char ChSet[] = "0123456789abcdefABCDEF:";

/* function 48-bit MAC address to MACaddr_structure
 *
 * in : *addrstring = MAC address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libmac/mac_to_macaddrstruct"
int mac_to_macaddrstruct(char *addrstring, char *resultstring, ipv6calc_macaddr *macaddrp) {
	int retval = 1, result, i, ccolons = 0;
	size_t cnt;
	int temp[6];

	/* check length */
	if ( ( strlen(addrstring) < 11 ) || ( strlen(addrstring) > 17 ) ) {
		snprintf(resultstring, NI_MAXHOST, "Error in given 48-bit MAC address, has not 11 to 17 chars!");
		retval = 1;
		return (retval);
	};

	/* check for hex chars and ":" only content */
	cnt = strspn(addrstring, ChSet);
	if ( cnt < strlen(addrstring) ) {
		snprintf(resultstring, NI_MAXHOST, "Illegal character in given MAC address '%s' on position %d (%c)!", addrstring, (int) cnt+1, addrstring[cnt]);
		retval = 1;
		return (retval);
		
	};

	/* count ":", must be 5 */
	for (i = 0; i < strlen(addrstring); i++) {
		if (addrstring[i] == ':') {
			ccolons++;
		};
	};

	if ( ccolons != 5 ) {
		snprintf(resultstring, NI_MAXHOST, "Error, given MAC address '%s' is not valid (number of colons is not 5)!", addrstring);
		retval = 1;
		return (retval);
	};

	/* scan address into array */
	result = sscanf(addrstring, "%x:%x:%x:%x:%x:%x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5]);

	if ( result != 6 ) {
		snprintf(resultstring, NI_MAXHOST, "Error splitting address %s, got %d items instead of 6!", addrstring, result);
		retval = 1;
		return (retval);
	};

	/* check address words range */
	for ( i = 0; i <= 5; i++ ) {
		if ( ( temp[i] < 0x0 ) || ( temp[i] > 0xff ) )    {
			snprintf(resultstring, NI_MAXHOST, "Error, given MAC address '%s' is not valid on position %d (%x)!", addrstring, i, temp[i]);
			retval = 1;
			return (retval);
		};
	};  

	/* copy address */
	for ( i = 0; i <= 5; i++ ) {
		macaddrp->addr[i] = temp[i];
	};  

	macaddrp->flag_valid = 1;

   	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name


/* 
 * clear MACaddr_structure
 *
 * in : *addrstring = MAC address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
void mac_clearall(ipv6calc_macaddr *macaddrp) {
	int i;

	for ( i = 0; i <= 5; i++ ) {
		macaddrp->addr[i] = 0;
	};  

	macaddrp->flag_valid = 1;

	return;
};
