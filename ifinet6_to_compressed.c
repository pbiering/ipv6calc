/*
 * ifinet6_to_compressed:
 *  Function to format an address string like shown in 
 *   Linux /proc/net/if_inet6 to a normal compressed one
 *
 * Version:		$Id: ifinet6_to_compressed.c,v 1.1 2001/10/07 14:47:40 peter Exp $
 * 
 * Author:		Peter Bieringer <pb@bieringer.de>
 *
 * some hints taken from ifconfig.c (net-tools)
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipv6calc.h"

void ifinet6_to_compressed_printhelp() {
	fprintf(stderr, " %s --ifinet6_to_compressed 32hexcharsIPv6addr [2hexcharsPrefixlength]\n", PROGRAM_NAME);
};

void ifinet6_to_compressed_printhelplong() {
	ifinet6_to_compressed_printhelp();
	fprintf(stderr, "  Converts given if_inet6 formatted IPv6 address to normal one\n");
	fprintf(stderr, "   e.g. 3ffe04000100f1010000000000000001 -> 3ffe:400:100::1\n");
	fprintf(stderr, "   e.g. 3ffe04000100f1010000000000000001 40 -> 3ffe:400:100::1/64\n\n");
};

/* function formats an given Linux /proc/net/if_inet6 format to normal one
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int ifinet6_to_compressed(char *addrstring, char *resultstring) {
	int retval = 1, result;
	char tempstring[NI_MAXHOST];
	char addr6p[8][5]; 

#ifdef DEBUG_ifinet6_to_compressed
	fprintf(stderr, "ifinet6_to_compressed: Got input %s\n", addrstring);
#endif
	
	/* simple test */
	if ( strlen(addrstring) != 32 ) {
		sprintf(resultstring, "Given hex string '%s' has not 32 chars!", addrstring);
		retval = 1;
		return (retval);
	};

	/* scan address into array */
	result = sscanf(addrstring, "%4s%4s%4s%4s%4s%4s%4s%4s\n", addr6p[0], addr6p[1], addr6p[2], addr6p[3], addr6p[4], addr6p[5], addr6p[6], addr6p[7]);
	if ( result != 8 ) {
		sprintf(resultstring, "Error splitting string %s, got only %d items!", addrstring, result);
		retval = 1;
		return (retval);
	};

	/* print out into temp string */
	sprintf(tempstring, "%s:%s:%s:%s:%s:%s:%s:%s", addr6p[0], addr6p[1], addr6p[2], addr6p[3], addr6p[4], addr6p[5], addr6p[6], addr6p[7]); 
					 
#ifdef DEBUG_ifinet6_to_compressed
	fprintf(stderr, "ifinet6_to_compressed: Filled: %s\n", tempstring);
#endif

	/* now do an compressing */
	result = addr_to_compressed(tempstring, resultstring);
	
#ifdef DEBUG_ifinet6_to_compressed
	fprintf(stderr, "ifinet6_to_compressed: Got temp result from uncompress: %s Result: %d\n", resultstring, result);
#endif

	retval = 0;
	return (retval);
}

/* function formats an given /proc/net/if_inet6 format to normal one with prefixlength
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
int ifinet6_to_compressedwithprefixlength(char *addrstring, char *prefixlengthstring, char *resultstring) {
	int retval = 1, result;
	char tempstring[NI_MAXHOST];
	unsigned int prefixlength = 0;

#ifdef DEBUG_ifinet6_to_compressed
	fprintf(stderr, "ifinet6_to_compressedwithprefixlength: Got input addressstring: '%s', prefixlengthstring: '%s'\n", addrstring, prefixlengthstring);
#endif

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
	result = ifinet6_to_compressed(addrstring, tempstring);
	if ( result != 0 ) {
		sprintf(resultstring, "%s", tempstring);
		retval = 1;
		return (retval);
	};

	/* final */
	sprintf(resultstring, "%s/%d", tempstring, prefixlength);
	
#ifdef DEBUG_ifinet6_to_compressed
	fprintf(stderr, "ifinet6_to_compressedwithprefixlength: Print: '%s'\n", resultstring);
#endif

			
	retval = 0;
	return (retval);
};
