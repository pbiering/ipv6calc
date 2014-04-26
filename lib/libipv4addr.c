/*
 * Project    : ipv6calc/lib
 * File       : libipv4addr.c
 * Version    : $Id: libipv4addr.c,v 1.55 2014/04/26 16:26:29 ds6peter Exp $
 * Copyright  : 2002-2014 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
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

#include "../databases/lib/libipv6calc_db_wrapper.h"


/*
 * function returns an octet of an IPv4 address
 *
 * in: ipv4addrp = pointer to IPv4 address structure
 * in: numoctet  = number of octet (0 = MSB, 3 = LSB)
 * additional: calls exit on out of range
 */
uint8_t ipv4addr_getoctet(const ipv6calc_ipv4addr *ipv4addrp, const unsigned int numoctet) {
	uint8_t retval;
	
	if ( numoctet > 3 ) {
		fprintf(stderr, "%s/%s: given octet number '%u' is out of range!\n", __FILE__, __func__, numoctet);
		exit(EXIT_FAILURE);
	};

	retval = (uint8_t) ( (ipv4addrp->in_addr.s_addr >> ( (unsigned int) (3 - numoctet) << 3)) & 0xff );

	return (retval);
};


/*
 * function returns a word of an IPv4 address
 *
 * in: ipv6addrp = pointer to IPv4 address structure
 * in: numword   = number of word (0 = MSB, 1 = LSB)
 * additional: calls exit on out of range
 */
uint16_t ipv4addr_getword(const ipv6calc_ipv4addr *ipv4addrp, const unsigned int numword) {
	uint16_t retval;
	
	if ( numword > 1 ) {
		ERRORPRINT_WA("given word number '%u' is out of range!", numword);
		exit(EXIT_FAILURE);
	};

	retval = (uint16_t) ( (ipv4addr_getoctet(ipv4addrp, (numword << 1)) << 8 ) | ipv4addr_getoctet(ipv4addrp, (numword << 1) + 1) );

	return (retval);
};


/*
 * function returns a dword of an IPv4 address
 *
 * in: ipv4addrp = pointer to IPv4 address structure
 * additional: calls exit on out of range
 */
uint32_t ipv4addr_getdword(const ipv6calc_ipv4addr *ipv4addrp) {
	uint32_t retval;
	
	retval = (uint32_t) ( (ipv4addr_getword(ipv4addrp, 0) << 16) | ipv4addr_getword(ipv4addrp, 1) );

	return (retval);
};


/*
 * function sets an octet of an IPv4 address
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 * in: numoctet   = number of word (0 = MSB, 3 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
void ipv4addr_setoctet(ipv6calc_ipv4addr *ipv4addrp, const unsigned int numoctet, const uint8_t value) {
	
	if ( numoctet > 3 ) {
		ERRORPRINT_WA("given octet number '%u' is out of range!", numoctet);
		exit(EXIT_FAILURE);
	};
	
	ipv4addrp->in_addr.s_addr &= ~ (0xff << ((unsigned int) (3 - numoctet) << 3) );
	ipv4addrp->in_addr.s_addr |= (value & 0xff) << ((unsigned int) (3 - numoctet) << 3);

	return;
};


/*
 * function sets a word of an IPv4 address
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 * in: numword   = number of word (0 = MSB, 1 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
void ipv4addr_setword(ipv6calc_ipv4addr *ipv4addrp, const unsigned int numword, const uint16_t value) {
	unsigned int n;
	unsigned int v;
	
	if ( numword > 1 ) {
		ERRORPRINT_WA("given word number '%u' is out of range!", numword);
		exit(EXIT_FAILURE);
	};
	
	n = numword << 1;
	v = (value & 0xff00) >> 8;

	DEBUGPRINT_WA(DEBUG_libipv4addr, "set octet %u: %02x (%u %04x)", n, v, numword, value);

	ipv4addr_setoctet(ipv4addrp, n, v);

	n = (numword << 1) + 1;
	v = value & 0xff;

	DEBUGPRINT_WA(DEBUG_libipv4addr, "set octet %u: %02x (%u %04x)", n, v, numword, value);

	ipv4addr_setoctet(ipv4addrp, n, v);
	
	return;
};


/*
 * function sets a dword of an IPv4 address
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 * in: value     = value to set
 * additional: calls exit on out of range
 */
void ipv4addr_setdword(ipv6calc_ipv4addr *ipv4addrp, const uint32_t value) {
	unsigned int n;
	unsigned int v;
	
	if ( value > 0xffffffffu ) {
		ERRORPRINT_WA("given value '%x' is out of range!", value);
		exit(EXIT_FAILURE);
	}; 

	n = 0;
	v = (value & 0xffff0000u) >> 16;

	DEBUGPRINT_WA(DEBUG_libipv4addr, "set word %u: %04x (%08x)", n, v, value);

	ipv4addr_setword(ipv4addrp, n, v);

	n = 1;
	v = value & 0xffffu;

	DEBUGPRINT_WA(DEBUG_libipv4addr, "set word %u: %04x (%08x)", n, v, value);

	ipv4addr_setword(ipv4addrp, n, v);

	return;
};


/*
 * function clears the IPv4 structure
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 */
void ipv4addr_clear(ipv6calc_ipv4addr *ipv4addrp) {
	ipv4addrp->in_addr.s_addr = 0;

	/* Clear valid flag */
	ipv4addrp->flag_valid = 0;

	return;
};


/*
 * function clears the IPv4 structure
 *
 * mod: ipv4addrp = pointer to IPv4 address structure
 */
void ipv4addr_clearall(ipv6calc_ipv4addr *ipv4addrp) {
	ipv4addr_clear(ipv4addrp);
	(*ipv4addrp).prefixlength = 0;
	(*ipv4addrp).flag_prefixuse = 0;

	return;
};


/*
 * function copies the IPv4 structure
 *
 * in:  ipv64ddrp  = pointer to IPv4 address structure
 * mod: ipv64ddrp2 = pointer to IPv4 address structure
 */
void ipv4addr_copy(ipv6calc_ipv4addr *ipv4addrp_dst, const ipv6calc_ipv4addr *ipv4addrp_src) {

	*(ipv4addrp_dst) = *(ipv4addrp_src);
	
	return;
};


/*
 * function gets type of an IPv4 address
 */
uint32_t ipv4addr_gettype(const ipv6calc_ipv4addr *ipv4addrp) {
	uint32_t type = 0, c, p;
	uint32_t ipv4 = ipv4addr_getdword(ipv4addrp);
	int i;

	DEBUGPRINT_WA(DEBUG_libipv4addr, "Got IPv4 address: 0x%08x", ipv4);

	/* check for anonymized IPv4 address */
	if ((ipv4 & 0xf0000000u) == 0xf0000000u) {
		DEBUGPRINT_NA(DEBUG_libipv4addr, "Check for anonymized IPv4 address");

		// count payload bits
		c = 0;
		p = 0x00000001;
		for (i = 0; i < 27; i++) {
			if ((ipv4 & p) != 0) {
				c++;
			};
			p <<= 1;
		};

		DEBUGPRINT_WA(DEBUG_libipv4addr, "Check for anonymized address, parity count c=%d", c);

		if (((c & 0x1) ^ 0x1) == ((ipv4 >> 27) & 0x1)) {
			// check country code (limited value)
			p = (ipv4 >> 17) & 0x3ff;

			if ((p <= COUNTRYCODE_INDEX_LETTER_MAX) || (p == COUNTRYCODE_INDEX_UNKNOWN) || ((p >= COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN) && (p <= COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MAX))) {
				DEBUGPRINT_NA(DEBUG_libipv4addr, "Address is an anonymized one");

				type = IPV4_ADDR_ANONYMIZED | IPV4_ADDR_UNICAST| IPV4_ADDR_GLOBAL;
				goto END_ipv4addr_gettype;
			};
		} else {
				DEBUGPRINT_NA(DEBUG_libipv4addr, "Address is not an detected anonymized one");
		};
	};

	if (ipv4 == 0x00000000u) {
		type = IPV4_ADDR_UNSPECIFIED;
	} else if ((ipv4 & 0xff000000u) == 0x0a000000u) {
		// 10.0.0.0/8 (RFC 1918)
		type = IPV4_ADDR_UNICAST | IPV4_ADDR_SITELOCAL;
	} else if ((ipv4 & 0xffc00000u) == 0x64400000u) {
		// 100.64.0.0/10 (RFC 6598)
		type = IPV4_ADDR_UNICAST | IPV4_ADDR_RESERVED;
	} else if ((ipv4 & 0xff000000u) == 0x7f000000u) {
		// 127.0.0.0/8 (RFC 1122)
		type = IPV4_ADDR_LOOPBACK;
	} else if ((ipv4 & 0xffff0000u) == 0xa9fe0000u) {
		// 169.254.0.0/16 (RFC 1918)
		type = IPV4_ADDR_UNICAST | IPV4_ADDR_ZEROCONF | IPV4_ADDR_SITELOCAL;
	} else if ((ipv4 & 0xfff00000u) == 0xac100000u) {
		// 172.16.0.0/12 (RFC 1918)
		type = IPV4_ADDR_UNICAST | IPV4_ADDR_SITELOCAL;
	} else if ((ipv4 & 0xffff0000u) == 0xc0a80000u) {
		// 192.168.0.0/16 (RFC 1918)
		type = IPV4_ADDR_UNICAST | IPV4_ADDR_SITELOCAL;
	} else if ((ipv4 & 0xffffff00u) == 0xc0000000u) {
		// 192.0.0.0/24 (RFC 5736)
		type = IPV4_ADDR_UNICAST | IPV4_ADDR_RESERVED;
	} else if ((ipv4 & 0xffffff00u) == 0xc0000200u) {
		// 192.0.2.0/24 (RFC 3330)
		type = IPV4_ADDR_UNICAST | IPV4_ADDR_RESERVED;
	} else if ((ipv4 & 0xffffff00u) == 0xc0586300u) {
		// 192.88.99.0/24 (RFC 3068)
		type = IPV4_ADDR_ANYCAST | IPV4_ADDR_6TO4RELAY;
	} else if ((ipv4 & 0xfffe0000u) == 0xc6120000u) {
		// 198.18.0.0/15 (RFC 2544)
		type = IPV4_ADDR_UNICAST | IPV4_ADDR_RESERVED;
	} else if ((ipv4 & 0xffffff00u) == 0xc6336400u) {
		// 198.51.100.0/24 (RFC 5737)
		type = IPV4_ADDR_UNICAST | IPV4_ADDR_RESERVED;
	} else if ((ipv4 & 0xffffff00u) == 0xcb007100u) {
		// 203.0.113.0/24 (RFC 5737)
		type = IPV4_ADDR_UNICAST | IPV4_ADDR_RESERVED;
	} else if ((ipv4 & 0xf0000000u) == 0xe0000000u) {
		// 224.0.0.0/4 (RFC 3171)
		type = IPV4_ADDR_MULTICAST;
	} else if ((ipv4 & 0xffffffffu) == 0xffffffffu) {
		// 255.255.255.255/32
		type = IPV4_ADDR_BROADCAST;
	} else if ((ipv4 & 0xff000000u) == 0x00000000u) {
		// 0.0.0.0/8
		type = IPV4_ADDR_RESERVED;
	} else if ((ipv4 & 0xf0000000u) == 0xf0000000u) {
		// 240.0.0.0/4 (RFC 1112)
		type = IPV4_ADDR_RESERVED;
	} else {
		type = IPV4_ADDR_UNICAST | IPV4_ADDR_GLOBAL;
	};

	DEBUGPRINT_WA(DEBUG_libipv4addr, "return typeinfo: 0x%08x", type);

END_ipv4addr_gettype:
	return(type);
};


/*
 * function stores an IPv4 address string into a structure
 *
 * in : *addrstring = IPv4 address
 * out: *resultstring = error message
 * out: ipv4addrp = changed IPv4 address structure
 * ret: ==0: ok, !=0: error
 */
int addr_to_ipv4addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv4addr *ipv4addrp) {
	int retval = 1, result, i, cpoints = 0, cdigits = 0;
	char *addronlystring, *cp;
	int expecteditems = 0;
	int compat[4];
	char tempstring[NI_MAXHOST], *cptr, **ptrptr;
	uint32_t typeinfo;

	ptrptr = &cptr;

	resultstring[0] = '\0'; /* clear result string */

	DEBUGPRINT_WA(DEBUG_libipv4addr, "Got input '%s'",  addrstring);

	if ((strlen(addrstring) < 7) || (strlen(addrstring) > 18)) {
		/* min: 0.0.0.0 */
		/* max: 123.123.123.123/32 */
		snprintf(resultstring, NI_MAXHOST - 1, "Error in given dot notated IPv4 address, has not 7 to 18 chars!");
		return (1);
	};

	if (strlen(addrstring) > sizeof(tempstring) - 1) {
		ERRORPRINT_WA("Input too long: %s", addrstring);
		return (1);
	};

	snprintf(tempstring, sizeof(tempstring) - 1, "%s", addrstring);
	
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
		
		DEBUGPRINT_WA(DEBUG_libipv4addr, "prefix length %u", (unsigned int) ipv4addrp->prefixlength);
		DEBUGPRINT_WA(DEBUG_libipv4addr, "flag_prefixuse %d", ipv4addrp->flag_prefixuse);
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
	
	DEBUGPRINT_WA(DEBUG_libipv4addr, "reading into array, got items: %d", result);

	if ( result != expecteditems ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error splitting address %s, got %d items instead of %d!", addronlystring, result, expecteditems);
		retval = 1;
		return (retval);
	};

	/* copy into structure */
	for ( i = 0; i <= 3; i++ ) {
		DEBUGPRINT_WA(DEBUG_libipv4addr, "Octett %d = %d", i, compat[i]);
		ipv4addr_setoctet(ipv4addrp, (unsigned int) i, (unsigned int) compat[i]);
	};

	DEBUGPRINT_WA(DEBUG_libipv4addr, "In structure %03u %03u %03u %03u", (unsigned int) ipv4addr_getoctet(ipv4addrp, 0), (unsigned int) ipv4addr_getoctet(ipv4addrp, 1), (unsigned int) ipv4addr_getoctet(ipv4addrp, 2), (unsigned int) ipv4addr_getoctet(ipv4addrp, 3));
	DEBUGPRINT_WA(DEBUG_libipv4addr, "In structure %8x", (unsigned int) ipv4addr_getdword(ipv4addrp));
	
	typeinfo = ipv4addr_gettype(ipv4addrp); 

	DEBUGPRINT_WA(DEBUG_libipv4addr, "Got typeinfo: 0x%08x", typeinfo);

	ipv4addrp->scope = typeinfo;
	ipv4addrp->flag_valid = 1;

	retval = 0;
	return (retval);
};


/*
 * function stores an IPv4 hexadecimal string into a structure
 *
 * in : *addrstring = IPv4 address in hex format
 * out: *resultstring = error message
 * out: ipv4addrp = changed IPv4 address structure
 * ret: ==0: ok, !=0: error
 */
int addrhex_to_ipv4addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv4addr *ipv4addrp, int flag_reverse) {
	int retval = 1, result, i;
	char *addronlystring, *cp;
	int expecteditems = 0;
	int compat[4];
	char tempstring[NI_MAXHOST], *cptr, **ptrptr;
	uint32_t typeinfo;

	ptrptr = &cptr;

	resultstring[0] = '\0'; /* clear result string */

	DEBUGPRINT_WA(DEBUG_libipv4addr, "Got input '%s'",  addrstring);

	if ((strlen(addrstring) < 7)  || (strlen(addrstring) > 11)) {
		/* min: ffffffff */
		/* max: ffffffff/32 */
		snprintf(resultstring, NI_MAXHOST - 1, "Error in given hex notated IPv4 address, has not 7 to 11 chars!");
		return (1);
	};

	if (strlen(addrstring) > sizeof(tempstring) - 1) {
		ERRORPRINT_WA("Input too long: %s", addrstring);
		return (1);
	};

	snprintf(tempstring, sizeof(tempstring) - 1, "%s", addrstring);
	
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
		
		DEBUGPRINT_WA(DEBUG_libipv4addr, "prefix length %u", (unsigned int) ipv4addrp->prefixlength);
		DEBUGPRINT_WA(DEBUG_libipv4addr, "flag_prefixuse %d", ipv4addrp->flag_prefixuse);
	};

	/* check length 3 */
	if ((flag_reverse == 0 && (strlen(addronlystring) < 7 || strlen(addronlystring) > 8)) || (flag_reverse != 0 && strlen(addronlystring) != 8)) {
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
		if (strlen(addronlystring) == 7) {
			result = sscanf(addronlystring, "%1x%2x%2x%2x", &compat[0], &compat[1], &compat[2], &compat[3]);
		} else {
			result = sscanf(addronlystring, "%2x%2x%2x%2x", &compat[0], &compat[1], &compat[2], &compat[3]);
		};
	};
	
	for ( i = 0; i <= 3; i++ ) {
		if ( ( compat[i] < 0 ) || ( compat[i] > 255 ) )	{
			snprintf(resultstring, NI_MAXHOST - 1, "Error, given IPv4 address '%s' is not valid (%d on position %d)!", addronlystring, compat[i], i+1);
			retval = 1;
			return (retval);
		};
	};
	
	DEBUGPRINT_WA(DEBUG_libipv4addr, "reading into array, got items: %d", result);

	if ( result != expecteditems ) {
		snprintf(resultstring, NI_MAXHOST - 1, "Error splitting address %s, got %d items instead of %d!", addronlystring, result, expecteditems);
		retval = 1;
		return (retval);
	};

	/* copy into structure */
	for ( i = 0; i <= 3; i++ ) {
		DEBUGPRINT_WA(DEBUG_libipv4addr, "Octett %d = %d", i, compat[i]);
		ipv4addr_setoctet(ipv4addrp, (unsigned int) i, (unsigned int) compat[i]);
	};

	DEBUGPRINT_WA(DEBUG_libipv4addr, "In structure %03u %03u %03u %03u", (unsigned int) ipv4addr_getoctet(ipv4addrp, 0), (unsigned int) ipv4addr_getoctet(ipv4addrp, 1), (unsigned int) ipv4addr_getoctet(ipv4addrp, 2), (unsigned int) ipv4addr_getoctet(ipv4addrp, 3));
	DEBUGPRINT_WA(DEBUG_libipv4addr, "In structure %8x", (unsigned int) ipv4addr_getdword(ipv4addrp));
	
	typeinfo = ipv4addr_gettype(ipv4addrp); 

	DEBUGPRINT_WA(DEBUG_libipv4addr, "Got typeinfo: 0x%08x", typeinfo);

	ipv4addrp->scope = typeinfo;
	ipv4addrp->flag_valid = 1;

	retval = 0;
	return (retval);
};


/*
 * stores the ipv4addr structure in a string
 *
 * in:  ipv4addr = IPv4 address structure
 * out: *resultstring = IPv4 address string
 * ret: ==0: ok, !=0: error
 */
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


/*
 * converts IPv4addr_structure to a reverse decimal format string
 *
 * in : *ipv4addrp = IPv4 address structure
 * out: *resultstring = result
 * ret: ==0: ok, !=0: error
 */
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
	
	DEBUGPRINT_WA(DEBUG_libipv4addr, "start bit %d  end bit %d", bit_start, bit_end);

	/* print out nibble format */
	/* 31 is lowest bit, 0 is highest bit */
	resultstring[0] = '\0';

	for (nbit = bit_end - 1; nbit >= bit_start - 1; nbit = nbit - 8) {
		/* calculate octet (8 bit) */
		noctet = ( ((unsigned int) nbit) & 0x78) >> 3;
		
		/* extract octet */
		octet = ipv4addr_getoctet(ipv4addrp, noctet);
		
		DEBUGPRINT_WA(DEBUG_libipv4addr, "bit: %d = noctet: %u, value: %x", nbit, noctet, (unsigned int) octet);

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
		
	DEBUGPRINT_WA(DEBUG_libipv4addr, "Print out: %s", resultstring);

	retval = 0;
	return (retval);
};


/*
 * function prints an IPv4 address in native octal format
 *
 * in:  ipv4addr = IPv4 address structure
 * formatoptions
 * out: *resultstring = IPv4 address (modified)
 * ret: ==0: ok, !=0: error
 */
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


/*
 * function prints an IPv4 address in native hex format
 *
 * in:  ipv4addr = IPv4 address structure
 * formatoptions
 * out: *resultstring = IPv4 address (modified)
 * ret: ==0: ok, !=0: error
 */
int libipv4addr_to_hex(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, /*@unused@*/ const uint32_t formatoptions) {
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


/*
 * anonymize IPv4 address
 *
 * in : *ipv4addrp = IPv4 address structure
 *      mask = number of bits of mask
 *      method = 2:zeroize  1:map to CountryCode and AS
 * ret: 0:anonymization ok
 *      1:anonymization method not supported
 */
int libipv4addr_anonymize(ipv6calc_ipv4addr *ipv4addrp, unsigned int mask, const int method) {
	DEBUGPRINT_WA(DEBUG_libipv4addr, "called, method=%d mask=%d type=0x%08x", method, mask, ipv4addrp->scope);

	/* anonymize IPv4 address according to settings */
	char resultstring[NI_MAXHOST];
	uint32_t as_num32, as_num32_comp17, as_num32_decomp17, ipv4addr_anon, p;
	uint16_t cc_index, c;
	int i;

	if ((ipv4addrp->scope & IPV4_ADDR_ANONYMIZED) != 0) {
		DEBUGPRINT_NA(DEBUG_libipv4addr, "skip already anonymized address");

	} else if ((ipv4addrp->scope & IPV4_ADDR_BROADCAST) != 0) {
		DEBUGPRINT_NA(DEBUG_libipv4addr, "skip anonymize (broadcast address)");

	} else if ((method != ANON_METHOD_KEEPTYPEASNCC) || ((ipv4addrp->scope & (IPV4_ADDR_UNICAST | IPV4_ADDR_GLOBAL)) != (IPV4_ADDR_UNICAST | IPV4_ADDR_GLOBAL))) {
		// not ANON_METHOD_KEEPTYPEASNCC or not a global address

		DEBUGPRINT_NA(DEBUG_libipv4addr, "anonymize by masking");

		if (mask == 0) {
			/* clear IPv4 address: 0.0.0.0 */
			ipv4addr_clear(ipv4addrp);
			ipv4addrp->flag_valid = 1;
		} else if (mask == 32) {
			/* nothing to do */
		} else if (mask < 1 || mask > 31) {
			/* should not happen here */
			fprintf(stderr, "%s/%s: 'mask' has an unexpected illegal value!\n", __FILE__, __func__);
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
	} else {
		/* check for required database support */
		if (libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV4_REQ_DB) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv4addr, "anonymization method not supported, db_wrapper reports too less features");
			return(1);
		};

		DEBUGPRINT_NA(DEBUG_libipv4addr, "anonymize by keep information");

		libipv4addr_ipv4addrstruct_to_string(ipv4addrp, resultstring, 0);

		// get AS number
		as_num32 = libipv6calc_db_wrapper_as_num32_by_addr(resultstring, 4);
		DEBUGPRINT_WA(DEBUG_libipv4addr, "result of AS number  retrievement: 0x%08x (%d)", as_num32, as_num32);

		as_num32_comp17 = libipv6calc_db_wrapper_as_num32_comp17(as_num32);
		DEBUGPRINT_WA(DEBUG_libipv4addr, "result of AS number   compression: 0x%05x", as_num32_comp17);

		as_num32_decomp17 = libipv6calc_db_wrapper_as_num32_decomp17(as_num32_comp17);
		DEBUGPRINT_WA(DEBUG_libipv4addr, "result of AS number decompression: 0x%08x (%d)", as_num32_decomp17, as_num32_decomp17);

		// get countrycode
		cc_index = libipv6calc_db_wrapper_cc_index_by_addr(resultstring, 4);
		if (cc_index == COUNTRYCODE_INDEX_UNKNOWN) {
			// on unknown country, map registry value
			cc_index = COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN + libipv6calc_db_wrapper_registry_num_by_ipv4addr(ipv4addrp);
		};

		DEBUGPRINT_WA(DEBUG_libipv4addr, "result of CountryCode index retrievement: 0x%03x (%d)", cc_index, cc_index);

		// 0-3   ( 4 bits) : prefix 0xf0
		// 4     ( 1 bit ) : parity bit (odd parity)
		// 5-14  (10 bits) : country code index
		// 15    ( 1 bits) : ASN flag
		// 16-31 (16 bits) : ASN16 or packed ASN32
		//
		ipv4addr_anon = 0xf0000000 | ((cc_index << 17) & 0x07fe0000 ) | (as_num32_comp17 & 0x0001ffff);

		// create parity bits
		c = 0;
		p = 0x00000001;
		for (i = 0; i < 27; i++) {
			if ((ipv4addr_anon & p) != 0) {
				c++;
			};
			p <<= 1;
		};

		ipv4addr_anon |= ((c & 0x1) ^ 0x1) << 27;

		DEBUGPRINT_WA(DEBUG_libipv4addr, "result anonymized IPv4 address: 0x%08x, bitcounts=%d", ipv4addr_anon, c);

		ipv4addr_setdword(ipv4addrp, ipv4addr_anon);
	};

	DEBUGPRINT_NA(DEBUG_libipv4addr, "return");

	return(0);
};


/*
 * get AS number of anonymized IPv4 address
 *
 * in : *ipv4addrp = IPv4 address structure
 * ret: AS number 16-bit
 */
uint32_t ipv4addr_anonymized_get_as_num32(const ipv6calc_ipv4addr *ipv4addrp) {
	if ((ipv4addrp->scope & IPV4_ADDR_ANONYMIZED) == 0) {
		return(ASNUM_AS_UNKNOWN);
	};

	return(libipv6calc_db_wrapper_as_num32_decomp17(ipv4addr_getdword(ipv4addrp) & 0x1ffff));
};


/*
 * get CountryCode index of anonymized IPv4 address
 *
 * in : *ipv4addrp = IPv4 address structure
 * ret: CountryCode index
 */
uint16_t ipv4addr_anonymized_get_cc_index(const ipv6calc_ipv4addr *ipv4addrp) {
	if ((ipv4addrp->scope & IPV4_ADDR_ANONYMIZED) == 0) {
		return(COUNTRYCODE_INDEX_UNKNOWN);
	};

	return((ipv4addr_getword(ipv4addrp, 0) & 0x7fe) >>1);
};


/*
 * clear filter IPv4 address
 *
 * in : *filter    = filter structure
 */
void ipv4addr_filter_clear(s_ipv6calc_filter_ipv4addr *filter) {
	filter->active = 0;
	filter->typeinfo_must_have = 0;
	filter->typeinfo_may_not_have = 0;
	return;
};


/*
 * parse filter IPv4 address
 *
 * in : *filter    = filter structure
 * ret: 0:found 1:skip 2:problem
 */
int ipv4addr_filter_parse(s_ipv6calc_filter_ipv4addr *filter, const char *token) {
	int i, result = 1, negate = 0, offset = 0;
	const char *prefix = "ipv4";
	const char *prefixdot = "ipv4.";

	if (token == NULL) {
		return (result);
	};

	DEBUGPRINT_WA(DEBUG_libipv4addr, "input: %s", token);

	if (token[0] == '^') {
		DEBUGPRINT_WA(DEBUG_libipv4addr, "found negate prefix in token: %s", token);

		negate = 1;
		offset += 1;
	};

	if (strcmp(token + offset, prefix) == 0) {
		/* any */
		if (negate == 1) {
			filter->typeinfo_may_not_have = ~IPV4_ADDR_ANY;
		} else {
			filter->typeinfo_must_have = IPV4_ADDR_ANY;
		};
		filter->active = 1;
		result = 0;
		goto END_ipv4addr_filter_parse;

	} else if (strncmp(token + offset, prefixdot, strlen(prefixdot)) == 0) {
		/* prefix with dot found */
		offset += strlen(prefixdot);
		result = 2; /* token with prefix, result into problem if not found */

	} else if (strstr(token, ".") != NULL) {
		/* other prefix */
		DEBUGPRINT_WA(DEBUG_libipv4addr, "prefix did not match: %s", token + offset);
		return(1);
	};

	for (i = 0; i < (int) (sizeof(ipv6calc_ipv4addrtypestrings) / sizeof(ipv6calc_ipv4addrtypestrings[0])); i++ ) {
		DEBUGPRINT_WA(DEBUG_libipv4addr, "check token against: %s", ipv6calc_ipv4addrtypestrings[i].token);

		if (strcmp(ipv6calc_ipv4addrtypestrings[i].token, token + offset) == 0) {
			DEBUGPRINT_WA(DEBUG_libipv4addr, "token match: %s", ipv6calc_ipv4addrtypestrings[i].token);

			if (negate == 1) {
				filter->typeinfo_may_not_have |= ipv6calc_ipv4addrtypestrings[i].number;
			} else {
				filter->typeinfo_must_have |= ipv6calc_ipv4addrtypestrings[i].number;
			};
			filter->active = 1;
			result = 0;
			break;
		};
	};

	if (result != 0) {
		DEBUGPRINT_WA(DEBUG_libipv4addr, "token not supported: %s",token);
		return (result);
	};

END_ipv4addr_filter_parse:
	DEBUGPRINT_WA(DEBUG_libipv4addr, "filter 'must_have'   : 0x%08x", filter->typeinfo_must_have);
	DEBUGPRINT_WA(DEBUG_libipv4addr, "filter 'may_not_have': 0x%08x", filter->typeinfo_may_not_have);
	DEBUGPRINT_WA(DEBUG_libipv4addr, "filter 'active': %d", filter->active);

	return (result);
};


/*
 * filter IPv4 address
 *
 * in : *ipv4addrp = IPv4 address structure
 * in : *filter    = filter structure
 * ret: 0=match 1=not match
 */
int ipv4addr_filter(const ipv6calc_ipv4addr *ipv4addrp, const s_ipv6calc_filter_ipv4addr *filter) {
	uint32_t typeinfo;

	if (filter->active == 0) {
		DEBUGPRINT_NA(DEBUG_libipv4addr, "no filter active (SKIP)");
		return (1);
	};

	DEBUGPRINT_NA(DEBUG_libipv4addr, "start");

	/* get type */
	typeinfo = ipv4addr_gettype(ipv4addrp);

	DEBUGPRINT_WA(DEBUG_libipv4addr, "compare typeinfo against must_have: 0x%08x/0x%08x", typeinfo, filter->typeinfo_must_have);

	if ((typeinfo & filter->typeinfo_must_have) == filter->typeinfo_must_have) {
		if ((typeinfo & filter->typeinfo_may_not_have) == 0) {
			return (0);
		};
	};

	return (1);
};


/*
 * country code index of IPv4 address
 *
 * in : *ipv4addrp = IPv4 address structure
 * out: country code index
 */
uint16_t libipv4addr_cc_index_by_addr(const ipv6calc_ipv4addr *ipv4addrp) {
	uint16_t cc_index = COUNTRYCODE_INDEX_UNKNOWN;
	char tempipv4string[NI_MAXHOST] = "";
	int retval;

	DEBUGPRINT_NA(DEBUG_libipv4addr, "start");

	if ((ipv4addrp->scope & IPV4_ADDR_ANONYMIZED) != 0) {
		cc_index = ipv4addr_anonymized_get_cc_index(ipv4addrp);
	} else if ((ipv4addrp->scope & IPV4_ADDR_RESERVED) != 0) {
		cc_index = COUNTRYCODE_INDEX_UNKNOWN;
	} else {
		if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_CC) == 1) {
			retval = libipv4addr_ipv4addrstruct_to_string(ipv4addrp, tempipv4string, 0);
			if ( retval != 0 ) {
				fprintf(stderr, "Error converting IPv4 address: %s\n", tempipv4string);
				goto END_libipv4addr_cc_index_by_addr;
			};

			cc_index = libipv6calc_db_wrapper_cc_index_by_addr(tempipv4string, 4);
		};
	};

END_libipv4addr_cc_index_by_addr:
	DEBUGPRINT_WA(DEBUG_libipv4addr, "cc_index=%d (0x%03x)", cc_index, cc_index);
	return(cc_index);
};


/*
 * 32-bit AS number of IPv4 address
 *
 * in : *ipv4addrp = IPv4 address structure
 * out: 32-bit AS number
 */
uint32_t libipv4addr_as_num32_by_addr(const ipv6calc_ipv4addr *ipv4addrp) {
	uint32_t as_num32 = ASNUM_AS_UNKNOWN;
	char tempipv4string[NI_MAXHOST] = "";
	int retval;

	if ((ipv4addrp->scope & IPV4_ADDR_ANONYMIZED) != 0) {
		as_num32 = ipv4addr_anonymized_get_as_num32(ipv4addrp);
	} else {
		if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_AS) == 1) {
			retval = libipv4addr_ipv4addrstruct_to_string(ipv4addrp, tempipv4string, 0);
			if ( retval != 0 ) {
				fprintf(stderr, "Error converting IPv4 address: %s\n", tempipv4string);
				goto END_libipv4addr_as_num32_by_addr;
			};

			as_num32 = libipv6calc_db_wrapper_as_num32_by_addr(tempipv4string, 4);
		};
	};

END_libipv4addr_as_num32_by_addr:
	DEBUGPRINT_WA(DEBUG_libipv4addr, "as_num32=%d (0x%08x)", as_num32, as_num32);
	return(as_num32);
};


/*
 * registry number of IPv4 address
 *
 * in : *ipv4addrp = IPv4 address structure
 * out: registry number
 */
int libipv4addr_registry_num_by_addr(const ipv6calc_ipv4addr *ipv4addrp) {
	int registry = IPV4_ADDR_REGISTRY_UNKNOWN;
	uint16_t cc_index;

	if ((ipv4addrp->scope & IPV4_ADDR_ANONYMIZED) != 0) {
		DEBUGPRINT_NA(DEBUG_libipv4addr, "IPv4 is anonymized, extract registry from anonymized data");
		/* retrieve registry via cc_index from anonymized address (simple) */
		cc_index = libipv4addr_cc_index_by_addr(ipv4addrp);
		registry = libipv6calc_db_wrapper_registry_num_by_cc_index(cc_index);
	} else {
		if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_REGISTRY) == 1) {
			DEBUGPRINT_NA(DEBUG_libipv4addr, "Get registry from IPV6CALC_DB_IPV4_TO_REGISTRY");
			registry = libipv6calc_db_wrapper_registry_num_by_ipv4addr(ipv4addrp);
		} else {
			DEBUGPRINT_NA(DEBUG_libipv4addr, "No support available for IPV6CALC_DB_IPV4_TO_REGISTRY");
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv4addr, "registry=%d (0x%x)", registry, registry);
	return(registry);
};
