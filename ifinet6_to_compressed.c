/*
 * Project    : ipv6calc
 * File       : ifinet6_to_compressed.c
 * Version    : $Id: ifinet6_to_compressed.c,v 1.3 2002/02/25 21:18:50 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function to format an address string like shown in 
 *   Linux /proc/net/if_inet6 to a normal compressed one
 *
 * some hints taken from ifconfig.c (net-tools)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipv6calc.h"
#include "libhelp.h"
#include "addr_to_compressed.h"
#include "libipv6calc.h"

void ifinet6_to_compressed_printhelp(void) {
	fprintf(stderr, " %s --ifinet6_to_compressed [<additional options>] <32hexcharsIPv6addr> [<2hexcharsPrefixlength>]\n", PROGRAM_NAME);
};

void ifinet6_to_compressed_printhelplong(void) {
	ifinet6_to_compressed_printhelp();
	fprintf(stderr, "  Converts given if_inet6 formatted IPv6 address to normal one, e.g.\n");
	fprintf(stderr, "   3ffeffff0100f1010000000000000001 -> 3ffe:ffff:100::1\n");
	fprintf(stderr, "   3ffeffff0100f1010000000000000001 40 -> 3ffe:ffff:100::1/64\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  Additional usable options:\n");
	printhelp_case();
	fprintf(stderr, "\n");
};

/*
 * function formats an given Linux /proc/net/if_inet6 format to normal one
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int ifinet6_to_compressed(char *addrstring, char *resultstring, unsigned long command) {
	int retval = 1;
	char tempstring[NI_MAXHOST];
	char addr6p[8][5]; 
	ipv6calc_ipv6addr ipv6addr;

	if (ipv6calc_debug & DEBUG_ifinet6_to_compressed) {
		fprintf(stderr, "ifinet6_to_compressed: Got input %s\n", addrstring);
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
					 
	if (ipv6calc_debug & DEBUG_ifinet6_to_compressed) {
		fprintf(stderr, "ifinet6_to_compressed: Filled: %s\n", tempstring);
	};

	/* fill structure */
	retval = addr_to_ipv6addrstruct(tempstring, resultstring, &ipv6addr);

	if (retval != 0) {
		return (retval);
	};

	/* now do an compressing */
	retval = addr_to_compressed(&ipv6addr, resultstring, command);
	
	if (ipv6calc_debug & DEBUG_ifinet6_to_compressed) {
		fprintf(stderr, "ifinet6_to_compressed: Got temp result from uncompress: %s Result: %d\n", resultstring, retval);
	};

	return (retval);
}

/* function formats an given /proc/net/if_inet6 format to normal one with prefixlength
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int ifinet6_to_compressedwithprefixlength(char *addrstring, char *prefixlengthstring, char *resultstring, unsigned long command) {
	int retval = 1, result;
	char tempstring[NI_MAXHOST];
	unsigned int prefixlength = 0;

	if (ipv6calc_debug & DEBUG_ifinet6_to_compressed) {
		fprintf(stderr, "ifinet6_to_compressedwithprefixlength: Got input addressstring: '%s', prefixlengthstring: '%s'\n", addrstring, prefixlengthstring);
	};

	/* simple test on prefix length string*/
	if ( strlen(prefixlengthstring) != 2 ) {
		sprintf(resultstring, "Given prefixlength hex string '%s' has not 2 chars!", addrstring);
		retval = 1;
		return (retval);
	};

	/* scan prefix length */
	result = sscanf(prefixlengthstring, "%2x\n", &prefixlength);
	if ( result != 1 ) {
		sprintf(resultstring, "error splitting string %s, got only %d items!", addrstring, result);
		retval = 1;
		return (retval);
	};
	
	if ( ( prefixlength < 0 ) || ( prefixlength > 128 ) ) {
		sprintf(resultstring, "decimal prefixlength '%d' out of range!", prefixlength);
		retval = 1;
		return (retval);
	}

	/* convert address */
	result = ifinet6_to_compressed(addrstring, tempstring, command);
	if ( result != 0 ) {
		sprintf(resultstring, "%s", tempstring);
		retval = 1;
		return (retval);
	};

	/* final */
	sprintf(resultstring, "%s/%d", tempstring, prefixlength);
	
	if (ipv6calc_debug & DEBUG_ifinet6_to_compressed) {
		fprintf(stderr, "ifinet6_to_compressedwithprefixlength: Print: '%s'\n", resultstring);
	};
			
	retval = 0;
	return (retval);
};
