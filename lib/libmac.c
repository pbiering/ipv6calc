/*
 * Project    : ipv6calc
 * File       : libmac.c
 * Version    : $Id$
 * Copyright  : 2001-2014 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library MAC address handling
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libipv6calc.h"
#include "libmac.h"
#include "libieee.h"
#include "ipv6calctypes.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"

static char ChSet[] = "0123456789abcdefABCDEF:- .";

/* function 48-bit MAC address to MACaddr_structure
 *
 * in : *addrstring = MAC address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int mac_to_macaddrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_macaddr *macaddrp) {
	int retval = 1, result, i, ccolons = 0, cdashes = 0, cspaces = 0, cdots = 0;
	size_t cnt;
	int temp[6];

	/* check length */
	if ( ( strlen(addrstring) < 11 ) || ( strlen(addrstring) > 17 ) ) {
		snprintf(resultstring, resultstring_length, "Error in given 48-bit MAC address, has not 11 to 17 chars!");
		retval = 1;
		return (retval);
	};

	/* check for hex chars and ":"/"-"/" " only content */
	cnt = strspn(addrstring, ChSet);
	if ( cnt < strlen(addrstring) ) {
		snprintf(resultstring, resultstring_length, "Illegal character in given MAC address '%s' on position %d (%c)!", addrstring, (int) cnt+1, addrstring[cnt]);
		retval = 1;
		return (retval);
		
	};

	/* count ":" or "-" or " " or " " must be 5 or 1 x "-" */
	for (i = 0; i < (int) strlen(addrstring); i++) {
		if (addrstring[i] == ':') {
			ccolons++;
		} else if (addrstring[i] == '-') {
			cdashes++;
		} else if (addrstring[i] == ' ') {
			cspaces++;
		} else if (addrstring[i] == '.') {
			cdots++;
		};
	};

	if ( ! ( (ccolons == 5 && cdashes == 0 && cspaces == 0 && cdots == 0)
		   || (ccolons == 0 && cdashes == 5 && cspaces == 0 && cdots == 0)
		   || (ccolons == 0 && cdashes == 0 && cspaces == 5 && cdots == 0)
		   || (ccolons == 0 && cdashes == 0 && cspaces == 0 && cdots == 2)
		   || (ccolons == 0 && cdashes == 1 && cspaces == 0 && strlen(addrstring) == 13 && cdots == 0)
		   || (ccolons == 0 && cdashes == 0 && cspaces == 0 && strlen(addrstring) == 12 && cdots == 0))
	   ) {
		snprintf(resultstring, resultstring_length, "Error in given MAC address, '%s' is not valid (number of colons/dashes/spaces is not 5 or number of dashes is not 1)!", addrstring);
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
	} else if ( cdots == 2 ) {
		result = sscanf(addrstring, "%2x%2x.%2x%2x.%2x%2x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5]);
	} else if ( cdashes == 0 ) {
		result = sscanf(addrstring, "%2x%2x%2x%2x%2x%2x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5]);
	} else {
		snprintf(resultstring, resultstring_length, "Error in given MAC address, unexpected failure on scanning '%s'!", addrstring);
		retval = 1;
		return (retval);
	};

	if ( result != 6 ) {
		snprintf(resultstring, resultstring_length, "Error in given MAC address, splitting of '%s' returns %d items instead of 6!", addrstring, result);
		retval = 1;
		return (retval);
	};

	/* check address words range */
	for ( i = 0; i <= 5; i++ ) {
		if ( ( temp[i] < 0x0 ) || ( temp[i] > 0xff ) )    {
			snprintf(resultstring, resultstring_length, "Error in given MAC address, '%s' is not valid on position %d!", addrstring, i);
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


/* compatible name */
int addr_to_macaddrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_macaddr *macaddrp) {
	return (mac_to_macaddrstruct(addrstring, resultstring, resultstring_length, macaddrp));
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
int macaddrstruct_to_string(const ipv6calc_macaddr *macaddrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST];

	/* address */
	snprintf(tempstring, sizeof(tempstring), "%02x:%02x:%02x:%02x:%02x:%02x", (unsigned int) macaddrp->addr[0], (unsigned int) macaddrp->addr[1], (unsigned int) macaddrp->addr[2], (unsigned int) macaddrp->addr[3], (unsigned int) macaddrp->addr[4], (unsigned int) macaddrp->addr[5]);

	if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
		snprintf(resultstring, resultstring_length, "MAC=%s", tempstring);
	} else {
		snprintf(resultstring, resultstring_length, "%s", tempstring);
	};

	return(0);
};


/* compatible name */
int libmacaddr_macaddrstruct_to_string(const ipv6calc_macaddr *macaddrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	return (macaddrstruct_to_string(macaddrp, resultstring, resultstring_length, formatoptions));
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
void libmacaddr_anonymize(ipv6calc_macaddr *macaddrp, const s_ipv6calc_anon_set *ipv6calc_anon_set_p) {
	int mask = 0, i, j;
	uint8_t bit_ul = 0;

	/* anonymize MAC address according to settings */
	DEBUGPRINT_WA(DEBUG_libmac, "called: EUI-48=%06x%06x method=%d", EUI48_00_23(macaddrp->addr), EUI48_24_47(macaddrp->addr), ipv6calc_anon_set_p->method);

	if (ipv6calc_anon_set_p->mask_autoadjust == 1) {
		DEBUGPRINT_NA(DEBUG_libmac, "mask-autoadjust is set, autoselect proper mask");

		if ((macaddrp->addr[0] & 0x2) == 0) {
			// global address
			mask = 24;

			if (libieee_check_oui36_iab(EUI48_00_23(macaddrp->addr)) == 1) {
				// OUI-36/IAB
				mask += 12; // increase by 12 bits
			};

			DEBUGPRINT_WA(DEBUG_libmac, "EUI-48 is a global one, source of mask: automagic: %d", mask);
		} else {
			// local address, honor mask_mac
			mask = ipv6calc_anon_set_p->mask_mac;
			DEBUGPRINT_WA(DEBUG_libmac, "EUI-48 is a local one, source of mask: mask-iid option: %d", mask);
		};

		if (ipv6calc_anon_set_p->mask_mac > mask) {
			mask = ipv6calc_anon_set_p->mask_mac;
			DEBUGPRINT_WA(DEBUG_libmac, "specified mask is higher than autoselected one, change to specified: %d", mask);
		};
	} else {
		DEBUGPRINT_WA(DEBUG_libmac, "mask-autoadjust is not set, use always mask: %d", mask);
		mask = ipv6calc_anon_set_p->mask_mac;
	};

	DEBUGPRINT_WA(DEBUG_libmac, "zeroize EUI-48 with masked bits: %d", mask);

	// save universal/local bit
	bit_ul = macaddrp->addr[0] & 0x02;

	if (mask == 48) {
		// nothing to do
	} else if (mask > 0) {
		j = mask >> 3;

		for (i = 5; i >= 0; i--) {
			DEBUGPRINT_WA(DEBUG_libmac, "zeroize EUI-48: mask=%02d i=%d j=%d", mask, i, j);
			if (j < i) {
				DEBUGPRINT_WA(DEBUG_libmac, "zeroize EUI-48: byte %d", i);
				macaddrp->addr[i] = 0x00;
			} else if (j == i) {
				DEBUGPRINT_WA(DEBUG_libmac, "zeroize EUI-48: mask byte %d with %02x (offset: %d)", i, (0xff00 >> (mask % 0x8)) & 0xff, (mask % 0x8));
				macaddrp->addr[i] &= (0xff00 >> (mask % 0x8)) & 0xff;
			} else {
				DEBUGPRINT_NA(DEBUG_libmac, "zeroize EUI-48: finished");
				break;
			};
		};
	} else {
		mac_clear(macaddrp);
	};

	// restore universal/local bit
	macaddrp->addr[0] = (macaddrp->addr[0] & 0xfd) | bit_ul;
	
	DEBUGPRINT_WA(DEBUG_libmac, "anonymization finished, return: %06x%06x", EUI48_00_23(macaddrp->addr), EUI48_24_47(macaddrp->addr));
	return;
};
