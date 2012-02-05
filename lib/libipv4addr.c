/*
 * Project    : ipv6calc/lib
 * File       : libipv4addr.c
 * Version    : $Id: libipv4addr.c,v 1.28 2012/02/05 09:03:34 peter Exp $
 * Copyright  : 2002-2012 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
 *
 * Information:
 *  Function library for IPv4 address handling
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "libipv4addr.h"
#include "ipv6calctypes.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"

#ifdef SUPPORT_DB_IPV4
#include "../databases/ipv4-assignment/dbipv4addr_assignment.h"
#endif


/*
 * function returns an octet of an IPv4 address
 *
 * in: ipv4addrp = pointer to IPv4 address structure
 * in: numoctet  = number of octet (0 = MSB, 3 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_getoctet"
uint8_t ipv4addr_getoctet(const ipv6calc_ipv4addr *ipv4addrp, const unsigned int numoctet) {
	uint8_t retval;
	
	if ( numoctet > 3 ) {
		fprintf(stderr, "%s: given ocett number '%u' is out of range!\n", DEBUG_function_name, numoctet);
		exit(EXIT_FAILURE);
	};

	retval = (uint8_t) ( (ipv4addrp->in_addr.s_addr >> ( (unsigned int) (3 - numoctet) << 3)) & 0xff );

	return (retval);
};
#undef DEBUG_function_name


/*
 * function returns a word of an IPv4 address
 *
 * in: ipv6addrp = pointer to IPv4 address structure
 * in: numword   = number of word (0 = MSB, 1 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_getword"
uint16_t ipv4addr_getword(const ipv6calc_ipv4addr *ipv4addrp, const unsigned int numword) {
	uint16_t retval;
	
	if ( numword > 1 ) {
		fprintf(stderr, "%s: given word number '%u' is out of range!\n", DEBUG_function_name, numword);
		exit(EXIT_FAILURE);
	};

	retval = (uint16_t) ( (ipv4addr_getoctet(ipv4addrp, (numword << 1)) << 8 ) | ipv4addr_getoctet(ipv4addrp, (numword << 1) + 1) );

	return (retval);
};
#undef DEBUG_function_name


/*
 * function returns a dword of an IPv4 address
 *
 * in: ipv4addrp = pointer to IPv4 address structure
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_getdword"
uint32_t ipv4addr_getdword(const ipv6calc_ipv4addr *ipv4addrp) {
	uint32_t retval;
	
	retval = (uint32_t) ( (ipv4addr_getword(ipv4addrp, 0) << 16) | ipv4addr_getword(ipv4addrp, 1) );

	return (retval);
};
#undef DEBUG_function_name


/*
 * function sets an octet of an IPv4 address
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 * in: numoctet   = number of word (0 = MSB, 3 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_setoctet"
void ipv4addr_setoctet(ipv6calc_ipv4addr *ipv4addrp, const unsigned int numoctet, const unsigned int value) {
	
	if ( numoctet > 3 ) {
		fprintf(stderr, "%s: given octet number '%u' is out of range!\n", DEBUG_function_name, numoctet);
		exit(EXIT_FAILURE);
	};
	
	if ( value > 0x0000000ff ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(EXIT_FAILURE);
	}; 

	ipv4addrp->in_addr.s_addr &= ~ (0xff << ((unsigned int) (3 - numoctet) << 3) );
	ipv4addrp->in_addr.s_addr |= (value & 0xff) << ((unsigned int) (3 - numoctet) << 3);

	return;
};
#undef DEBUG_function_name


/*
 * function sets a word of an IPv4 address
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 * in: numword   = number of word (0 = MSB, 1 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_setword"
void ipv4addr_setword(ipv6calc_ipv4addr *ipv4addrp, const unsigned int numword, const unsigned int value) {
	unsigned int n;
	unsigned int v;
	
	if ( numword > 1 ) {
		fprintf(stderr, "%s: given word number '%u' is out of range!\n", DEBUG_function_name, numword);
		exit(EXIT_FAILURE);
	};
	
	if ( value > 0x0000ffff ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(EXIT_FAILURE);
	}; 

	n = numword << 1;
	v = (value & 0xff00) >> 8;

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: set octet %u: %02x (%u %04x)\n", DEBUG_function_name, n, v, numword, value);
	};

	ipv4addr_setoctet(ipv4addrp, n, v);

	n = (numword << 1) + 1;
	v = value & 0xff;

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: set octet %u: %02x (%u %04x)\n", DEBUG_function_name, n, v, numword, value);
	};

	ipv4addr_setoctet(ipv4addrp, n, v);
	
	return;
};
#undef DEBUG_function_name


/*
 * function sets a dword of an IPv4 address
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_setdword"
void ipv4addr_setdword(ipv6calc_ipv4addr *ipv4addrp, const unsigned int value) {
	unsigned int n;
	unsigned int v;
	
	if ( value > 0xffffffffu ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(EXIT_FAILURE);
	}; 

	n = 0;
	v = (value & 0xffff0000u) >> 16;

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: set word %u: %04x (%08x)\n", DEBUG_function_name, n, v, value);
	};

	ipv4addr_setword(ipv4addrp, n, v);

	n = 1;
	v = value & 0xffffu;

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: set word %u: %04x (%08x)\n", DEBUG_function_name, n, v, value);
	};

	ipv4addr_setword(ipv4addrp, n, v);

	return;
};
#undef DEBUG_function_name


/*
 * function clears the IPv4 structure
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_clear"
void ipv4addr_clear(ipv6calc_ipv4addr *ipv4addrp) {
	ipv4addrp->in_addr.s_addr = 0;

	/* Clear valid flag */
	ipv4addrp->flag_valid = 0;

	return;
};
#undef DEBUG_function_name


/*
 * function clears the IPv4 structure
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 */
#define DEBUG_function_name "libipv4addr/ipv4addr_clearall"
void ipv4addr_clearall(ipv6calc_ipv4addr *ipv4addrp) {
	ipv4addr_clear(ipv4addrp);
	(*ipv4addrp).prefixlength = 0;
	(*ipv4addrp).flag_prefixuse = 0;

	return;
};
#undef DEBUG_function_name


/*
 * function gets type of an IPv4 address
 *
 *   to be implemented...
 */
uint32_t ipv4addr_gettype(/*@unused@*/ const ipv6calc_ipv4addr *ipv4addrp) {
	return (0);
};


/*
 * function stores an IPv4 address string into a structure
 *
 * in : *addrstring = IPv4 address
 * out: *resultstring = error message
 * out: ipv4addrp = changed IPv4 address structure
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv4addr/addr_to_ipv4addrstruct"
int addr_to_ipv4addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv4addr *ipv4addrp) {
	int retval = 1, result, i, cpoints = 0, cdigits = 0;
	char *addronlystring, *cp;
	int expecteditems = 0;
	int compat[4];
	char tempstring[NI_MAXHOST], *cptr, **ptrptr;
	uint32_t typeinfo;

	ptrptr = &cptr;

	resultstring[0] = '\0'; /* clear result string */

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: Got input '%s'\n", DEBUG_function_name,  addrstring);
	};

	if ((strlen(addrstring) < 7) || (strlen(addrstring) > 18)) {
		/* min: 0.0.0.0 */
		/* max: 123.123.123.123/32 */
		snprintf(resultstring, NI_MAXHOST - 1, "Error in given dot notated IPv4 address, has not 7 to 18 chars!");
		return (1);
	};

	if (strlen(addrstring) > sizeof(tempstring) - 1) {
		fprintf(stderr, "%s: Input too long: %s\n", DEBUG_function_name, addrstring);
		return (1);
	};

	strncpy(tempstring, addrstring, sizeof(tempstring) - 1);
	
	ipv4addr_clearall(ipv4addrp);

	/* save prefix length first, if available */
	addronlystring = strtok_r(tempstring, "/", ptrptr);

	if ( addronlystring == NULL ) {
		fprintf(stderr, "Strange input: %s\n", addrstring);
		return (1);
	};

	cp = strtok_r(NULL, "/", ptrptr);
	if ( cp != NULL ) {
		i = atoi(cp);
		if (i < 0 || i > 32 ) {
			snprintf(resultstring, NI_MAXHOST - 1, "Illegal prefix length: '%s'", cp);
			retval = 1;
			return (retval);
		};
		ipv4addrp->flag_prefixuse = 1;
		ipv4addrp->prefixlength = (uint8_t) i;
		
		if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
			fprintf(stderr, "%s: prefix length %u\n", DEBUG_function_name, (unsigned int) ipv4addrp->prefixlength);
			fprintf(stderr, "%s: flag_prefixuse %d\n", DEBUG_function_name, ipv4addrp->flag_prefixuse);
		};
	};

	/* count "." and digits */
	for (i = 0; i < (int) strlen(addronlystring); i++) {
		if (addronlystring[i] == '.') {
			cpoints++;
		};
		if (isdigit(addronlystring[i])) {
			cdigits++;
		};
	};

	/* check amount of ".", must be 3 */
	if ( cpoints != 3 ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error, given IPv4 address '%s' is not valid (only %d dots)!", addronlystring, cpoints);
		retval = 1;
		return (retval);
	};

	/* amount of "." and digits must be length */
	if (cdigits + cpoints != strlen(addronlystring)) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error, given IPv4 address '%s' is not valid!", addronlystring);
		retval = 1;
		return (retval);
	};

	/* clear variables */
	for ( i = 0; i <= 3; i++ ) {
		compat[i] = 0;
	};

	expecteditems = 4;
	result = sscanf(addronlystring, "%d.%d.%d.%d", &compat[0], &compat[1], &compat[2], &compat[3]);
	
	for ( i = 0; i <= 3; i++ ) {
		if ( ( compat[i] < 0 ) || ( compat[i] > 255 ) )	{
			snprintf(resultstring, NI_MAXHOST - 1, "Error, given IPv4 address '%s' is not valid (%d on position %d)!", addronlystring, compat[i], i+1);
			retval = 1;
			return (retval);
		};
	};
	
	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: reading into array, got items: %d\n", DEBUG_function_name, result);
	};

	if ( result != expecteditems ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error splitting address %s, got %d items instead of %d!", addronlystring, result, expecteditems);
		retval = 1;
		return (retval);
	};

	/* copy into structure */
	for ( i = 0; i <= 3; i++ ) {
		if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
			fprintf(stderr, "%s: Octett %d = %d\n", DEBUG_function_name, i, compat[i]);
		};
		ipv4addr_setoctet(ipv4addrp, (unsigned int) i, (unsigned int) compat[i]);
	};

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: In structure %03u %03u %03u %03u\n", DEBUG_function_name, (unsigned int) ipv4addr_getoctet(ipv4addrp, 0), (unsigned int) ipv4addr_getoctet(ipv4addrp, 1), (unsigned int) ipv4addr_getoctet(ipv4addrp, 2), (unsigned int) ipv4addr_getoctet(ipv4addrp, 3));
		fprintf(stderr, "%s: In structure %8x\n", DEBUG_function_name, (unsigned int) ipv4addr_getdword(ipv4addrp));
	};
	
	typeinfo = ipv4addr_gettype(ipv4addrp); 

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: Got scope %02x\n", DEBUG_function_name, (unsigned int) typeinfo);
	};

	ipv4addrp->scope = typeinfo;
	ipv4addrp->flag_valid = 1;

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * function stores an IPv4 hexadecimal string into a structure
 *
 * in : *addrstring = IPv4 address in hex format
 * out: *resultstring = error message
 * out: ipv4addrp = changed IPv4 address structure
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv4addr/addrhex_to_ipv4addrstruct"
int addrhex_to_ipv4addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv4addr *ipv4addrp, int flag_reverse) {
	int retval = 1, result, i;
	char *addronlystring, *cp;
	int expecteditems = 0;
	int compat[4];
	char tempstring[NI_MAXHOST], *cptr, **ptrptr;
	uint32_t typeinfo;

	ptrptr = &cptr;

	resultstring[0] = '\0'; /* clear result string */

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: Got input '%s'\n", DEBUG_function_name,  addrstring);
	};

	if ((strlen(addrstring) < 8)  || (strlen(addrstring) > 11)) {
		/* min: ffffffff */
		/* max: ffffffff/32 */
		snprintf(resultstring, NI_MAXHOST - 1, "Error in given hex notated IPv4 address, has not 8 to 11 chars!");
		return (1);
	};

	if (strlen(addrstring) > sizeof(tempstring) - 1) {
		fprintf(stderr, "%s: Input too long: %s\n", DEBUG_function_name, addrstring);
		return (1);
	};

	strncpy(tempstring, addrstring, sizeof(tempstring) - 1);
	
	ipv4addr_clearall(ipv4addrp);

	/* save prefix length first, if available */
	addronlystring = strtok_r(tempstring, "/", ptrptr);
	cp = strtok_r(NULL, "/", ptrptr);
	if ( cp != NULL ) {
		i = atoi(cp);
		if (i < 0 || i > 32 ) {
			snprintf(resultstring, NI_MAXHOST - 1, "Illegal prefix length: '%s'", cp);
			retval = 1;
			return (retval);
		};
		ipv4addrp->flag_prefixuse = 1;
		ipv4addrp->prefixlength = (uint8_t) i;
		
		if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
			fprintf(stderr, "%s: prefix length %u\n", DEBUG_function_name, (unsigned int) ipv4addrp->prefixlength);
			fprintf(stderr, "%s: flag_prefixuse %d\n", DEBUG_function_name, ipv4addrp->flag_prefixuse);
		};
	};

	/* check length 3 */
	if (strlen(addronlystring) != 8) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error, given hexadecimal IPv4 address '%s' is not valid (not proper length)!", addronlystring);
		retval = 1;
		return (retval);
	};

	/* clear variables */
	for ( i = 0; i <= 3; i++ ) {
		compat[i] = 0;
	};

	expecteditems = 4;
	if (flag_reverse != 0)  {
		result = sscanf(addronlystring, "%2x%2x%2x%2x", &compat[3], &compat[2], &compat[1], &compat[0]);
	} else {
		result = sscanf(addronlystring, "%2x%2x%2x%2x", &compat[0], &compat[1], &compat[2], &compat[3]);
	};
	
	for ( i = 0; i <= 3; i++ ) {
		if ( ( compat[i] < 0 ) || ( compat[i] > 255 ) )	{
			snprintf(resultstring, NI_MAXHOST - 1, "Error, given IPv4 address '%s' is not valid (%d on position %d)!", addronlystring, compat[i], i+1);
			retval = 1;
			return (retval);
		};
	};
	
	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: reading into array, got items: %d\n", DEBUG_function_name, result);
	};

	if ( result != expecteditems ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error splitting address %s, got %d items instead of %d!", addronlystring, result, expecteditems);
		retval = 1;
		return (retval);
	};

	/* copy into structure */
	for ( i = 0; i <= 3; i++ ) {
		if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
			fprintf(stderr, "%s: Octett %d = %d\n", DEBUG_function_name, i, compat[i]);
		};
		ipv4addr_setoctet(ipv4addrp, (unsigned int) i, (unsigned int) compat[i]);
	};

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: In structure %03u %03u %03u %03u\n", DEBUG_function_name, (unsigned int) ipv4addr_getoctet(ipv4addrp, 0), (unsigned int) ipv4addr_getoctet(ipv4addrp, 1), (unsigned int) ipv4addr_getoctet(ipv4addrp, 2), (unsigned int) ipv4addr_getoctet(ipv4addrp, 3));
		fprintf(stderr, "%s: In structure %8x\n", DEBUG_function_name, (unsigned int) ipv4addr_getdword(ipv4addrp));
	};
	
	typeinfo = ipv4addr_gettype(ipv4addrp); 

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: Got scope %02x\n", DEBUG_function_name, (unsigned int) typeinfo);
	};

	ipv4addrp->scope = typeinfo;
	ipv4addrp->flag_valid = 1;

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * stores the ipv4addr structure in a string
 *
 * in:  ipv4addr = IPv4 address structure
 * out: *resultstring = IPv4 address string
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv4addr/ipv4addrstruct_to_string"
int libipv4addr_ipv4addrstruct_to_string(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions) {
	char tempstring[NI_MAXHOST];

	/* address */
	snprintf(tempstring, sizeof(tempstring) - 1, "%u.%u.%u.%u", (unsigned int) ipv4addr_getoctet(ipv4addrp, 0), (unsigned int) ipv4addr_getoctet(ipv4addrp, 1), (unsigned int) ipv4addr_getoctet(ipv4addrp, 2), (unsigned int) ipv4addr_getoctet(ipv4addrp, 3));

	if ( (formatoptions & FORMATOPTION_machinereadable) != 0 ) {
		snprintf(resultstring, NI_MAXHOST - 1, "IPV4=%s", tempstring);
	} else {
		snprintf(resultstring, NI_MAXHOST - 1, "%s", tempstring);
	};

	/* netmaks */
	if (ipv4addrp->flag_prefixuse == 1) {
		/* to be filled */
		/* IPV4NETMASK= */
	};
	
	return(0);
};
#undef DEBUG_function_name


/*
 * get registry string of an IPv4 address
 *
 * in:  ipv4addr = IPv4 address structure
 * out: *resultstring = Registry string
 * ret: 0: ok, 1: unknown, 2: reserved
 */
#define DEBUG_function_name "libipv4addr/get_registry_string"
int libipv4addr_get_registry_string(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring) {
	int i;
	int match = -1;
	uint32_t match_mask = 0;
	uint8_t  octet_msb;

	uint32_t ipv4 = ipv4addr_getdword(ipv4addrp);
	
	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: Given IPv4 address: %08x\n", DEBUG_function_name, (unsigned int) ipv4);
	};

	if ((ipv4 & 0xff000000u) == 0x00000000u) {
		// 0.0.0.0/8 (RFC 1122)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC1122#3.2.1.3)");
		return (2);
	} else if ((ipv4 & 0xff000000u) == 0x0a000000u) {
		// 10.0.0.0/8 (RFC 1918)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC1918#3)");
		return (2);
	} else if ((ipv4 & 0xff000000u) == 0x7f000000u) {
		// 127.0.0.0/8 (RFC 1122)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC1122#3.2.1.3)");
		return (2);
	} else if ((ipv4 & 0xffff0000u) == 0xa9fe0000u) {
		// 169.254.0.0/16 (RFC 1918)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC3927#1)");
		return (2);
	} else if ((ipv4 & 0xfff00000u) == 0xac100000u) {
		// 172.16.0.0/12 (RFC 1918)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC1918#3)");
		return (2);
	} else if ((ipv4 & 0xffff0000u) == 0xc0a80000u) {
		// 192.168.0.0/16 (RFC 1918)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC1918#3)");
		return (2);
	} else if ((ipv4 & 0xffffff00u) == 0xc0000000u) {
		// 192.0.0.0/24 (RFC 5736)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC5736#1)");
		return (2);
	} else if ((ipv4 & 0xffffff00u) == 0xc0000200u) {
		// 192.0.2.0/24 (RFC 3330)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC5737#1)");
		return (2);
	} else if ((ipv4 & 0xffffff00u) == 0xc0586300u) {
		// 192.88.99.0/24 (RFC 3068)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC3068#2.3)");
		return (2);
	} else if ((ipv4 & 0xfffe0000u) == 0xc6120000u) {
		// 198.18.0.0/15 (RFC 2544)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC2544#C.2.2)");
		return (2);
	} else if ((ipv4 & 0xffffff00u) == 0xc6336400u) {
		// 198.51.100.0/24 (RFC 5737)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC5737#3)");
		return (2);
	} else if ((ipv4 & 0xffffff00u) == 0xcb007100u) {
		// 203.0.113.0/24 (RFC 5737)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC5737#3)");
		return (2);
	} else if ((ipv4 & 0xf0000000u) == 0xe0000000u) {
		// 224.0.0.0/4 (RFC 3171)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC3171#2)");
		return (2);
	} else if ((ipv4 & 0xffffffffu) == 0xffffffffu) {
		// 255.255.255.255/32
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC919#7)");
		return (2);
	} else if ((ipv4 & 0xf0000000u) == 0xf0000000u) {
		// 240.0.0.0/4 (RFC 1112)
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "reserved(RFC1112#4)");
		return (2);
	}; 

#ifdef SUPPORT_DB_IPV4
#define OPTIMIZED_LOOKUP 1
#ifdef OPTIMIZED_LOOKUP
	/* lookup in hint table for faster start */
	octet_msb = ipv4addr_getoctet(ipv4addrp, 0);

	for (i = (int) dbipv4addr_assignment_hint[octet_msb].start; i <= (int) dbipv4addr_assignment_hint[octet_msb].end; i++) {
#else
	for (i = 0; i < (int) ( sizeof(dbipv4addr_assignment) / sizeof(dbipv4addr_assignment[0])); i++) {
#endif
		/* run through database array */
		if ( (ipv4 & dbipv4addr_assignment[i].ipv4mask) == dbipv4addr_assignment[i].ipv4addr ) {
			/* ok, entry matches */
			if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
				fprintf(stderr, "%s: Found match number: %d\n", DEBUG_function_name, i);
			};

			/* have already found one */
			if ( match != -1 ) {
				if ( dbipv4addr_assignment[i].ipv4mask > match_mask ) {
					/* this entry wins */
					if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
						fprintf(stderr, "%s: Overwrite match number: %d (old: %d)\n", DEBUG_function_name, i, match);
					};
					match = i;
					match_mask = dbipv4addr_assignment[i].ipv4mask;
				} else {
					if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
						fprintf(stderr, "%s: No overwriting of match number: %d (candidate: %d)\n", DEBUG_function_name, match, i);
					};
				};
			} else {
				match = i;
				match_mask = dbipv4addr_assignment[i].ipv4mask;
			};
		};
	};

	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: Final match number: %d\n", DEBUG_function_name, match);
	};
	
	/* result */
	if ( match > -1 ) {
		snprintf(resultstring, NI_MAXHOST - 1, "%s", dbipv4addr_assignment[match].string_registry);
		return(0);
	} else {
		snprintf(resultstring, NI_MAXHOST - 1, "%s", "unknown");
		return(1);
	};
#else
	snprintf(resultstring, NI_MAXHOST - 1, "%s", "(IPv4 database not compiled in)");
	return(1);
#endif

};
#undef DEBUG_function_name

/*
 * get registry number of an IPv4 address
 *
 * in:  ipv4addr = IPv4 address structure
 * out: assignment number (-1 = no result)
 */
#define DEBUG_function_name "libipv4addr/getregistry"
int ipv4addr_getregistry(const ipv6calc_ipv4addr *ipv4addrp) {
	char resultstring[NI_MAXHOST];
	int i;

	i = libipv4addr_get_registry_string(ipv4addrp, resultstring);

	if (i == 2) {
		return(IPV4_ADDR_REGISTRY_RESERVED);
	} else if (i != 0) {
		return(IPV4_ADDR_REGISTRY_UNKNOWN);
	};

#ifdef SUPPORT_DB_IPV4
	if (strcmp(resultstring, "IANA") == 0) {
		return(IPV4_ADDR_REGISTRY_IANA);
	} else if (strcmp(resultstring, "APNIC") == 0) {
		return(IPV4_ADDR_REGISTRY_APNIC);
	} else if (strcmp(resultstring, "ARIN") == 0) {
		return(IPV4_ADDR_REGISTRY_ARIN);
	} else if (strcmp(resultstring, "RIPENCC") == 0) {
		return(IPV4_ADDR_REGISTRY_RIPE);
	} else if (strcmp(resultstring, "LACNIC") == 0) {
		return(IPV4_ADDR_REGISTRY_LACNIC);
	} else if (strcmp(resultstring, "AFRINIC") == 0) {
		return(IPV4_ADDR_REGISTRY_AFRINIC);
	} else {
		return(IPV4_ADDR_REGISTRY_UNKNOWN);
	};
#else
	return(IPV4_ADDR_REGISTRY_UNKNOWN);
#endif
};
#undef DEBUG_function_name

/*
 * converts IPv4addr_structure to a reverse decimal format string
 *
 * in : *ipv4addrp = IPv4 address structure
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv4addr/addr_to_reversestring"
int libipv4addr_to_reversestring(ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	uint8_t octet;
	int bit_start, bit_end, nbit;
	char tempstring[NI_MAXHOST];
	unsigned int noctet;
	
	if ( ((formatoptions & (FORMATOPTION_printprefix | FORMATOPTION_printsuffix | FORMATOPTION_printstart | FORMATOPTION_printend)) == 0 ) && ((*ipv4addrp).flag_prefixuse != 0) ) {
		bit_start = 1;
		bit_end = (int) (*ipv4addrp).prefixlength;
	} else {
		bit_start = 1;
		bit_end = 32;
	};
	
	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: start bit %d  end bit %d\n", DEBUG_function_name, bit_start, bit_end);
	};

	/* print out nibble format */
	/* 31 is lowest bit, 0 is highest bit */
	resultstring[0] = '\0';

	for (nbit = bit_end - 1; nbit >= bit_start - 1; nbit = nbit - 8) {
		/* calculate octet (8 bit) */
		noctet = ( ((unsigned int) nbit) & 0x78) >> 3;
		
		/* extract octet */
		octet = ipv4addr_getoctet(ipv4addrp, noctet);
		
		if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
			fprintf(stderr, "%s: bit: %d = noctet: %u, value: %x\n", DEBUG_function_name, nbit, noctet, (unsigned int) octet);
		};

		snprintf(tempstring, sizeof(tempstring) - 1, "%s%u", resultstring, (unsigned int) octet);
		snprintf(resultstring, NI_MAXHOST - 1, "%s.", tempstring);
	};

	if (bit_start == 1) {
		snprintf(tempstring, sizeof(tempstring) - 1, "%sin-addr.arpa.", resultstring);
	};

	snprintf(resultstring, NI_MAXHOST - 1, "%s", tempstring);

	if ( (formatoptions & FORMATOPTION_printuppercase) != 0 ) {
		string_to_upcase(resultstring);
	};
	
	if ( (formatoptions & FORMATOPTION_printmirrored) != 0 ) {
		string_to_reverse_dotted(resultstring);
	};
		
	if ( (ipv6calc_debug & DEBUG_libipv4addr) != 0 ) {
		fprintf(stderr, "%s: Print out: %s\n", DEBUG_function_name, resultstring);
	};

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * function prints an IPv4 address in native octal format
 *
 * in:  ipv4addr = IPv4 address structure
 * formatoptions
 * out: *resultstring = IPv4 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv4addr/ipv4addrstruct_to_octal"
int libipv4addr_to_octal(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	char tempstring[NI_MAXHOST];

	if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
		snprintf(tempstring, sizeof(tempstring) - 1, "\\0%03o\\0%03o\\0%03o\\0%03o",
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 0),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 1),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 2),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 3)   \
		);
	} else {
		snprintf(tempstring, sizeof(tempstring) - 1, "\\0%o\\0%o\\0%o\\0%o",
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 0),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 1),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 2),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 3)   \
		);
	};

	snprintf(resultstring, NI_MAXHOST - 1, "%s", tempstring);
	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name


/*
 * function prints an IPv4 address in native hex format
 *
 * in:  ipv4addr = IPv4 address structure
 * formatoptions
 * out: *resultstring = IPv4 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv4addr/ipv4addrstruct_to_hex"
int libipv4addr_to_hex(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	char tempstring[NI_MAXHOST];

	snprintf(tempstring, sizeof(tempstring) - 1, "%02x%02x%02x%02x",
		(unsigned int) ipv4addr_getoctet(ipv4addrp, 0),  \
		(unsigned int) ipv4addr_getoctet(ipv4addrp, 1),  \
		(unsigned int) ipv4addr_getoctet(ipv4addrp, 2),  \
		(unsigned int) ipv4addr_getoctet(ipv4addrp, 3)   \
	);

	snprintf(resultstring, NI_MAXHOST - 1, "%s", tempstring);
	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name


/*
 * anonymize IPv4 address
 *
 * in : *ipv4addrp = IPv4 address structure
 *      mask = number of bits of mask
 * ret: <void>
 */
#define DEBUG_function_name "libipv4addr/anonymize"
void libipv4addr_anonymize(ipv6calc_ipv4addr *ipv4addrp, unsigned int mask) {
	/* anonymize IPv4 address according to settings */

	if (mask == 0) {
		/* clear IPv4 address: 0.0.0.0 */
		ipv4addr_clear(ipv4addrp);
	} else if (mask == 32) {
		/* nothing to do */
	} else if (mask < 1 || mask > 31) {
		/* should not happen here */
		fprintf(stderr, "%s: 'mask' has an unexpected illegal value!\n", DEBUG_function_name);
		exit(EXIT_FAILURE);
	} else {
		/* quick mode */
		if (mask == 24) {
			ipv4addr_setoctet(ipv4addrp, 3, 0u);
		} else if (mask == 16) {
			ipv4addr_setword(ipv4addrp, 1, 0u);
		} else if (mask == 8) {
			ipv4addr_setword(ipv4addrp, 1, 0u);
			ipv4addr_setoctet(ipv4addrp, 1, 0u);
		} else {
			/* mask IPv4 address */
			ipv4addr_setdword(ipv4addrp, ipv4addr_getdword(ipv4addrp) & (0xffffffffu << ((unsigned int) 32 - mask)));
		};
	};
	return;
};
#undef DEBUG_function_name

