/*
 * Project    : ipv6calc
 * File       : librfc1884.c
 * Version    : $Id: librfc1884.c,v 1.14 2011/11/27 15:44:41 peter Exp $
 * Copyright  : 2001-2011 by Peter Bieringer <pb (at) bieringer.de>
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
#define DEBUG_function_name "librfc1884/compaddr_to_uncompaddr"
int compaddr_to_uncompaddr(const char *addrstring, char *resultstring) {
	int retval = 1, cnt;
	char *cp, *op, *strp;
	char tempstring[NI_MAXHOST];

	if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
		fprintf(stderr, "%s: got input: %s\n", DEBUG_function_name, addrstring);
	};

	snprintf(tempstring, sizeof(tempstring) - 1, "%s", addrstring);

	strp = strstr(tempstring, "::");
	if (strp) {
			
		if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
			fprintf(stderr, "%s: found '::' in IPv6 address\n",  DEBUG_function_name);
		};

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
				if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
					fprintf(stderr, "%s: cnt: %d\n", DEBUG_function_name, cnt);
				};
				if ((cp-1) == tempstring) {
					if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
						fprintf(stderr, "%s: fill one '0:' (%d)\n", DEBUG_function_name, cnt);
					};
					op--;
					*op++ = '0';
					*op++ = ':';
				};
				if (cnt < 8) {
					if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
						fprintf(stderr, "%s: fill one '0:' (%d)\n", DEBUG_function_name, cnt);
					};
			   		*op++ = '0';
				} else if (cnt == 8) {
					if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
						fprintf(stderr, "%s: replace ':' by '0' (%d)\n", DEBUG_function_name, cnt);
					};
					op--;
				};
				while (cnt++ < 7) {
					if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
						fprintf(stderr, "%s: fill one ':0' (%d)\n", DEBUG_function_name, cnt);
					};
					*op++ = ':';
					*op++ = '0';
				};
			};
		};
		if (*(op-1)==':') *op++ = '0';
		*op = '\0';

		if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
			fprintf(stderr, "%s: result: %s\n", DEBUG_function_name, resultstring);
		};
	} else {
		snprintf(resultstring, NI_MAXHOST - 1, "%s", addrstring);
		if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
			fprintf(stderr, "%s: address is not in compressed format\n", DEBUG_function_name);
		};
	};

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


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
	
#define DEBUG_function_name "librfc1884/ipv6addrstruct_to_compaddr"
int librfc1884_ipv6addrstruct_to_compaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST], temp2string[NI_MAXHOST];
	int retval = 1;
	int zstart = -1, zend = -1, tstart = -1, tend = -1, i;

	if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
		fprintf(stderr, "%s: scope of IPv6 address: %08x\n", DEBUG_function_name, (unsigned int) ipv6addrp->scope);
	};
	
	if ( (ipv6addrp->scope & IPV6_ADDR_COMPATv4) != 0 ) {
		/* compatv4 address */
			
		if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
			fprintf(stderr, "%s: IPV6_ADDR_COMPATv4 type - fast conversion\n", DEBUG_function_name);
		};
	
		snprintf(tempstring, sizeof(tempstring) - 1, "::%u.%u.%u.%u", (unsigned int) ipv6addrp->in6_addr.s6_addr[12], (unsigned int) ipv6addrp->in6_addr.s6_addr[13], (unsigned int) ipv6addrp->in6_addr.s6_addr[14], (unsigned int) ipv6addrp->in6_addr.s6_addr[15]);
		retval = 0;
	} else if ( (ipv6addrp->scope & IPV6_ADDR_MAPPED) != 0 ) {
		/* mapped address */
			
		if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
			fprintf(stderr, "%s: IPV6_ADDR_MAPPED type - fast conversion\n", DEBUG_function_name);
		};
		
		snprintf(tempstring, sizeof(tempstring) - 1, "::%x:%u.%u.%u.%u", (unsigned int) ipv6addr_getword(ipv6addrp, 5), (unsigned int) ipv6addrp->in6_addr.s6_addr[12], (unsigned int) ipv6addrp->in6_addr.s6_addr[13], (unsigned int) ipv6addrp->in6_addr.s6_addr[14], (unsigned int) ipv6addrp->in6_addr.s6_addr[15]);
		retval = 0;
	} else if ( (ipv6addr_getdword(ipv6addrp, 0) == 0) && (ipv6addr_getdword(ipv6addrp, 1) == 0) && (ipv6addr_getdword(ipv6addrp, 2) == 0) && (ipv6addr_getdword(ipv6addrp, 3) == 0) ) {
		/* unspecified address */
			
		if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
			fprintf(stderr, "%s: unspecified address - fast conversion\n", DEBUG_function_name);
		};
		
		snprintf(tempstring, sizeof(tempstring) - 1, "::");
		retval = 0;
	} else if ( (ipv6addr_getdword(ipv6addrp, 0) == 0) && (ipv6addr_getdword(ipv6addrp, 1) == 0) && (ipv6addr_getdword(ipv6addrp, 2) == 0) && (ipv6addr_getdword(ipv6addrp, 3) == 1) ) {
		/* loopback address */
			
		if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
			fprintf(stderr, "%s: loopback - fast conversion\n",  DEBUG_function_name);
		};
		
		snprintf(tempstring, sizeof(tempstring) - 1, "::1");
		retval = 0;
	} else {
		/* normal address */
			
		if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
			fprintf(stderr, "%s: normal address, detect '0' blocks now\n",  DEBUG_function_name);
		};

		for ( i = 0; i <= 7; i++ ) {
			if ( ipv6addr_getword(ipv6addrp, (unsigned int) i) == 0 ) {
				/* found a '0' */
					
				if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
					fprintf(stderr, "%s: Found '0' in word '%d'\n",  DEBUG_function_name, i);
				};

				if ( tstart == -1 ) {
					/* possible starting '0' block */ 
						
					if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
						fprintf(stderr, "%s: Found a possible '0' starting block at '%d'\n",  DEBUG_function_name, i);
					};
					
					tstart = i;				
				};
			} else {
				/* end of a '0' block */
					
				if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
					fprintf(stderr, "%s: Found non '0' in word '%d'\n",  DEBUG_function_name, i);
				};

				if ( tstart != -1 ) {
					tend = i - 1;
					if ( ( tend - tstart ) > 0 ) {
						/* ok, a block with 2 or more '0' */
							
						if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
							fprintf(stderr, "%s: Found a '0' block from '%d' to '%d' with length '%d'\n",  DEBUG_function_name, tstart, tend, tend - tstart + 1);
						};
						
						if ( zstart < 0 ) {
							/* no other block before, init */
							zstart = tstart;
							zend = tend;
						
							if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
								fprintf(stderr, "%s: First found '0' block from '%d' to '%d' with length '%d'\n",  DEBUG_function_name, zstart, zend, zend - zstart + 1);
							};
							
						} else if ( ( zend - zstart ) < ( tend - tstart ) ) {
							/* ok, bigger block found */
							zstart = tstart;
							zend = tend;
							
							if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
								fprintf(stderr, "%s: Found bigger '0' block from '%d' to '%d' with length '%d'\n",  DEBUG_function_name, zstart, zend, zend - zstart + 1);
							};
							
						} else {
								
							if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
								fprintf(stderr, "%s: This '0' block is not bigger than the last one - skip\n",  DEBUG_function_name);
							};

						};
					};
					tstart = -1;
					tend = -1;
				};
			};
		};
		/* cleanup */
		if ( tstart >= 0 ) {
			tend = 7;
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

		if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
			if ( zstart != -1 ) {
				fprintf(stderr, "%s: biggest '0' block is from word '%d' to '%d'\n",  DEBUG_function_name, zstart, zend);
			} else {
				fprintf(stderr, "%s: no '0' block found\n",  DEBUG_function_name);
			};
		};

		/* create string */
		tempstring[0] = '\0';

		for ( i = 0; i <= 7; i++ ) {
			if ( i == zstart ) {
		
				if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
					fprintf(stderr, "%s: start of '0' at '%d'\n",  DEBUG_function_name, i);
				};
				
				snprintf(temp2string, sizeof(temp2string) - 1, "%s:", tempstring);
			} else if ( i == 0 ) {
				if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
					fprintf(stderr, "%s: normal start value at '%d' (%x)\n",  DEBUG_function_name, i, (unsigned int) ipv6addr_getword(ipv6addrp, (unsigned int) i));
				};
				
				snprintf(temp2string, sizeof(temp2string) - 1, "%x", (unsigned int) ipv6addr_getword(ipv6addrp, (unsigned int) i));
			} else if ( ( i > zend ) || ( i < zstart ) ) {
				snprintf(temp2string, sizeof(temp2string) - 1, "%s:%x", tempstring, (unsigned int) ipv6addr_getword(ipv6addrp, (unsigned int) i));
			} else if ( ( i == 7 ) && ( zend == i )) {
				snprintf(temp2string, sizeof(temp2string) - 1, "%s:", tempstring);
			};
			snprintf(tempstring, sizeof(tempstring) - 1, "%s", temp2string);
		};
		
		if ( (ipv6calc_debug & DEBUG_librfc1884) != 0 ) {
			fprintf(stderr, "%s: new method: '%s'\n",  DEBUG_function_name, tempstring);
		};

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

	return (retval);
};
#undef DEBUG_function_name
