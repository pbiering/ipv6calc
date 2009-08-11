/*
 * Project    : ipv6calc
 * File       : libifinet6.c
 * Version    : $Id: libifinet6.c,v 1.6 2009/08/11 20:38:51 peter Exp $
 * Copyright  : 2001-2009 by Peter Bieringer <pb (at) bieringer.de>
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
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
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

	if ( (ipv6calc_debug & DEBUG_libifinet6) != 0 ) {
		fprintf(stderr, "%s: Got input '%s'\n", DEBUG_function_name, addrstring);
	};
	
	/* simple test */
	if ( strlen(addrstring) != 32 ) {
		snprintf(resultstring,NI_MAXHOST - 1, "Error in given hex string, has not 32 chars!");
		retval = 1;
		return (retval);
	};

	/* scan address into array */
	retval = sscanf(addrstring, "%4s%4s%4s%4s%4s%4s%4s%4s\n", addr6p[0], addr6p[1], addr6p[2], addr6p[3], addr6p[4], addr6p[5], addr6p[6], addr6p[7]);
	if ( retval != 8 ) {
		snprintf(resultstring,NI_MAXHOST - 1, "Error splitting string %s, got only %d items!", addrstring, retval);
		retval = 1;
		return (retval);
	};

	/* print out into temp string */
	snprintf(tempstring, sizeof(tempstring) - 1, "%s:%s:%s:%s:%s:%s:%s:%s", addr6p[0], addr6p[1], addr6p[2], addr6p[3], addr6p[4], addr6p[5], addr6p[6], addr6p[7]); 
					 
	if ( (ipv6calc_debug & DEBUG_libifinet6) != 0 ) {
		fprintf(stderr, "%s: Filled: %s\n", DEBUG_function_name, tempstring);
	};

	/* fill structure */
	retval = addr_to_ipv6addrstruct(tempstring, resultstring, ipv6addrp);

	return (retval);
};
#undef DEBUG_function_name

/*
 * function formats an given /proc/net/if_inet6 format to normal one with prefixlength
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libifinet6/ifinet6_withprefixlength_to_ipv6addrstruct"
int libifinet6_ifinet6_withprefixlength_to_ipv6addrstruct(char *addrstring, char *prefixlengthstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1, result, tempint;
	char tempstring[NI_MAXHOST];
	uint8_t prefixlength = 0;

	if ( (ipv6calc_debug & DEBUG_libifinet6) != 0 ) {
		fprintf(stderr, "%s: Got input addressstring: '%s', prefixlengthstring: '%s'\n", DEBUG_function_name, addrstring, prefixlengthstring);
	};

	/* simple test on prefix length string*/
	if ( strlen(prefixlengthstring) != 2 ) {
		snprintf(resultstring,NI_MAXHOST - 1, "Given prefixlength hex string '%s' has not 2 chars!", prefixlengthstring);
		retval = 1;
		return (retval);
	};

	/* scan prefix length */
	result = sscanf(prefixlengthstring, "%2x\n", &tempint);
	if ( result != 1 ) {
		snprintf(resultstring,NI_MAXHOST - 1, "error splitting string %s, got only %d items!", prefixlengthstring, result);
		retval = 1;
		return (retval);
	};
	
	if ( (tempint < 0) || (tempint > 128) ) {
		snprintf(resultstring,NI_MAXHOST - 1, "decimal prefixlength '%d' out of range!", tempint);
		retval = 1;
		return (retval);
	};
	prefixlength = (uint8_t) tempint;

	/* convert plain address */
	result = libifinet6_ifinet6_to_ipv6addrstruct(addrstring, tempstring, ipv6addrp);
	if ( result != 0 ) {
		snprintf(resultstring,NI_MAXHOST - 1, "%s", tempstring);
		retval = 1;
		return (retval);
	};

	/* set prefix length */
	ipv6addrp->prefixlength = (uint8_t) prefixlength;
	ipv6addrp->flag_prefixuse = 1;
	
	if ( (ipv6calc_debug & DEBUG_libifinet6) != 0 ) {
		fprintf(stderr, "%s: Print: '%s'\n", DEBUG_function_name, resultstring);
	};
			
	retval = 0;
	return (retval);
};


/*
 * function formats an given IPv6 address to Linux /proc/net/if_inet6 format
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int libifinet6_ipv6addrstruct_to_ifinet6(ipv6calc_ipv6addr *ipv6addrp, char *resultstring) {
	int retval = 1;
	char tempstring[NI_MAXHOST];

	/* print out array */	
	snprintf(tempstring, sizeof(tempstring) - 1, "%08x%08x%08x%08x %02x", (unsigned int) ipv6addr_getdword(ipv6addrp, 0), (unsigned int) ipv6addr_getdword(ipv6addrp, 1), (unsigned int) ipv6addr_getdword(ipv6addrp, 2), (unsigned int) ipv6addr_getdword(ipv6addrp, 3), (unsigned int) (*ipv6addrp).scope & IPV6_ADDR_SCOPE_MASK);
	
	if ( (*ipv6addrp).flag_prefixuse == 1 ) {
		snprintf(resultstring,NI_MAXHOST - 1, "%s %02x", tempstring, (unsigned int) (*ipv6addrp).prefixlength);
	} else {
		snprintf(resultstring,NI_MAXHOST - 1, "%s", tempstring);
	};

	if ( (ipv6calc_debug & DEBUG_libifinet6) != 0 ) {
		fprintf(stderr, "addr_to_ifinet6: Print out: %s\n", resultstring);
	};

	retval = 0;
	return (retval);
};
