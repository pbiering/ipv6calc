/*
 * Project    : ipv6calc
 * File       : libipv6calc.c
 * Version    : $Id: libipv6calc.c,v 1.5 2002/02/25 22:55:54 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Function libary for conversions
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*
#if defined(__NetBSD__)
*/
/* work around incompatibilities between Linux and KAME */
/*
#define s6_addr32              __u6_addr.__u6_addr32
#define in6_u                  __u6_addr
#define u6_addr32              __u6_addr32
#include <netinet/in.h>
#else
#ifndef _NETINET_IN_H
#include <linux/ipv6.h>
#endif
#endif 
*/

#include "ipv6calc.h"
#include "libipv6addr.h"
#include "librfc1884.h"
#include "libipv6calc.h"


/* function stores an IPv6 address string into a structure
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = error message
 * out: ipv6addrp = changed IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6calc/addr_to_ipv6addrstruct"
int addr_to_ipv6addrstruct(char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1, result, i, cpoints = 0, ccolons = 0;
	char *addronlystring, *cp, tempstring[NI_MAXHOST];
	int scope = 0, expecteditems = 0;
	unsigned int temp[8];
	int compat[4];

	sprintf(resultstring, "%s", ""); /* clear result string */

	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: got input %s\n", DEBUG_function_name,  addrstring);
	};
	
	ipv6addr_clearall(ipv6addrp);

	/* save prefix length first, if available */
	ipv6addrp->flag_prefixuse = 0; /* reset flag first */
	addronlystring = strtok (addrstring, "/");
	cp = strtok (NULL, "/");
	if ( cp != NULL ) {
		i = atol(cp);
		if (i < 0 || i > 128 ) {
			sprintf(resultstring, "Illegal prefix length: '%s'", cp);
			retval = 1;
			return (retval);
		};
		ipv6addrp->flag_prefixuse = 1;
		ipv6addrp->prefixlength = i;
		
		if ( ipv6calc_debug & DEBUG_libipv6calc ) {
			fprintf(stderr, "%s: prefix length %d\n", DEBUG_function_name, ipv6addrp->prefixlength);
			fprintf(stderr, "%s: flag_prefixuse %d\n", DEBUG_function_name, ipv6addrp->flag_prefixuse);
		};
	};

	/* uncompress string, if necessary */
	if (strstr(addronlystring, "::")) {
		result = compaddr_to_uncompaddr(addronlystring, tempstring);
		if ( result != 0 ) {
			sprintf(resultstring, "%s", tempstring);
			retval = 1;
			return (retval);
		};
	} else {
		sprintf(tempstring, "%s", addronlystring);
	};

	/* count ":", must be 6 (compat) or 7 (other) */
	for (i = 0; i < strlen(tempstring); i++) {
		if (tempstring[i] == ':') {
			ccolons++;
		};
		if (tempstring[i] == '.') {
			cpoints++;
		};
	};
	if ( ! ( ( ( ccolons == 7 ) && ( cpoints == 0 ) ) ||  ( ( ccolons == 6 ) && ( cpoints == 3 ) ) ) ) {
		if (strstr(addronlystring, "::")) {
			sprintf(resultstring, "Error, given address expanded to '%s' is not valid!", tempstring);
		} else {
			sprintf(resultstring, "Error, given address '%s' is not valid!", addronlystring);
		};
		retval = 1;
		return (retval);
	};

	/* clear variables */
	for ( i = 0; i <= 7; i++ ) {
		compat[i] = 0;
	};

	ipv6addr_clear(ipv6addrp);
	
	if ( ccolons == 6 ) {
		/* compatv4/mapped format */
		expecteditems = 10;
		result = sscanf(tempstring, "%x:%x:%x:%x:%x:%x:%d.%d.%d.%d", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5], &compat[0], &compat[1], &compat[2], &compat[3]);
		/* check compat */
		for ( i = 0; i <= 3; i++ ) {
			if ( ( compat[i] < 0 ) || ( compat[i] > 255 ) )	{
				sprintf(resultstring, "Error, given compatv4/mapped address '%s' is not valid (%d)!", addronlystring, compat[i]);
				retval = 1;
				return (retval);
			};
		};
		temp[6] = ( compat[0] << 8 ) | compat[1];
		temp[7] = ( compat[2] << 8 ) | compat[3];
		scope = IPV6_ADDR_COMPATv4;
	} else {
		/* normal format */
		expecteditems = 8;
		result = sscanf(tempstring, "%x:%x:%x:%x:%x:%x:%x:%x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5], &temp[6], &temp[7]);
	};
	
	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: reading into array, got items: %d\n", DEBUG_function_name, result);
	};

	if ( result != expecteditems ) {
		sprintf(resultstring, "Error splitting address %s, got %d items instead of %d!", addronlystring, result, expecteditems);
		retval = 1;
		return (retval);
	};

	/* check address words range */
	for ( i = 0; i <= 7; i++ ) {
		if ( ( temp[i] < 0x0 ) || ( temp[i] > 0xffff ) )	{
			sprintf(resultstring, "Error, given address '%s' is not valid on position %d (%x)!", addronlystring, i, temp[i]);
			retval = 1;
			return (retval);
		};
	};
	
	/* copy into structure */
	for ( i = 0; i <= 7; i++ ) {
		ipv6addr_setword(ipv6addrp, i, temp[i]);
	};

	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: In structure %08x %08x %08x %08x\n", DEBUG_function_name, ipv6addr_getdword(ipv6addrp, 0), ipv6addr_getdword(ipv6addrp, 1), ipv6addr_getdword(ipv6addrp, 2), ipv6addr_getdword(ipv6addrp, 3));
		fprintf(stderr, "%s: In structure %04x %04x %04x %04x %04x %04x %04x %04x\n", DEBUG_function_name, ipv6addr_getword(ipv6addrp, 0), ipv6addr_getword(ipv6addrp, 1), ipv6addr_getword(ipv6addrp, 2), ipv6addr_getword(ipv6addrp, 3), ipv6addr_getword(ipv6addrp, 4), ipv6addr_getword(ipv6addrp, 5), ipv6addr_getword(ipv6addrp, 6), ipv6addr_getword(ipv6addrp, 7));
	};
	
	result = ipv6addr_gettype(ipv6addrp); 

	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: Got scope %02x\n", DEBUG_function_name, result);
	};

	ipv6addrp->scope = result;

	if ( scope != 0 ) {
		/* test, whether compatv4/mapped/ISATAP is really one */
		if ( ! ( ipv6addrp->scope & IPV6_ADDR_COMPATv4 || ipv6addrp->scope & IPV6_ADDR_MAPPED || ipv6addrp->scope & IPV6_NEW_ADDR_ISATAP) ) {
			sprintf(resultstring, "Error, given address '%s' is not valid compatv4/mapped/ISATAP one!", addronlystring);
			retval = 1;
			return (retval);
		};
	};

	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: First word is: %x, address info value: %x\n", DEBUG_function_name, ipv6addr_getword(ipv6addrp, 0), result);
		fprintf(stderr, "%s: flag_prefixuse %d\n", DEBUG_function_name, ipv6addrp->flag_prefixuse);
	};
	
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/* function stores the ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6calc/ipv6addrstruct_to_uncompaddr"
int ipv6addrstruct_to_uncompaddr(ipv6calc_ipv6addr *ipv6addrp, char *resultstring) {
	int retval = 1, result;
	char tempstring[NI_MAXHOST];

	/* print array */
	if ( ( ipv6addrp->scope & IPV6_ADDR_COMPATv4 ) || ( ipv6addrp->scope & IPV6_ADDR_MAPPED ) ) {
		result = sprintf(tempstring, "%x:%x:%x:%x:%x:%x:%u.%u.%u.%u", ipv6addr_getword(ipv6addrp, 0), ipv6addr_getword(ipv6addrp, 1), ipv6addr_getword(ipv6addrp, 2), ipv6addr_getword(ipv6addrp, 3), ipv6addr_getword(ipv6addrp, 4), ipv6addr_getword(ipv6addrp, 5), ipv6addrp->in6_addr.s6_addr[12], ipv6addrp->in6_addr.s6_addr[13], ipv6addrp->in6_addr.s6_addr[14], ipv6addrp->in6_addr.s6_addr[15]);
	} else {
		result = sprintf(tempstring, "%x:%x:%x:%x:%x:%x:%x:%x", ipv6addr_getword(ipv6addrp, 0), ipv6addr_getword(ipv6addrp, 1), ipv6addr_getword(ipv6addrp, 2), ipv6addr_getword(ipv6addrp, 3), ipv6addr_getword(ipv6addrp, 4), ipv6addr_getword(ipv6addrp, 5), ipv6addr_getword(ipv6addrp, 6), ipv6addr_getword(ipv6addrp, 7));
	};

	if (ipv6addrp->flag_prefixuse == 1) {
		/* append prefix length */
		result = sprintf(resultstring, "%s/%u", tempstring, ipv6addrp->prefixlength);
	} else {
		result = sprintf(resultstring, "%s", tempstring);
	};

	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: result string: %s\n", DEBUG_function_name, resultstring);
	};
	
	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name


/* function stores the prefix of an ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6calc/ipv6addrstruct_to_uncompaddrsuffix"
int ipv6addrstruct_to_uncompaddrprefix(ipv6calc_ipv6addr *ipv6addrp, char *resultstring) {
	int retval = 1;
	int max, i;
	char tempstring1[NI_MAXHOST], tempstring2[NI_MAXHOST];

	/* test for misuse */
	if ( ( ( ipv6addrp->scope & IPV6_ADDR_COMPATv4 ) || ( ipv6addrp->scope & IPV6_ADDR_MAPPED ) ) && ( ipv6addrp->prefixlength > 96 ) ) {
		sprintf(resultstring, "Error, cannot print prefix of a compatv4/mapped address with prefix length bigger than 96!");
		retval = 1;
		return (retval);
	};
	if ( ipv6addrp->prefixlength == 0 ) {
		sprintf(resultstring, "Error, cannot print prefix of a address with prefix length 0!");
		retval = 1;
		return (retval);
	};

	max = ( ipv6addrp->prefixlength - 1 ) >> 4;
	i = 0;
	sprintf(tempstring1, "%s", "");
	while (i <= max ) {
		if ( i < max ) {
			sprintf(tempstring2, "%s%x:", tempstring1, ipv6addr_getword(ipv6addrp, i));
		} else {
			sprintf(tempstring2, "%s%x", tempstring1, ipv6addr_getword(ipv6addrp, i));
		};
		i++;
		sprintf(tempstring1, "%s", tempstring2);
	};
	sprintf(resultstring, "%s", tempstring1);

	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: result string: %s\n", DEBUG_function_name, resultstring);
	};

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/* function stores the suffix of an ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6calc/ipv6addrstruct_to_uncompaddrsuffix"
int ipv6addrstruct_to_uncompaddrsuffix(ipv6calc_ipv6addr *ipv6addrp, char *resultstring) {
	int retval = 1;
	int max, i;
	char tempstring1[NI_MAXHOST], tempstring2[NI_MAXHOST];

	/* test for misuse */
	if ( ( ( ipv6addrp->scope & IPV6_ADDR_COMPATv4 ) || ( ipv6addrp->scope & IPV6_ADDR_MAPPED ) ) && ( ipv6addrp->prefixlength > 96 ) ) {
		sprintf(resultstring, "Error, cannot print suffix of a compatv4/mapped address with prefix length bigger than 96!");
		retval = 1;
		return (retval);
	};
	if ( ipv6addrp->prefixlength == 128 ) {
		sprintf(resultstring, "Error, cannot print suffix of a address with prefix length 128!");
		retval = 1;
		return (retval);
	};

	max = 7;
	i   = ipv6addrp->prefixlength >> 4;
	sprintf(tempstring1, "%s", "");
	while (i <= max ) {
		if ( ( ( ipv6addrp->scope & IPV6_ADDR_COMPATv4 ) || ( ipv6addrp->scope & IPV6_ADDR_MAPPED ) ) && ( i == 6 ) ) {
			sprintf(tempstring2, "%s%u.%u.%u.%u", tempstring1, ipv6addrp->in6_addr.s6_addr[12], ipv6addrp->in6_addr.s6_addr[13], ipv6addrp->in6_addr.s6_addr[14], ipv6addrp->in6_addr.s6_addr[15]);
			i = max;
		} else if ( i < max ) {
			sprintf(tempstring2, "%s%x:", tempstring1, ipv6addr_getword(ipv6addrp, i));
		} else {
			sprintf(tempstring2, "%s%x", tempstring1, ipv6addr_getword(ipv6addrp, i));
		};
		i++;
		sprintf(tempstring1, "%s", tempstring2);
	};
	sprintf(resultstring, "%s", tempstring1);

	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: result string: %s\n", DEBUG_function_name, resultstring);
	};
	
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/* function stores the ipv6addr structure in an full uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6calc/ipv6addrstruct_to_fulluncompaddr"
int ipv6addrstruct_to_fulluncompaddr(ipv6calc_ipv6addr *ipv6addrp, char *resultstring) {
	int retval = 1, result;
	char tempstring[NI_MAXHOST];

	/* print array */
	if ( ( ipv6addrp->scope & IPV6_ADDR_COMPATv4 ) || ( ipv6addrp->scope & IPV6_ADDR_MAPPED ) ) {
		result = sprintf(tempstring, "%04x:%04x:%04x:%04x:%04x:%04x:%u.%u.%u.%u", ipv6addr_getword(ipv6addrp, 0), ipv6addr_getword(ipv6addrp, 1), ipv6addr_getword(ipv6addrp, 2), ipv6addr_getword(ipv6addrp, 3), ipv6addr_getword(ipv6addrp, 4), ipv6addr_getword(ipv6addrp, 5), ipv6addrp->in6_addr.s6_addr[12], ipv6addrp->in6_addr.s6_addr[13], ipv6addrp->in6_addr.s6_addr[14], ipv6addrp->in6_addr.s6_addr[15]);
	} else {
		result = sprintf(tempstring, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x", ipv6addr_getword(ipv6addrp, 0), ipv6addr_getword(ipv6addrp, 1), ipv6addr_getword(ipv6addrp, 2), ipv6addr_getword(ipv6addrp, 3), ipv6addr_getword(ipv6addrp, 4), ipv6addr_getword(ipv6addrp, 5), ipv6addr_getword(ipv6addrp, 6), ipv6addr_getword(ipv6addrp, 7));
	};

	if (ipv6addrp->flag_prefixuse == 1) {
		/* append prefix length */
		result = sprintf(resultstring, "%s/%u", tempstring, ipv6addrp->prefixlength);
	} else {
		result = sprintf(resultstring, "%s", tempstring);
	};

	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: result string: %s\n", DEBUG_function_name, resultstring);
	};
	
	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name


/* mask prefix bits (set suffix bits to 0)
 * 
 * in:  structure via reference
 * out: modified structure
 */
#define DEBUG_function_name "libipv6calc/ipv6addrstruct_masksuffix"
void ipv6addrstruct_maskprefix(ipv6calc_ipv6addr *ipv6addrp) {
	int nbit, nword;
	unsigned int mask, newword;

	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};
   
	if (ipv6addrp->flag_prefixuse != 1) {
		/* hmm, no prefix specified. skip */
		return;
	};

	for (nbit = 127; nbit >= 0; nbit--) {
		if (nbit >= ipv6addrp->prefixlength) {
			/* set bit to zero */
			
			/* calculate word (16 bit) - matches with addr6p[]*/
			nword = (nbit & 0x70) >> 4;
				 
			/* calculate mask */
			mask = 0x8000 >> ((nbit & 0x0f));
			newword = ipv6addr_getword(ipv6addrp, nword) & (~ mask );
			
			if ( ipv6calc_debug & DEBUG_libipv6calc ) {
				fprintf(stderr, "%s: bit: %d = nword: %d, mask: %04x, word: %04x newword: %04x\n", DEBUG_function_name, nbit, nword, mask, ipv6addr_getword(ipv6addrp, nword), newword);
			};

			ipv6addr_setword(ipv6addrp, nword, newword);
		};
	};
};
#undef DEBUG_function_name


/* mask suffix bits (set prefix bits to 0) 
 *
 * in:  structure via reference
 * out: modified structure
 */
#define DEBUG_function_name "libipv6calc/ipv6addrstruct_masksuffix"
void ipv6addrstruct_masksuffix(ipv6calc_ipv6addr *ipv6addrp) {
	int nbit, nword;
	unsigned int mask, newword;

	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};
   
	if (ipv6addrp->flag_prefixuse != 1) {
		/* hmm, no prefix specified. skip */
		return;
	};

	for (nbit = 127; nbit >= 0; nbit--) {
		if (nbit < ipv6addrp->prefixlength) {
			/* set bit to zero */
			
			/* calculate word (16 bit) - matches with addr6p[]*/
			nword = (nbit & 0x70) >> 4;
				 
			/* calculate mask */
			mask = 0x8000 >> ((nbit & 0x0f));
			newword = ipv6addr_getword(ipv6addrp, nword) & (~ mask );
			
			if ( ipv6calc_debug & DEBUG_libipv6calc ) {
				fprintf(stderr, "libipv6calc/ipv6calc_ipv6addr_masksuffix: bit: %d = nword: %d, mask: %04x, word: %04x newword: %04x\n", nbit, nword, mask, ipv6addr_getword(ipv6addrp, nword), newword);
			};

			ipv6addr_setword(ipv6addrp, nword, newword);
		};
	};
};
#undef DEBUG_function_name


/* function stores an 16 char identifier/token into a structure
 *
 * in : *addrstring = 16 char identifier/token
 * out: *resultstring = error message
 * out: ipv6addr = IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6calc/identifier_to_ipv6addrstruct"
int identifier_to_ipv6addrstruct(char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1, result, i;
	unsigned int temp[8];

	sprintf(resultstring, "%s", ""); /* clear result string */

	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: got input '%s'\n", DEBUG_function_name, addrstring);
	};
	
	if ( strlen(addrstring) != 16 ) {
		sprintf(resultstring, "Error, given identifier/token '%s' is not valid (length != 16!", addrstring);
		retval = 1;
		return (retval);
	};

	/* clear variables */
	ipv6addr_clear(ipv6addrp);

	/* scan address into array */
	result = sscanf(addrstring, "%04x%04x%04x%04x", &temp[4], &temp[5], &temp[6], &temp[7]);
	if ( result != 4 ) {
		sprintf(resultstring, "Error splitting address '%s', got %d items instead of 4!", addrstring, result);
		retval = 1;
		return (retval);
	};
	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: reading into array, got items: %d\n", DEBUG_function_name, result);
	};

	/* check address words range */
	for ( i = 4; i <= 7; i++ ) {
		if ( ( temp[i] < 0x0 ) || ( temp[i] > 0xffff ) )	{
			sprintf(resultstring, "Error, given address '%s' is not valid on position %d (%x)!", addrstring, i, temp[i]);
			retval = 1;
			return (retval);
		};
	};
	
	/* copy into structure */
	ipv6addr_setword(ipv6addrp, 4, temp[4]);
	ipv6addr_setword(ipv6addrp, 5, temp[5]);
	ipv6addr_setword(ipv6addrp, 6, temp[6]);
	ipv6addr_setword(ipv6addrp, 7, temp[7]);
	
	if ( ipv6calc_debug & DEBUG_libipv6calc ) {
		fprintf(stderr, "%s: Got value '%04x%04x%04x%04x'\n", DEBUG_function_name, temp[4], temp[5], temp[6], temp[7]);
	};

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * function converts chars in a string to upcase
 * in : pointer to a string
 */
#define DEBUG_function_name "libipv6calc/string_to_upcase"
void string_to_upcase(char *string) {
	int i;

	if (strlen(string) > 0) {
		for (i = 0; i < strlen(string); i++) {
			string[i] = toupper(string[i]);
		};
	};

	return;
};
#undef DEBUG_function_name
