/*
 * Project    : ipv6calc
 * File       : lib1884.c
 * Version    : $Id: librfc1884.c,v 1.2 2002/02/25 21:18:51 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 * Function libary for conversions defined in RFC 1884
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ipv6calc.h"
#include "libipv6addr.h"
#include "librfc1884.h"


/* function decompress a given IPv6 address (reverse RFC 1884)
 *  
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 *
 * Based on code in from 'ircd'
 */
#define DEBUG_function_name "librfc1884/compaddr_to_uncompaddr"
int compaddr_to_uncompaddr(char *addrstring, char *resultstring) {
	int retval = 1;
	char cnt, *cp, *op, *strp;

	if ( ipv6calc_debug & DEBUG_librfc1884 ) {
		fprintf(stderr, "%s: got input: %s\n", DEBUG_function_name, addrstring);
	};

	strp = strstr(addrstring, "::");
	if (strp) {
			
	if ( ipv6calc_debug & DEBUG_librfc1884 ) {
		fprintf(stderr, "%s: found '::' in IPv6 address\n",  DEBUG_function_name);
	};

		/* check for additional "::" occurance - not allowed! */
		if (strstr(strp+1, "::")) {
			sprintf(resultstring, "%s", "More than 2 colons in address are not allowed!");
			retval = 1;
			return (retval);
		};
		
		cnt = 0;
		cp = addrstring;
		op = resultstring;
	   	while (*cp) {
			if (*cp == ':')	cnt += 1;
			if (*cp++ == '.') {
				cnt += 1;
				break;
			};
	    };
		cp = addrstring;
		while (*cp) {
			*op++ = *cp++;
			if (*(cp-1) == ':' && *cp == ':') {
				if ((cp-1) == addrstring) {
					op--;
					*op++ = '0';
					*op++ = ':';
				};
		   		*op++ = '0';
				while (cnt++ < 7) {
					*op++ = ':';
					*op++ = '0';
				};
			};
		};
		if (*(op-1)==':') *op++ = '0';
		*op = '\0';

		if ( ipv6calc_debug & DEBUG_librfc1884 ) {
			fprintf(stderr, "%s: result: %s\n", DEBUG_function_name, resultstring);
		};
	} else {
		strcpy(resultstring, addrstring);
		if ( ipv6calc_debug & DEBUG_librfc1884 ) {
			fprintf(stderr, "%s: address is not in compressed format\n", DEBUG_function_name);
		};
	};

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/* function IPv6addrstruct to compressed format (RFC 1884)
 *
 *  compress the biggest '0' block, leading has precedence
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "librfc1884/ipv6addrstruct_to_compaddr"
int ipv6addrstruct_to_compaddr(ipv6calc_ipv6addr *ipv6addrp, char *resultstring) {
	char tempstring[NI_MAXHOST], temp2string[NI_MAXHOST];
	int retval = 1, result;
	int zstart = -1, zend = -1, tstart = -1, tend = -1, i;

	if ( ipv6addrp->scope & IPV6_ADDR_COMPATv4 ) {
		/* compatv4 address */
			
		if ( ipv6calc_debug & DEBUG_librfc1884 ) {
			fprintf(stderr, "%s: IPV6_ADDR_COMPATv4 type - fast conversion\n", DEBUG_function_name);
		};
		
		result = sprintf(tempstring, "::%u.%u.%u.%u", ipv6addrp->in6_addr.s6_addr[12], ipv6addrp->in6_addr.s6_addr[13], ipv6addrp->in6_addr.s6_addr[14], ipv6addrp->in6_addr.s6_addr[15]);
		retval = 0;
	} else if ( ipv6addrp->scope & IPV6_ADDR_MAPPED ) {
		/* mapped address */
			
		if ( ipv6calc_debug & DEBUG_librfc1884 ) {
			fprintf(stderr, "%s: IPV6_ADDR_MAPPED type - fast conversion\n", DEBUG_function_name);
		};
		
		result = sprintf(tempstring, "::%x:%u.%u.%u.%u", ipv6addr_getword(ipv6addrp, 5), ipv6addrp->in6_addr.s6_addr[12], ipv6addrp->in6_addr.s6_addr[13], ipv6addrp->in6_addr.s6_addr[14], ipv6addrp->in6_addr.s6_addr[15]);
		retval = 0;
	} else if ( (ipv6addr_getdword(ipv6addrp, 0) == 0) && (ipv6addr_getdword(ipv6addrp, 1) == 0) && (ipv6addr_getdword(ipv6addrp, 2) == 0) && (ipv6addr_getdword(ipv6addrp, 3) == 0) ) {
		/* unspecified address */
			
		if ( ipv6calc_debug & DEBUG_librfc1884 ) {
			fprintf(stderr, "%s: unspecified address - fast conversion\n", DEBUG_function_name);
		};
		
		result = sprintf(tempstring, "::");
		retval = 0;
	} else if ( (ipv6addr_getdword(ipv6addrp, 0) == 0) && (ipv6addr_getdword(ipv6addrp, 1) == 0) && (ipv6addr_getdword(ipv6addrp, 2) == 0) && (ipv6addr_getdword(ipv6addrp, 3) == 1) ) {
		/* loopback address */
			
		if ( ipv6calc_debug & DEBUG_librfc1884 ) {
			fprintf(stderr, "%s: loopback - fast conversion\n",  DEBUG_function_name);
		};
		
		result = sprintf(tempstring, "::1");
		retval = 0;
	} else {
		/* normal address */
			
		if ( ipv6calc_debug & DEBUG_librfc1884 ) {
			fprintf(stderr, "%s: normal address, detect '0' blocks now\n",  DEBUG_function_name);
		};

		for ( i = 0; i <= 7; i++ ) {
			if ( ipv6addr_getword(ipv6addrp, i) == 0 ) {
				/* found a '0' */
					
				if ( ipv6calc_debug & DEBUG_librfc1884 ) {
					fprintf(stderr, "%s: Found '0' in word '%d'\n",  DEBUG_function_name, i);
				};

				if ( tstart == -1 ) {
					/* possible starting '0' block */ 
						
					if ( ipv6calc_debug & DEBUG_librfc1884 ) {
						fprintf(stderr, "%s: Found a possible '0' starting block at '%d'\n",  DEBUG_function_name, i);
					};
					
					tstart = i;				
				};
			} else {
				/* end of a '0' block */
					
				if ( ipv6calc_debug & DEBUG_librfc1884 ) {
					fprintf(stderr, "%s: Found non '0' in word '%d'\n",  DEBUG_function_name, i);
				};

				if ( tstart != -1 ) {
					tend = i - 1;
					if ( ( tend - tstart ) > 0 ) {
						/* ok, a block with 2 or more '0' */
							
						if ( ipv6calc_debug & DEBUG_librfc1884 ) {
							fprintf(stderr, "%s: Found a '0' block from '%d' to '%d' with length '%d'\n",  DEBUG_function_name, tstart, tend, tend - tstart + 1);
						};
						
						if ( zstart < 0 ) {
							/* no other block before, init */
							zstart = tstart;
							zend = tend;
						
							if ( ipv6calc_debug & DEBUG_librfc1884 ) {
								fprintf(stderr, "%s: First found '0' block from '%d' to '%d' with length '%d'\n",  DEBUG_function_name, zstart, zend, zend - zstart + 1);
							};
							
						} else if ( ( zend - zstart ) < ( tend - tstart ) ) {
							/* ok, bigger block found */
							zstart = tstart;
							zend = tend;
							
							if ( ipv6calc_debug & DEBUG_librfc1884 ) {
								fprintf(stderr, "%s: Found bigger '0' block from '%d' to '%d' with length '%d'\n",  DEBUG_function_name, zstart, zend, zend - zstart + 1);
							};
							
						} else {
								
							if ( ipv6calc_debug & DEBUG_librfc1884 ) {
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

		if ( ipv6calc_debug & DEBUG_librfc1884 ) {
			if ( zstart != -1 ) {
				fprintf(stderr, "%s: biggest '0' block is from word '%d' to '%d'\n",  DEBUG_function_name, zstart, zend);
			} else {
				fprintf(stderr, "%s: no '0' block found\n",  DEBUG_function_name);
			};
		};

		/* create string */
		sprintf(tempstring, "%s", "");
		for ( i = 0; i <= 7; i++ ) {
			if ( i == zstart ) {
		
				if ( ipv6calc_debug & DEBUG_librfc1884 ) {
					fprintf(stderr, "%s: start of '0' at '%d'\n",  DEBUG_function_name, i);
				};
				
				sprintf(temp2string, "%s:", tempstring);
			} else if ( i == 0 ) {
				if ( ipv6calc_debug & DEBUG_librfc1884 ) {
					fprintf(stderr, "%s: normal start value at '%d' (%x)\n",  DEBUG_function_name, i, ipv6addr_getword(ipv6addrp, i));
				};
				
				sprintf(temp2string, "%x", ipv6addr_getword(ipv6addrp, i));
			} else if ( ( i > zend ) || ( i < zstart ) ) {
				sprintf(temp2string, "%s:%x", tempstring, ipv6addr_getword(ipv6addrp, i));
			} else if ( ( i == 7 ) && ( zend == i )) {
				sprintf(temp2string, "%s:", tempstring);
			};
			sprintf(tempstring, "%s", temp2string);
		};
		
		if ( ipv6calc_debug & DEBUG_librfc1884 ) {
			fprintf(stderr, "%s: new method: '%s'\n",  DEBUG_function_name, tempstring);
		};

		retval = 0;
	};

	         
	if ( ( retval == 0 ) && ( ipv6addrp->flag_prefixuse == 1 ) ) {
		sprintf(resultstring, "%s/%u", tempstring, ipv6addrp->prefixlength);
	} else {
		sprintf(resultstring, "%s", tempstring);
	};
	
	return (retval);
};
#undef DEBUG_function_name
