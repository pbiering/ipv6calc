/*
 * Project    : ipv6calc
 * File       : libeui64.c
 * Version    : $Id: libeui64.c,v 1.9 2013/10/31 21:24:46 ds6peter Exp $
 * Copyright  : 2001-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library EUI-64 identifier handling
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libeui64.h"
#include "libieee.h"

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
int create_eui64_from_mac(ipv6calc_ipv6addr *ipv6addrp, ipv6calc_macaddr *macaddrp) {
	int retval = 1;

	DEBUGPRINT_NA(DEBUG_libeui64, "called");

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


/*
 * stores the EUI-64 structure in a string
 *
 * in:  eui64addr_p = EUI-64 address structure ptr
 * out: *resultstring = EUI-64 address string
 * ret: ==0: ok, !=0: error
 */
int libeui64_eui64addrstruct_to_string(const ipv6calc_eui64addr *eui64addr_p, char *resultstring, const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST];

	/* address */
	snprintf(tempstring, sizeof(tempstring), "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", \
		(unsigned int) eui64addr_p->addr[0],	\
		(unsigned int) eui64addr_p->addr[1],	\
		(unsigned int) eui64addr_p->addr[2],	\
		(unsigned int) eui64addr_p->addr[3],	\
		(unsigned int) eui64addr_p->addr[4],	\
		(unsigned int) eui64addr_p->addr[5],	\
		(unsigned int) eui64addr_p->addr[6],	\
		(unsigned int) eui64addr_p->addr[7]);

	if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
		snprintf(resultstring, NI_MAXHOST, "EUI64=%s", tempstring);
	} else {
		snprintf(resultstring, NI_MAXHOST, "%s", tempstring);
	};

	return(0);
};


/* function 48-bit EUI-64 address to eui64addr_structure
 *
 * in : *addrstring = EUI-64 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int libeui64_addr_to_eui64addrstruct(const char *addrstring, char *resultstring, ipv6calc_eui64addr *eui64addrp) {
	int retval = 1, result, i, ccolons = 0, cdashes = 0, cspaces = 0;
	size_t cnt;
	int temp[8];

	DEBUGPRINT_NA(DEBUG_libeui64, "called");

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


/* 
 * clear EUI-64 addr
 *
 * mod: *addrstring = EUI-64 address
 */
void libeui64_clear(ipv6calc_eui64addr *eui64addrp) {
	int i;

	DEBUGPRINT_NA(DEBUG_libeui64, "called");

	for ( i = 0; i <= 7; i++ ) {
		eui64addrp->addr[i] = 0;
	};  

	return;
};


/* 
 * clear EUI64 addr_structure
 *
 * mod: *addrstring = EUI64 address
 */
void libeui64_clearall(ipv6calc_eui64addr *eui64addrp) {
	libeui64_clear(eui64addrp);

	DEBUGPRINT_NA(DEBUG_libeui64, "called");

	/* Clear valid flag */
	eui64addrp->flag_valid = 0;

	return;
};


/* 
 * anonymize EUI-64 addr
 *
 * mod: *addrstring = EUI-64 address
 */
void libeui64_anonymize(ipv6calc_eui64addr *eui64addrp, const s_ipv6calc_anon_set *ipv6calc_anon_set_p) {
	int mask = 0, i, j;
	uint8_t bit_ul = 0;

	DEBUGPRINT_WA(DEBUG_libeui64, "called: EUI-64=%08x%08x method=%d", EUI64_00_31(eui64addrp->addr), EUI64_32_63(eui64addrp->addr), ipv6calc_anon_set_p->method);

	// if (ipv6calc_anon_set_p->method == ANON_METHOD_ZEROIZE) { TODO: different implementations
		if (ipv6calc_anon_set_p->mask_autoadjust == 1) {
			DEBUGPRINT_NA(DEBUG_libeui64, "mask-autoadjust is set, autoselect proper mask");

			if ((eui64addrp->addr[0] & 0x2) == 0) {
				// global address
				if ((eui64addrp->addr[3] == 0xff) && (eui64addrp->addr[4] == 0xfe)) {
					// expanded EUI-48
					mask = 40; // 24 + 16 bits
					DEBUGPRINT_WA(DEBUG_libeui64, "EUI-64 is a expanded EUI-48, change mask: %d", mask);
				} else {
					mask = 24;
				};

				if (libieee_check_oui36_iab(EUI64_00_23(eui64addrp->addr)) == 1) {
					// OUI-36/IAB
					mask += 12; // increase by 12 bits
					DEBUGPRINT_WA(DEBUG_libeui64, "EUI-64 contains OUI-36/IAB, change mask: %d", mask);
				};

				DEBUGPRINT_WA(DEBUG_libeui64, "EUI-64 is a global one, source of mask: automagic: %d", mask);
			} else {
				// local address, honor mask_eui64
				mask = ipv6calc_anon_set_p->mask_eui64;
				DEBUGPRINT_WA(DEBUG_libeui64, "EUI-64 is a local one, source of mask: mask-iid option: %d", mask);
			};

			if (ipv6calc_anon_set_p->mask_eui64 > mask) {
				mask = ipv6calc_anon_set_p->mask_eui64;
				DEBUGPRINT_WA(DEBUG_libeui64, "specified mask is higher than autoselected one, change to specified: %d", mask);
			};
		} else {
			DEBUGPRINT_WA(DEBUG_libeui64, "mask-autoadjust is not set, use always mask: %d", mask);
			mask = ipv6calc_anon_set_p->mask_eui64;
		};

		// save universal/local bit
		bit_ul = eui64addrp->addr[0] & 0x02;

		DEBUGPRINT_WA(DEBUG_libeui64, "zeroize EUI-64 with masked bits: %d (universal/local=%d)", mask, bit_ul);

		if (mask == 64) {
			// nothing to do
		} else if (mask > 0) {
			j = mask >> 3;

			for (i = 7; i >= 0; i--) {
				DEBUGPRINT_WA(DEBUG_libeui64, "zeroize EUI-64: mask=%02d i=%d j=%d", mask, i, j);
				if (j < i) {
					DEBUGPRINT_WA(DEBUG_libeui64, "zeroize EUI-64: byte %d", i);
					eui64addrp->addr[i] = 0x00;
				} else if (j == i) {
					DEBUGPRINT_WA(DEBUG_libeui64, "zeroize EUI-64: mask byte %d with %02x (offset: %d)", i, (0xff00 >> (mask % 0x8)) & 0xff, (mask % 0x8));
					eui64addrp->addr[i] &= (0xff00 >> (mask % 0x8)) & 0xff;
				} else {
					DEBUGPRINT_NA(DEBUG_libeui64, "zeroize EUI-64: finished");
					break;
				};
			};
		} else {
			libeui64_clear(eui64addrp);
		};

		// restore universal/local bit
		eui64addrp->addr[0] = (eui64addrp->addr[0] & 0xfd) | bit_ul;
	// };
	
	DEBUGPRINT_WA(DEBUG_libeui64, "anonymization finished, return: %08x%08x", EUI64_00_31(eui64addrp->addr), EUI64_32_63(eui64addrp->addr));

	return;
};

