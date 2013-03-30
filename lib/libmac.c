/*
 * Project    : ipv6calc
 * File       : libmac.c
 * Version    : $Id: libmac.c,v 1.19 2013/03/30 18:03:45 ds6peter Exp $
 * Copyright  : 2001-2012 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library MAC address handling
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libipv6calc.h"
#include "libmac.h"
#include "ipv6calctypes.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"

static char ChSet[] = "0123456789abcdefABCDEF:- ";

/* function 48-bit MAC address to MACaddr_structure
 *
 * in : *addrstring = MAC address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libmac/mac_to_macaddrstruct"
int mac_to_macaddrstruct(char *addrstring, char *resultstring, ipv6calc_macaddr *macaddrp) {
	int retval = 1, result, i, ccolons = 0, cdashes = 0, cspaces = 0;
	size_t cnt;
	int temp[6];

	/* check length */
	if ( ( strlen(addrstring) < 11 ) || ( strlen(addrstring) > 17 ) ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error in given 48-bit MAC address, has not 11 to 17 chars!");
		retval = 1;
		return (retval);
	};

	/* check for hex chars and ":"/"-"/" " only content */
	cnt = strspn(addrstring, ChSet);
	if ( cnt < strlen(addrstring) ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Illegal character in given MAC address '%s' on position %d (%c)!", addrstring, (int) cnt+1, addrstring[cnt]);
		retval = 1;
		return (retval);
		
	};

	/* count ":" or "-" or " " must be 5 or 1 x "-" */
	for (i = 0; i < (int) strlen(addrstring); i++) {
		if (addrstring[i] == ':') {
			ccolons++;
		} else if (addrstring[i] == '-') {
			cdashes++;
		} else if (addrstring[i] == ' ') {
			cspaces++;
		};
	};

	if ( ! ( (ccolons == 5 && cdashes == 0 && cspaces == 0) || (ccolons == 0 && cdashes == 5 && cspaces == 0)  || (ccolons == 0 && cdashes == 0 && cspaces == 5) || (ccolons == 0 && cdashes == 1 && cspaces == 0 && strlen(addrstring) == 13) || (ccolons == 0 && cdashes == 0 && cspaces == 0 && strlen(addrstring) == 12)) ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error, given MAC address '%s' is not valid (number of colons/dashes/spaces is not 5 or number of dashes is not 1)!", addrstring);
		retval = 1;
		return (retval);
	};

	/* scan address into array */
	if ( ccolons == 5 ) {
		result = sscanf(addrstring, "%x:%x:%x:%x:%x:%x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5]);
	} else if ( cdashes == 5 ) {
		result = sscanf(addrstring, "%x-%x-%x-%x-%x-%x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5]);
	} else if ( cdashes == 1 ) {
		result = sscanf(addrstring, "%2x%2x%2x-%2x%2x%2x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5]);
	} else if ( cspaces == 5 ) {
		result = sscanf(addrstring, "%x %x %x %x %x %x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5]);
	} else if ( cdashes == 0 ) {
		result = sscanf(addrstring, "%2x%2x%2x%2x%2x%2x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5]);
	} else {
		snprintf(resultstring, NI_MAXHOST - 1, "Error, unexpected failure on scanning MAC address '%s'!", addrstring);
		retval = 1;
		return (retval);
	};

	if ( result != 6 ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error splitting address %s, got %d items instead of 6!", addrstring, result);
		retval = 1;
		return (retval);
	};

	/* check address words range */
	for ( i = 0; i <= 5; i++ ) {
		if ( ( temp[i] < 0x0 ) || ( temp[i] > 0xff ) )    {
			snprintf(resultstring, NI_MAXHOST - 1, "Error, given MAC address '%s' is not valid on position %d!", addrstring, i);
			retval = 1;
			return (retval);
		};
	};  

	/* copy address */
	for ( i = 0; i <= 5; i++ ) {
		macaddrp->addr[i] = (uint8_t) temp[i];
	};  

	macaddrp->flag_valid = 1;

   	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name

/* compatible name */
int addr_to_macaddrstruct(char *addrstring, char *resultstring, ipv6calc_macaddr *macaddrp) {
	return (mac_to_macaddrstruct(addrstring, resultstring, macaddrp));
};

/* 
 * clear MACaddr
 *
 * mod: *addrstring = MAC address
 */
void mac_clear(ipv6calc_macaddr *macaddrp) {
	int i;

	for ( i = 0; i <= 5; i++ ) {
		macaddrp->addr[i] = 0;
	};  

	return;
};

/* 
 * clear MACaddr_structure
 *
 * mod: *addrstring = MAC address
 */
void mac_clearall(ipv6calc_macaddr *macaddrp) {
	mac_clear(macaddrp);

	/* Clear valid flag */
	macaddrp->flag_valid = 0;

	return;
};


/*
 * stores the macaddr structure in a string
 *
 * in:  macaddr = MAC address structure
 * out: *resultstring = MAC address string
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libmac/macaddrstruct_to_string"
int macaddrstruct_to_string(const ipv6calc_macaddr *macaddrp, char *resultstring, const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST];

	/* address */
	snprintf(tempstring, sizeof(tempstring) - 1, "%02x:%02x:%02x:%02x:%02x:%02x", (unsigned int) macaddrp->addr[0], (unsigned int) macaddrp->addr[1], (unsigned int) macaddrp->addr[2], (unsigned int) macaddrp->addr[3], (unsigned int) macaddrp->addr[4], (unsigned int) macaddrp->addr[5]);

	if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
		snprintf(resultstring, NI_MAXHOST - 1, "MAC=%s", tempstring);
	} else {
		snprintf(resultstring, NI_MAXHOST - 1, "%s", tempstring);
	};

	return(0);
};
#undef DEBUG_function_name

/* compatible name */
int libmacaddr_macaddrstruct_to_string(const ipv6calc_macaddr *macaddrp, char *resultstring, const uint32_t formatoptions) {
	return (macaddrstruct_to_string(macaddrp, resultstring, formatoptions));
};


/*
 * clear filter MAC address
 *
 * in : *filter    = filter structure
 */
void macaddr_filter_clear(s_ipv6calc_filter_macaddr *filter) {
	filter->active = 0;
	filter->typeinfo_must_have = 0;
	return;
};



/*
 * filter MAC address
 *
 * in : *macaddrp = MAC address structure
 * in : *filter    = filter structure
 * ret: 0=match 1=not match
 */
int macaddr_filter(const ipv6calc_macaddr *macaddrp, const s_ipv6calc_filter_macaddr *filter) {

	return 1;
};


/*
 * anonymize MAC address
 *
 * in : *macaddrp = MAC address structure
 *      mask = number of bits of mask
 * ret: <void>
 */
void libmacaddr_anonymize(ipv6calc_macaddr *macaddrp, const unsigned int mask) {
	int i, max, min, bit;

	/* anonymize MAC address according to settings */

	if (mask == 0) {
		/* clear MAC address: 0:0:0:0:0:0 */
		mac_clear(macaddrp);
	} else if (mask == 48) {
		/* nothing to do */
	} else if (mask < 1 || mask > 48) {
		/* should not happen here */
		fprintf(stderr, "%s/%s: 'mask' has an unexpected illegal value: %d!\n", __FILE__, __func__, mask);
		exit(EXIT_FAILURE);
	} else {
		/* complete byte mask */
		max = 6;
		min = mask / 8 + 2;

                if ( (ipv6calc_debug ) != 0 ) {
                        fprintf(stderr, "%s/%s: mask=%d -> complete byte mask from %d to %d\n", __FILE__, __func__, mask, max, min);
                };

		if (max > min) {
			for (i = max; i >= min; i--) {
				macaddrp->addr[i-1] = 0;
			};
		};

		/* partial byte mask */
		bit = (~ (0xffu >> (mask % 8))) & 0xff;
                if ( (ipv6calc_debug ) != 0 ) {
                        fprintf(stderr, "%s/%s: mask=%d -> partial byte mask %d with 0x%02x\n", __FILE__, __func__, mask, min-1, bit);
                };

		macaddrp->addr[min-2] &= bit;
	};
	return;
};
