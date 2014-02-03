/*
 * Project    : ipv6calc
 * File       : librfc1884.c
 * Version    : $Id: librfc1884.c,v 1.20 2014/02/03 20:48:04 ds6peter Exp $
 * Copyright  : 2001-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function library for conversions defined in RFC 1884
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libipv6calcdebug.h"
#include "libipv6addr.h"
#include "libipv6calc.h"
#include "librfc1884.h"
#include "ipv6calctypes.h"


/*
 * function decompress a given IPv6 address (reverse RFC 1884)
 *  
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 *
 * Based on code in from 'ircd'
 */
int compaddr_to_uncompaddr(const char *addrstring, char *resultstring) {
	int retval = 1, cnt;
	char *cp, *op, *strp;
	char tempstring[NI_MAXHOST];

	DEBUGPRINT_WA(DEBUG_librfc1884, "got input: %s", addrstring);

	snprintf(tempstring, sizeof(tempstring) - 1, "%s", addrstring);

	strp = strstr(tempstring, "::");
	if (strp) {
			
		DEBUGPRINT_NA(DEBUG_librfc1884, "found '::' in IPv6 address");

		/* check for additional "::" occurance - not allowed! */
		if (strstr(strp+1, "::")) {
			snprintf(resultstring, NI_MAXHOST - 1, "%s", "More than 1 block of 2 colons in address is not allowed!");
			retval = 1;
			return (retval);
		};
		
		cnt = 0;
		cp = tempstring;
		op = resultstring;
	   	while (*cp != '\0') {
			if (*cp == ':')	cnt += 1;
			if (*cp++ == '.') {
				cnt += 1;
				break;
			};
		};
		cp = tempstring;
		while (*cp != '\0') {
			*op++ = *cp++;
			if (*(cp-1) == ':' && *cp == ':') {
				DEBUGPRINT_WA(DEBUG_librfc1884, "cnt: %d", cnt);
				if ((cp-1) == tempstring) {
					DEBUGPRINT_WA(DEBUG_librfc1884, "fill one '0:' (%d)", cnt);
					op--;
					*op++ = '0';
					*op++ = ':';
				};
				if (cnt < 8) {
					DEBUGPRINT_WA(DEBUG_librfc1884, "fill one '0:' (%d)", cnt);
			   		*op++ = '0';
				} else if (cnt == 8) {
					DEBUGPRINT_WA(DEBUG_librfc1884, "replace ':' by '0' (%d)", cnt);
					op--;
				};
				while (cnt++ < 7) {
					DEBUGPRINT_WA(DEBUG_librfc1884, "fill one ':0' (%d)", cnt);
					*op++ = ':';
					*op++ = '0';
				};
			};
		};
		if (*(op-1)==':') *op++ = '0';
		*op = '\0';

		DEBUGPRINT_WA(DEBUG_librfc1884, "result: %s", resultstring);
	} else {
		snprintf(resultstring, NI_MAXHOST - 1, "%s", addrstring);
		DEBUGPRINT_NA(DEBUG_librfc1884, "address is not in compressed format");
	};

	retval = 0;
	return (retval);
};


/*
 * function IPv6addrstruct to compressed format (RFC 1884)
 *
 *  compress the biggest '0' block, leading has precedence
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int ipv6addrstruct_to_compaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring) {
	int retval = -1;

	uint32_t formatoptions = FORMATOPTION_printlowercase;

	/* old style compatibility */
	retval = librfc1884_ipv6addrstruct_to_compaddr(ipv6addrp, resultstring, formatoptions);

	return (retval);
};
	
int librfc1884_ipv6addrstruct_to_compaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST], temp2string[NI_MAXHOST];
	int retval = 1;
	int zstart = -1, zend = -1, tstart = -1, tend = -1, i, w_max = 7;

	DEBUGPRINT_WA(DEBUG_librfc1884, "scope of IPv6 address: %08x", (unsigned int) ipv6addrp->scope);
	
	if ( (ipv6addrp->scope & IPV6_ADDR_COMPATv4) != 0 ) {
		/* compatv4 address */
			
		DEBUGPRINT_NA(DEBUG_librfc1884, "IPV6_ADDR_COMPATv4 type - fast conversion");
	
		snprintf(tempstring, sizeof(tempstring) - 1, "::%u.%u.%u.%u", (unsigned int) ipv6addrp->in6_addr.s6_addr[12], (unsigned int) ipv6addrp->in6_addr.s6_addr[13], (unsigned int) ipv6addrp->in6_addr.s6_addr[14], (unsigned int) ipv6addrp->in6_addr.s6_addr[15]);
		retval = 0;
	} else if ( (ipv6addrp->scope & IPV6_ADDR_MAPPED) != 0 ) {
		/* mapped address */
			
		DEBUGPRINT_NA(DEBUG_librfc1884, "IPV6_ADDR_MAPPED type - fast conversion");
		
		snprintf(tempstring, sizeof(tempstring) - 1, "::%x:%u.%u.%u.%u", (unsigned int) ipv6addr_getword(ipv6addrp, 5), (unsigned int) ipv6addrp->in6_addr.s6_addr[12], (unsigned int) ipv6addrp->in6_addr.s6_addr[13], (unsigned int) ipv6addrp->in6_addr.s6_addr[14], (unsigned int) ipv6addrp->in6_addr.s6_addr[15]);
		retval = 0;
	} else if ( (ipv6addr_getdword(ipv6addrp, 0) == 0) && (ipv6addr_getdword(ipv6addrp, 1) == 0) && (ipv6addr_getdword(ipv6addrp, 2) == 0) && (ipv6addr_getdword(ipv6addrp, 3) == 0) ) {
		/* unspecified address */
			
		DEBUGPRINT_NA(DEBUG_librfc1884, "unspecified address - fast conversion");
		
		snprintf(tempstring, sizeof(tempstring) - 1, "::");
		retval = 0;
	} else if ( (ipv6addr_getdword(ipv6addrp, 0) == 0) && (ipv6addr_getdword(ipv6addrp, 1) == 0) && (ipv6addr_getdword(ipv6addrp, 2) == 0) && (ipv6addr_getdword(ipv6addrp, 3) == 1) ) {
		/* loopback address */
			
		DEBUGPRINT_NA(DEBUG_librfc1884, "loopback - fast conversion");
		
		snprintf(tempstring, sizeof(tempstring) - 1, "::1");
		retval = 0;
	} else {
		/* normal address */

		if ( ((ipv6addrp->scope & IPV6_ADDR_IID_32_63_HAS_IPV4) != 0) && ((ipv6addrp->scope & IPV6_ADDR_ANONYMIZED_IID) == 0)) {
			w_max = 5;
		};
			
		DEBUGPRINT_NA(DEBUG_librfc1884, "normal address, detect '0' blocks now");

		for ( i = 0; i <= w_max; i++ ) {
			if ( ipv6addr_getword(ipv6addrp, (unsigned int) i) == 0 ) {
				/* found a '0' */
					
				DEBUGPRINT_WA(DEBUG_librfc1884, "Found '0' in word '%d'", i);

				if ( tstart == -1 ) {
					/* possible starting '0' block */ 
						
					DEBUGPRINT_WA(DEBUG_librfc1884, "Found a possible '0' starting block at '%d'",  i);
					
					tstart = i;				
				};
			} else {
				/* end of a '0' block */
					
				DEBUGPRINT_WA(DEBUG_librfc1884, "Found non '0' in word '%d'", i);

				if ( tstart != -1 ) {
					tend = i - 1;
					if ( ( tend - tstart ) >= 0 ) {
						/* ok, a block with 1 or more '0' */
							
						DEBUGPRINT_WA(DEBUG_librfc1884, "Found a '0' block from '%d' to '%d' with length '%d'", tstart, tend, tend - tstart + 1);
						
						if ( zstart < 0 ) {
							/* no other block before, init */
							zstart = tstart;
							zend = tend;
						
							DEBUGPRINT_WA(DEBUG_librfc1884, "First found '0' block from '%d' to '%d' with length '%d'",  zstart, zend, zend - zstart + 1);
							
						} else if ( ( zend - zstart ) < ( tend - tstart ) ) {
							/* ok, bigger block found */
							zstart = tstart;
							zend = tend;
							
							DEBUGPRINT_WA(DEBUG_librfc1884, "Found bigger '0' block from '%d' to '%d' with length '%d'",  zstart, zend, zend - zstart + 1);
							
						} else {
								
							DEBUGPRINT_NA(DEBUG_librfc1884, "This '0' block is not bigger than the last one - skip");

						};
					};
					tstart = -1;
					tend = -1;
				};
			};
		};
		/* cleanup */
		if ( tstart >= 0 ) {
			tend = w_max;
			/* trailing '0' block */
			if ( ( tend - tstart ) > 0 ) {
				/* ok, a block with 2 or more '0' */
				if ( zstart < 0 ) {
					/* no other block before, init */
					zstart = tstart;
					zend = tend;
				} else if ( ( zend - zstart ) < ( tend - tstart ) ) {
					/* ok, bigger block found */
					zstart = tstart;
					zend = tend;
				};
			};
		};

		if ( zstart != -1 ) {
			DEBUGPRINT_WA(DEBUG_librfc1884, "biggest '0' block is from word '%d' to '%d'",  zstart, zend);
		} else {
			DEBUGPRINT_NA(DEBUG_librfc1884, "no '0' block found");
		};

		/* create string */
		tempstring[0] = '\0';

		for ( i = 0; i <= w_max; i++ ) {
			if ( i == zstart ) {
		
				DEBUGPRINT_WA(DEBUG_librfc1884, "start of '0' at '%d'", i);
				
				snprintf(temp2string, sizeof(temp2string) - 1, "%s:", tempstring);
			} else if ( i == 0 ) {
				DEBUGPRINT_WA(DEBUG_librfc1884, "normal start value at '%d' (%x)", i, (unsigned int) ipv6addr_getword(ipv6addrp, (unsigned int) i));
				
				snprintf(temp2string, sizeof(temp2string) - 1, "%x", (unsigned int) ipv6addr_getword(ipv6addrp, (unsigned int) i));
			} else if ( ( i > zend ) || ( i < zstart ) ) {
				snprintf(temp2string, sizeof(temp2string) - 1, "%s:%x", tempstring, (unsigned int) ipv6addr_getword(ipv6addrp, (unsigned int) i));
			} else if ( ( i == 7 ) && ( zend == i )) {
				snprintf(temp2string, sizeof(temp2string) - 1, "%s:", tempstring);
			};
			snprintf(tempstring, sizeof(tempstring) - 1, "%s", temp2string);
		};
		
		if ( ((ipv6addrp->scope & IPV6_ADDR_IID_32_63_HAS_IPV4) != 0) && ((ipv6addrp->scope & IPV6_ADDR_ANONYMIZED_IID) == 0)) {
			/* append IPv4 address */
			snprintf(temp2string, sizeof(temp2string) - 1, "%s:%u.%u.%u.%u", \
				tempstring, \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[12], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[13], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[14], \
				(unsigned int) ipv6addrp->in6_addr.s6_addr[15]  \
			);
			snprintf(tempstring, sizeof(tempstring) - 1, "%s", temp2string);
		};

		DEBUGPRINT_WA(DEBUG_librfc1884, "new method: '%s'", tempstring);

		retval = 0;
	};

	if ( ( retval == 0 ) && ( ipv6addrp->flag_prefixuse == 1 ) && ((formatoptions & FORMATOPTION_literal) == 0) ) {
		snprintf(resultstring, NI_MAXHOST - 1, "%s/%u", tempstring, (unsigned int) ipv6addrp->prefixlength);
	} else {
		if ((formatoptions & FORMATOPTION_literal) != 0) {
			/* replace : by - */
			for (i =0; i < strlen(tempstring); i++) {
				if (tempstring[i] == ':') {
					tempstring[i] = '-';
				};
			};
			if (ipv6addrp->flag_scopeid) {
				snprintf(resultstring, NI_MAXHOST - 1, "%ss%s.ipv6-literal.net", tempstring, ipv6addrp->scopeid);
			} else {
				snprintf(resultstring, NI_MAXHOST - 1, "%s.ipv6-literal.net", tempstring);
			};
		} else {
			if (ipv6addrp->flag_scopeid) {
				snprintf(resultstring, NI_MAXHOST - 1, "%s%%%s", tempstring, ipv6addrp->scopeid);
			} else {
				snprintf(resultstring, NI_MAXHOST - 1, "%s", tempstring);
			};
		};
	};

	if ( (formatoptions & FORMATOPTION_printlowercase) != 0 ) {
		/* nothing to do */
	} else if ( (formatoptions & FORMATOPTION_printuppercase) != 0 ) {
		string_to_upcase(resultstring);
	};

	DEBUGPRINT_WA(DEBUG_librfc1884, "Result: '%s'", resultstring);
	return (retval);
};
