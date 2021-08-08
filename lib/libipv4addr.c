/*
 * Project    : ipv6calc/lib
 * File       : libipv4addr.c
 * Version    : $Id$
 * Copyright  : 2002-2021 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
 * License    : GNU GPL v2
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
		ERRORPRINT_WA("given octet number '%u' is out of range!\n", numoctet);
		exit(EXIT_FAILURE);
	};

	retval = (uint8_t) ( (ipv4addrp->in_addr.s_addr >> ( numoctet << 3)) & 0xff );

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
	
	ipv4addrp->in_addr.s_addr &= ~ (0xff << (numoctet << 3) );
	ipv4addrp->in_addr.s_addr |= (value & 0xff) << (numoctet << 3);

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

	/* Clear test mode */
	ipv4addrp->test_mode = 0;

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
 * in:  ipv4addrp_src = pointer to IPv4 address structure
 * mod: ipv4addrp_dst = pointer to IPv4 address structure
 */
void ipv4addr_copy(ipv6calc_ipv4addr *ipv4addrp_dst, const ipv6calc_ipv4addr *ipv4addrp_src) {

	*(ipv4addrp_dst) = *(ipv4addrp_src);
	
	return;
};


/*
 * function compares the IPv4 structure
 *
 * in:  ipv4addrp1  = pointer to IPv4 address structure
 * in:  ipv4addrp2  = pointer to IPv4 address structure
 * in:  compare_flags:
 *   0=less than/equal/greater than
 *   1=honor prefix length of addr2
 * returns: 0: addr2 equal with addr1, 1: addr1 > addr2, -1: addr1 < addr2 (compare_flags == 0)
 * returns: 0: addr1 equal with addr2 or covered by addr2/prefix (compare_flags == 1)
 */
int ipv4addr_compare(const ipv6calc_ipv4addr *ipv4addrp1, const ipv6calc_ipv4addr *ipv4addrp2, const uint16_t compare_flags) {
	uint32_t ipv4addr1 = ipv4addr_getdword(ipv4addrp1);
	uint32_t ipv4addr2 = ipv4addr_getdword(ipv4addrp2);

	DEBUGPRINT_WA(DEBUG_libipv4addr, "compare addr1 with addr2 0x%08x/%d 0x%08x/%d (compare flags: %08x)", ipv4addr1, ipv4addrp1->prefixlength, ipv4addr2, ipv4addrp2->prefixlength, compare_flags);

	if (compare_flags == 1) {
		// honors prefix of addr2
		if (ipv4addrp2->flag_prefixuse == 1) {
			/* mask addr2 with prefix length of addr1 */
			ipv4addr1 &= (0xffffffffu << ((unsigned int) 32 - ipv4addrp2->prefixlength));
			/* mask addr1 with prefix length of addr1 */
			ipv4addr2 &= (0xffffffffu << ((unsigned int) 32 - ipv4addrp2->prefixlength));
		};

		DEBUGPRINT_WA(DEBUG_libipv4addr, "compare addr1 with addr2 0x%08x 0x%08x (after masking with prefix length of addr2)", ipv4addr1, ipv4addr2);

		if (ipv4addr1 != ipv4addr2) {
			return(1);
		};
	} else if (compare_flags == 0) {
		if (ipv4addr1 > ipv4addr2) {
			return(1);
		} else if (ipv4addr1 < ipv4addr2) {
			return(-1);
		} else {
		};
	};

	return(0);
};


/*
 * Set type of an IPv4 address
 *
 * in: ipv4addrp = pointer to IPv4 address structure
 *     flag_reset = 1: redetect type
 */
void ipv4addr_settype(ipv6calc_ipv4addr *ipv4addrp, int flag_reset) {
	uint32_t typeinfo = 0, typeinfo2 = 0;

	if (flag_reset == 1) {
		ipv4addrp->flag_typeinfo = 0;
	};

	if (ipv4addrp->flag_typeinfo == 0) {
		//typeinfo = ipv4addr_gettype(ipv4addrp, &typeinfo2);
		typeinfo = ipv4addr_gettype(ipv4addrp);

		ipv4addrp->typeinfo = typeinfo;
		ipv4addrp->typeinfo2 = typeinfo2;
		ipv4addrp->flag_typeinfo = 1;
	};
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
			// odd parity: check country code (limited value)
			p = (ipv4 >> 17) & 0x3ff;

			if ((p <= COUNTRYCODE_INDEX_LETTER_MAX) || (p == COUNTRYCODE_INDEX_UNKNOWN) || ((p >= COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MIN) && (p <= COUNTRYCODE_INDEX_UNKNOWN_REGISTRY_MAP_MAX))) {
				DEBUGPRINT_NA(DEBUG_libipv4addr, "Address is an anonymized one (KeepType/ASN/CountryCode)");

				type = IPV4_ADDR_ANONYMIZED | IPV4_ADDR_UNICAST| IPV4_ADDR_GLOBAL;

				if ((ipv4 & 0x11fff) == 0x11800) {
					DEBUGPRINT_NA(DEBUG_libipv4addr, "Address is a LISP unicast one");
					type |= IPV4_ADDR_LISP;
				};
				goto END_ipv4addr_gettype;
			};
		} else  if (((c & 0x1) ^ 0x0) == ((ipv4 >> 27) & 0x1)) {
			//even parity: check GeonameID source
			p = (ipv4 >> 24) & 0x7;

			if (p == 0x7) {
				DEBUGPRINT_NA(DEBUG_libipv4addr, "Address is potentially an anonymized one (KeepType/GeonameID: LISP)");

				if ((ipv4 & 0x11fff) == 0x11800) {
					// 11 bit LISP information still not encoded
					DEBUGPRINT_NA(DEBUG_libipv4addr, "Address is a LISP unicast one");

					type = IPV4_ADDR_ANONYMIZED | IPV4_ADDR_UNICAST| IPV4_ADDR_GLOBAL | IPV4_ADDR_ANONYMIZED_GEONAMEID | IPV4_ADDR_LISP;

					goto END_ipv4addr_gettype;
				};
			};

			if (((p > IPV6CALC_DB_GEO_GEONAMEID_TYPE_UNKNOWN) \
			     && p <= IPV6CALC_DB_GEO_GEONAMEID_TYPE_CITY)) {
				DEBUGPRINT_NA(DEBUG_libipv4addr, "Address is an anonymized one (KeepGeonameID)");

				type = IPV4_ADDR_ANONYMIZED | IPV4_ADDR_UNICAST| IPV4_ADDR_GLOBAL | IPV4_ADDR_ANONYMIZED_GEONAMEID;

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
	} else if ((ipv4 & 0xffff0000u) == 0x99100000u) {
		// 153.16.0.0/16 (RFC6830)
		type = IPV4_ADDR_UNICAST | IPV4_ADDR_GLOBAL | IPV4_ADDR_LISP;
	} else if ((ipv4 & 0xffffff00u) == 0xc169b700u) {
		// 193.105.183.0/24 (RFC6830)
		type = IPV4_ADDR_ANYCAST | IPV4_ADDR_GLOBAL | IPV4_ADDR_LISP | IPV4_ADDR_LISP_PETR;
	} else if ((ipv4 & 0xffffff00u) == 0x5bdcc900u) {
		// 91.220.201.0/24 (RFC6830)
		type = IPV4_ADDR_ANYCAST | IPV4_ADDR_GLOBAL | IPV4_ADDR_LISP | IPV4_ADDR_LISP_MAP_RESOLVER;
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
int addr_to_ipv4addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv4addr *ipv4addrp) {
	int in_prefix_len = 0, digit = 0;
	unsigned int compat[5], i;
	uint32_t typeinfo;
	const char *p;

	if (resultstring_length > 0)
		resultstring[0] = '\0'; /* clear result string */

	DEBUGPRINT_WA(DEBUG_libipv4addr, "Got input '%s'",  addrstring);

	if (*addrstring == '\0') {
		snprintf(resultstring, resultstring_length, "Error in given dot notated IPv4 address: empty string!");
		return (1);
	}

	memset(compat, 0, sizeof(compat));

	if (sscanf(addrstring, "0%3o.0%3o.0%3o.0%3o/%2d", &compat[0], &compat[1], &compat[2], &compat[3], &compat[4]) == 5) {
		// IPv4 address in octal format separated by . with prefix length
		in_prefix_len = 1;
	} else if (sscanf(addrstring, "0%3o.0%3o.0%3o.0%3o", &compat[0], &compat[1], &compat[2], &compat[3]) == 4) {
		// IPv4 address in octal format separated by .
	} else {
		// standard but potentially shortened notation
		for (p = addrstring, i = 0; *p && i < (in_prefix_len ? 5 : 4); p++)
		{
			if (*p >= '0' && *p <= '9') {
				digit = 1;
				if (in_prefix_len && p[0] == '0' && p[1] && compat[i] == 0) {
					snprintf(resultstring, resultstring_length, "Error in given IPv4 address, '%s' is not valid (CIDR prefix length cannot start with zero)!",
						 addrstring);
					return (1);
				}
				compat[i] = compat[i] * 10 + (*p - '0');
				if (compat[i] > (in_prefix_len ? 32 : 255)) {
					snprintf(resultstring, resultstring_length, "Error in given IPv4 address, '%s' is not valid (%d on position %d)!",
						 addrstring, compat[i], (int)(p - addrstring + 1));
					return (1);
				}
			} else if (*p == '.' && !in_prefix_len && digit) {
				digit = 0;
				i++;
			} else if (*p == '/' && !in_prefix_len && p != addrstring) {
				digit = 0;
				in_prefix_len = 1;
				i = 4;
			} else {
				snprintf(resultstring, resultstring_length, "Error in given IPv4 address, '%s' is not valid (illegal char at %d)!",
					 addrstring, (int)(p - addrstring + 1));
				return (1);
			}
		}
		if (p[-1] == '/') {
			snprintf(resultstring, resultstring_length, "Error in given IPv4 address, '%s' is not valid (illegal char at %d)!",
				 addrstring, (int)(p - addrstring));
			return (1);
		}
		if (i >= (in_prefix_len ? 5 : 4)) {
			snprintf(resultstring, resultstring_length, "Error in given IPv4 address, '%s' is not valid (more than 3 dots)!", addrstring);
			return (1);
		}

		/* do not allow '1.2', but allow '1.2/12' */
		if (!in_prefix_len && i < 3) {
			snprintf(resultstring, resultstring_length, "Error in given IPv4 address, '%s' is not valid (missing %d dots)!", addrstring, 3-i);
			return (1);
		}
		if (!in_prefix_len && p[-1] == '.') {
			snprintf(resultstring, resultstring_length, "Error in given IPv4 address, '%s' is not valid (missing last octet)!", addrstring);
			return (1);
		}
	};

	ipv4addr_clearall(ipv4addrp);
	if (in_prefix_len) {
		ipv4addrp->flag_prefixuse = 1;

		if (compat[4] > 32) {
			snprintf(resultstring, resultstring_length, "Error in given IPv4 address, '%s' is not valid (prefix length is out-of-range 0..32)!", addrstring);
			return (1);
		};
		ipv4addrp->prefixlength = (uint8_t) compat[4];

		DEBUGPRINT_WA(DEBUG_libipv4addr, "prefix length %u", (unsigned int) ipv4addrp->prefixlength);
		DEBUGPRINT_WA(DEBUG_libipv4addr, "flag_prefixuse %d", ipv4addrp->flag_prefixuse);
	}
	for ( i = 0; i <= 3; i++ ) {
		DEBUGPRINT_WA(DEBUG_libipv4addr, "Octett %d = %d", i, compat[i]);
		ipv4addr_setoctet(ipv4addrp, i, compat[i]);
	};

	DEBUGPRINT_WA(DEBUG_libipv4addr, "In structure %03u %03u %03u %03u", (unsigned int) ipv4addr_getoctet(ipv4addrp, 0), (unsigned int) ipv4addr_getoctet(ipv4addrp, 1), (unsigned int) ipv4addr_getoctet(ipv4addrp, 2), (unsigned int) ipv4addr_getoctet(ipv4addrp, 3));
	DEBUGPRINT_WA(DEBUG_libipv4addr, "In structure %8x", (unsigned int) ipv4addr_getdword(ipv4addrp));

	typeinfo = ipv4addr_gettype(ipv4addrp); 

	DEBUGPRINT_WA(DEBUG_libipv4addr, "Got typeinfo: 0x%08x", typeinfo);

	ipv4addrp->typeinfo = typeinfo;
	ipv4addrp->flag_valid = 1;

	return (0);
};


/*
 * function stores an IPv4 hexadecimal string into a structure
 *
 * in : *addrstring = IPv4 address in hex format
 * out: *resultstring = error message
 * out: ipv4addrp = changed IPv4 address structure
 * ret: ==0: ok, !=0: error
 */
int addrhex_to_ipv4addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv4addr *ipv4addrp, const int flag_reverse) {
	int retval = 1, result, i;
	char *addronlystring, *cp;
	int expecteditems = 0;
	int compat[4];
	char tempstring[IPV6CALC_STRING_MAX], *cptr, **ptrptr;
	uint32_t typeinfo;

	ptrptr = &cptr;

	resultstring[0] = '\0'; /* clear result string */

	DEBUGPRINT_WA(DEBUG_libipv4addr, "Got input '%s'",  addrstring);

	if ((strlen(addrstring) < 7)  || (strlen(addrstring) > 11)) {
		/* min: ffffffff */
		/* max: ffffffff/32 */
		snprintf(resultstring, resultstring_length, "Error in given hex notated IPv4 address, has not 7 to 11 chars!");
		return (1);
	};

	if (strlen(addrstring) >= sizeof(tempstring)) {
		ERRORPRINT_WA("Input too long: %s", addrstring);
		return (1);
	};

	snprintf(tempstring, sizeof(tempstring), "%s", addrstring);
	
	ipv4addr_clearall(ipv4addrp);

	/* save prefix length first, if available */
	addronlystring = strtok_r(tempstring, "/", ptrptr);
	cp = strtok_r(NULL, "/", ptrptr);
	if ( cp != NULL ) {
		i = atoi(cp);
		if (i < 0 || i > 32 ) {
			snprintf(resultstring, resultstring_length, "Error in given hex notated IPv4 address, illegal prefix length: '%s'", cp);
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
		snprintf(resultstring, resultstring_length, "Error in given hex notated IPv4 address, '%s' is not valid (not proper length)!", addronlystring);
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
			snprintf(resultstring, resultstring_length, "Error in given hex notated IPv4 address, '%s' is not valid (%d on position %d)!", addronlystring, compat[i], i+1);
			retval = 1;
			return (retval);
		};
	};
	
	DEBUGPRINT_WA(DEBUG_libipv4addr, "reading into array, got items: %d", result);

	if ( result != expecteditems ) {
		snprintf(resultstring, resultstring_length, "Error in given hex notated IPv4 address, splitting '%s' returns %d items instead of %d!", addronlystring, result, expecteditems);
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

	ipv4addrp->typeinfo = typeinfo;
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
int libipv4addr_ipv4addrstruct_to_string(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	char tempstring[IPV6CALC_STRING_MAX];

	if ((formatoptions & FORMATOPTION_machinereadable) != 0) {
		snprintf(resultstring, resultstring_length, "IPV4=%u.%u.%u.%u",
		         (unsigned int) ipv4addr_getoctet(ipv4addrp, 0),
		         (unsigned int) ipv4addr_getoctet(ipv4addrp, 1),
		         (unsigned int) ipv4addr_getoctet(ipv4addrp, 2),
		         (unsigned int) ipv4addr_getoctet(ipv4addrp, 3));
	} else {
		int oct_to_print = 4;

		if ((formatoptions & FORMATOPTION_printcompressed) != 0) {
			for (; oct_to_print > 1; oct_to_print--)
				if (ipv4addr_getoctet(ipv4addrp, oct_to_print-1) != 0)
					break;
		}

		switch (oct_to_print) {
		case 4:
			snprintf(tempstring, sizeof(tempstring), "%u.%u.%u.%u",
			         (unsigned int) ipv4addr_getoctet(ipv4addrp, 0),
			         (unsigned int) ipv4addr_getoctet(ipv4addrp, 1),
			         (unsigned int) ipv4addr_getoctet(ipv4addrp, 2),
			         (unsigned int) ipv4addr_getoctet(ipv4addrp, 3));
			break;

		case 3:
			snprintf(tempstring, sizeof(tempstring), "%u.%u.%u",
			         (unsigned int) ipv4addr_getoctet(ipv4addrp, 0),
			         (unsigned int) ipv4addr_getoctet(ipv4addrp, 1),
			         (unsigned int) ipv4addr_getoctet(ipv4addrp, 2));
			break;

		case 2:
			snprintf(tempstring, sizeof(tempstring), "%u.%u",
			         (unsigned int) ipv4addr_getoctet(ipv4addrp, 0),
			         (unsigned int) ipv4addr_getoctet(ipv4addrp, 1));
			break;

		case 1:
			snprintf(tempstring, sizeof(tempstring), "%u",
			         (unsigned int) ipv4addr_getoctet(ipv4addrp, 0));
			break;

		default:
			/* should not happen here */
			fprintf(stderr, "%s/%s: 'oct_to_print' has an unexpected illegal value!\n", __FILE__, __func__);
			exit(EXIT_FAILURE);
		}

		if ((ipv4addrp->flag_prefixuse == 1) && ((formatoptions & FORMATOPTION_no_prefixlength) == 0)) {
			snprintf(resultstring, resultstring_length, "%s/%d", tempstring, ipv4addrp->prefixlength);
		} else {
			snprintf(resultstring, resultstring_length, "%s", tempstring);
		};
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
int libipv4addr_to_reversestring(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	int retval = 1;
	uint8_t octet;
	int bit_start, bit_end, nbit;
	char tempstring[IPV6CALC_STRING_MAX];
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

		snprintf(tempstring, sizeof(tempstring), "%s%u", resultstring, (unsigned int) octet);
		snprintf(resultstring, resultstring_length, "%s.", tempstring);
	};

	if (bit_start == 1) {
		snprintf(tempstring, sizeof(tempstring), "%sin-addr.arpa.", resultstring);
	};

	snprintf(resultstring, resultstring_length, "%s", tempstring);

	if ( (formatoptions & FORMATOPTION_printuppercase) != 0 ) {
		string_to_upcase(resultstring);
	};
	
	if ( (formatoptions & FORMATOPTION_printmirrored) != 0 ) {
		string_to_reverse_dotted(resultstring, resultstring_length);
	};
		
	DEBUGPRINT_WA(DEBUG_libipv4addr, "Print out: %s", resultstring);

	retval = 0;
	return (retval);
};


/*
 * function prints an IPv4 address in native octal format
 *
 * in:  ipv4addr = IPv4 address structure
 * in:  formatoptions
 * out: *resultstring = IPv4 address (modified)
 * ret: ==0: ok, !=0: error
 */
int libipv4addr_to_octal(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	int retval = 1;
	char tempstring[IPV6CALC_STRING_MAX];

	if ( (formatoptions & FORMATOPTION_print_octal_separated) != 0 ) {
		snprintf(tempstring, sizeof(tempstring), "0%03o.0%03o.0%03o.0%03o",
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 0),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 1),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 2),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 3)   \
		);
	} else if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
		snprintf(tempstring, sizeof(tempstring), "\\0%03o\\0%03o\\0%03o\\0%03o",
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 0),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 1),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 2),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 3)   \
		);
	} else {
		snprintf(tempstring, sizeof(tempstring), "\\0%o\\0%o\\0%o\\0%o",
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 0),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 1),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 2),  \
			(unsigned int) ipv4addr_getoctet(ipv4addrp, 3)   \
		);
	};

	if ((ipv4addrp->flag_prefixuse == 1) && ((formatoptions & FORMATOPTION_no_prefixlength) == 0) && ((formatoptions & FORMATOPTION_print_octal_separated) != 0)) {
		snprintf(resultstring, resultstring_length, "%s/%d", tempstring, ipv4addrp->prefixlength);
	} else {
		snprintf(resultstring, resultstring_length, "%s", tempstring);
	};

	retval = 0;	
	return (retval);
};


/*
 * mask prefix bits (set suffix bits to 0)
 *
 * in:  structure via reference
 * out: modified structure
 */
void ipv4addrstruct_maskprefix(ipv6calc_ipv4addr *ipv4addrp) {
	unsigned int nbit, nword;
	uint16_t mask, newword;
	int i;

	DEBUGPRINT_NA(DEBUG_libipv4addr, "called");

	if (ipv4addrp->flag_prefixuse != 1) {
		/* hmm, no prefix specified. skip */
		return;
	};

	for (i = 31; i >= 0; i--) {
		nbit = (unsigned int) i;
		if (nbit >= (unsigned int) ipv4addrp->prefixlength) {
			/* set bit to zero */

			/* calculate word (16 bit) - matches with addr4p[]*/
			nword = (nbit & 0x70) >> 4;

			/* calculate mask */
			mask = ((uint16_t) 0x8000u) >> (( ((uint16_t) nbit) & ((uint16_t) 0x0fu)));
			newword = ipv4addr_getword(ipv4addrp, nword) & (~ mask );

			DEBUGPRINT_WA(DEBUG_libipv4addr, "bit: %u = nword: %u, mask: %04x, word: %04x newword: %04x", nbit, nword, (unsigned int) mask, (unsigned int) ipv4addr_getword(ipv4addrp, nword), (unsigned int) newword);

			ipv4addr_setword(ipv4addrp, nword, (unsigned int) newword);
		};
	};
};


/*
 * mask suffix bits (set prefix bits to 0)
 *
 * in:  structure via reference
 * out: modified structure
 */
void ipv4addrstruct_masksuffix(ipv6calc_ipv4addr *ipv4addrp) {
	unsigned int nbit, nword;
	uint16_t mask, newword;
	int i;

	DEBUGPRINT_NA(DEBUG_libipv4addr, "called");
 
	if (ipv4addrp->flag_prefixuse != 1) {
		/* hmm, no prefix specified. skip */
		return;
	};

	for (i = 31; i >= 0; i--) {
		nbit = (unsigned int) i;

		if (nbit < (unsigned int) ipv4addrp->prefixlength) {
			/* set bit to zero */

			/* calculate word (16 bit) - matches with addr4p[]*/
			nword = (nbit & 0x70) >> 4;

			/* calculate mask */
			mask = ((uint32_t) 0x8000u) >> (((uint32_t) nbit) & ((uint32_t) 0x0fu ));
			newword = ipv4addr_getword(ipv4addrp, nword) & (~ mask );

			DEBUGPRINT_WA(DEBUG_libipv4addr, "%u = nword: %u, mask: %04x, word: %04x newword: %04x", nbit, nword, (unsigned int) mask, (unsigned int) ipv4addr_getword(ipv4addrp, nword), (unsigned int) newword);

			ipv4addr_setword(ipv4addrp, nword, (unsigned int) newword);
		};
	};
};


/*
 * function prints an IPv4 address in native hex format
 *
 * in:  ipv4addr = IPv4 address structure
 * formatoptions
 * out: *resultstring = IPv4 address (modified)
 * ret: ==0: ok, !=0: error
 */
int libipv4addr_to_hex(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions) {
	DEBUGPRINT_WA(DEBUG_libipv4addr, "called, formatoptions=0x%08x", formatoptions);

	int retval = 1;
	int i;
	unsigned int s;
	char tempstring[IPV6CALC_STRING_MAX];

	snprintf(tempstring, sizeof(tempstring), "%02x%02x%02x%02x",
		(unsigned int) ipv4addr_getoctet(ipv4addrp, 0),  \
		(unsigned int) ipv4addr_getoctet(ipv4addrp, 1),  \
		(unsigned int) ipv4addr_getoctet(ipv4addrp, 2),  \
		(unsigned int) ipv4addr_getoctet(ipv4addrp, 3)   \
	);

	snprintf(resultstring, resultstring_length, "%s", tempstring);


	if ((formatoptions & FORMATOPTION_printprefix) && (ipv4addrp->flag_prefixuse == 1)) {
		// shorten string
		resultstring[ipv4addrp->prefixlength / 4] = '\0';
	} else if ((formatoptions & FORMATOPTION_printsuffix) && (ipv4addrp->flag_prefixuse == 1)) {
		// move string
		for (i = 0; i < 8 - (ipv4addrp->prefixlength / 4); i++) {
			resultstring[i] = resultstring[i + (ipv4addrp->prefixlength / 4)];
		};
		resultstring[8 - (ipv4addrp->prefixlength / 4)] = '\0';
	};

	if (formatoptions & FORMATOPTION_printuppercase) {
		for (s = 0; s < strlen(resultstring); s++) {
			resultstring[s] = toupper(resultstring[s]);
		};
	};

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
	DEBUGPRINT_WA(DEBUG_libipv4addr, "called, method=%d mask=%d type=0x%08x", method, mask, ipv4addrp->typeinfo);

	/* anonymize IPv4 address according to settings */
	uint32_t as_num32, as_num32_comp17, as_num32_decomp17, ipv4addr_anon, p;
	uint16_t cc_index, c;
	ipv6calc_ipaddr ipaddr;
	int i;

	ipv4addr_settype(ipv4addrp, 0); // set typeinfo if not already done

	if ((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED) != 0) {
		DEBUGPRINT_NA(DEBUG_libipv4addr, "skip already anonymized address");

	} else if ((ipv4addrp->typeinfo & IPV4_ADDR_BROADCAST) != 0) {
		DEBUGPRINT_NA(DEBUG_libipv4addr, "skip anonymize (broadcast address)");

	} else if (((method != ANON_METHOD_KEEPTYPEASNCC) && (method != ANON_METHOD_KEEPTYPEGEONAMEID)) || ((ipv4addrp->typeinfo & (IPV4_ADDR_UNICAST | IPV4_ADDR_GLOBAL)) != (IPV4_ADDR_UNICAST | IPV4_ADDR_GLOBAL))) {
		// not ANON_METHOD_KEEPTYPEASNCC/ANON_METHOD_KEEPTYPEGEONAMEID or not a global address
		if (((ipv4addrp->typeinfo & IPV4_ADDR_ANYCAST) != 0) && ((ipv4addrp->typeinfo & IPV4_ADDR_LISP) != 0)) {
			if (mask < 24) {
				mask = 24; // keeping address type
			};
			DEBUGPRINT_WA(DEBUG_libipv6addr, "Mask adjusted to: %d", mask);
		};

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
	} else if (method == ANON_METHOD_KEEPTYPEASNCC) {
		/* check for required database support */
		if (libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEASNCC_IPV4_REQ_DB) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv4addr, "anonymization method not supported, db_wrapper reports too less features");
			return(1);
		};

		DEBUGPRINT_NA(DEBUG_libipv4addr, "anonymize by keep Type/ASN/CountryCode information");

		CONVERT_IPV4ADDRP_IPADDR(ipv4addrp, ipaddr);

		if (((ipv4addrp->typeinfo & IPV4_ADDR_UNICAST) != 0) && ((ipv4addrp->typeinfo & IPV4_ADDR_LISP) != 0)) {
			as_num32_comp17 = 0x11800;
			as_num32_comp17 |= (libipv6calc_db_wrapper_registry_num_by_ipv4addr(ipv4addrp) & 0x7) << 13;
			as_num32_comp17 |= 0x000; // TODO: map LISP information into 11 LSB
		} else {
			// get AS number
			as_num32 = libipv6calc_db_wrapper_as_num32_by_addr(&ipaddr, NULL);
			DEBUGPRINT_WA(DEBUG_libipv4addr, "result of AS number  retrievement: 0x%08x (%d)", as_num32, as_num32);

			as_num32_comp17 = libipv6calc_db_wrapper_as_num32_comp17(as_num32);
			DEBUGPRINT_WA(DEBUG_libipv4addr, "result of AS number   compression: 0x%05x", as_num32_comp17);

			as_num32_decomp17 = libipv6calc_db_wrapper_as_num32_decomp17(as_num32_comp17);
			DEBUGPRINT_WA(DEBUG_libipv4addr, "result of AS number decompression: 0x%08x (%d)", as_num32_decomp17, as_num32_decomp17);
		};

		// get countrycode
		cc_index = libipv6calc_db_wrapper_cc_index_by_addr(&ipaddr, NULL);
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

		DEBUGPRINT_WA(DEBUG_libipv4addr, "result anonymized (Type/ASN/CC) IPv4 address: 0x%08x, bitcounts=%d", ipv4addr_anon, c);

		ipv4addr_setdword(ipv4addrp, ipv4addr_anon);
	} else {
		unsigned int GeonameID_type = IPV6CALC_DB_GEO_GEONAMEID_TYPE_UNKNOWN;
		uint32_t GeonameID = IPV6CALC_DB_GEO_GEONAMEID_UNKNOWN;

		/* check for required database support */
		if (libipv6calc_db_wrapper_has_features(ANON_METHOD_KEEPTYPEGEONAMEID_IPV4_REQ_DB) == 0) {
			DEBUGPRINT_NA(DEBUG_libipv4addr, "anonymization method not supported, db_wrapper reports too less features");
			return(1);
		};

		DEBUGPRINT_NA(DEBUG_libipv4addr, "anonymize by keep GeonameID information");

		CONVERT_IPV4ADDRP_IPADDR(ipv4addrp, ipaddr);

		if (((ipv4addrp->typeinfo & IPV4_ADDR_UNICAST) != 0) && ((ipv4addrp->typeinfo & IPV4_ADDR_LISP) != 0)) {
			GeonameID_type = 0x7;
			GeonameID = 0x11800;
			GeonameID |= (libipv6calc_db_wrapper_registry_num_by_ipv4addr(ipv4addrp) & 0x7) << 13;
			GeonameID |= 0x000; // TODO: map LISP information into 11 LSB
		} else {
			// get GeonameID
			GeonameID_type |= IPV6CALC_DB_GEO_GEONAMEID_TYPE_FLAG_24BIT;
			GeonameID = libipv6calc_db_wrapper_GeonameID_by_addr(&ipaddr, NULL, &GeonameID_type);
			DEBUGPRINT_WA(DEBUG_libipv4addr, "result of GeonameID retrievement: %d (0x%08x) (source: %d)", GeonameID, GeonameID, GeonameID_type);

			if (GeonameID > 0xffffff) {
				ERRORPRINT_WA("result of GeonameID retrievement: %d (source: %d) above 24-bit limit (fix code)", GeonameID, GeonameID_type);
			};
		};

		// 0-3   ( 4 bits) : prefix 0xf0
		// 4     ( 1 bit ) : parity bit (even parity)
		// 5-7   ( 3 bits) : GeoNameID source - or 0x7 in case of LISP
		// 8-31  (24 bits) : GeonameID (limited to 24 bit) - or Registry in case of LISP
		ipv4addr_anon = 0xf0000000 | ((GeonameID_type << 24) & 0x07000000 ) | (GeonameID & 0x00ffffff);

		// create parity bits
		c = 0;
		p = 0x00000001;
		for (i = 0; i < 27; i++) {
			if ((ipv4addr_anon & p) != 0) {
				c++;
			};
			p <<= 1;
		};

		ipv4addr_anon |= ((c & 0x1) ^ 0x0) << 27;

		DEBUGPRINT_WA(DEBUG_libipv4addr, "result anonymized (Type/GeonameID) IPv4 address: 0x%08x, bitcounts=%d", ipv4addr_anon, c);

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
	if ((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED) == 0) {
		return(ASNUM_AS_UNKNOWN);
	} else if ((ipv4addrp->typeinfo & IPV4_ADDR_LISP) != 0) {
		// anonymized LISP can't save AS number
		return(ASNUM_AS_UNKNOWN);
 	} else if ((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED_GEONAMEID) != 0) {
		// GeonameID can't save AS number
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
	if ((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED) == 0) {
		return(COUNTRYCODE_INDEX_UNKNOWN);
 	} else if ((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED_GEONAMEID) != 0) {
		// GeonameID can't save CountryCode
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

	filter->filter_typeinfo.active = 0;
	filter->filter_typeinfo.typeinfo_must_have = 0;
	filter->filter_typeinfo.typeinfo_may_not_have = 0;

	libipv6calc_filter_clear_db_cc(&filter->filter_db_cc);
	libipv6calc_filter_clear_db_asn(&filter->filter_db_asn);
	libipv6calc_filter_clear_db_registry(&filter->filter_db_registry);

        filter->filter_addr.active = 0;
        filter->filter_addr.addr_must_have_max = 0;
        filter->filter_addr.addr_may_not_have_max = 0;

	return;
};


/*
 * parse filter IPv4 address
 *
 * in : *filter    = filter structure
 * ret: 0:found 1:skip 2:problem
 */
int ipv4addr_filter_parse(s_ipv6calc_filter_ipv4addr *filter, const char *token) {
	int i, result = 1, negate = 0, offset = 0, r;
	const char *prefix = "ipv4";
	const char *prefixdot = "ipv4.";
	const char *prefixdbdot = "db.";
	const char *prefixaddreq = "addr=";
	const char *prefixaddreq_le = "addr<=";
	const char *prefixaddreq_lt = "addr<";
	const char *prefixaddreq_ge = "addr>=";
	const char *prefixaddreq_gt = "addr>";
	const char *prefixaddreq_le2 = "addr=le=";
	const char *prefixaddreq_lt2 = "addr=lt=";
	const char *prefixaddreq_ge2 = "addr=ge=";
	const char *prefixaddreq_gt2 = "addr=gt=";
	ipv6calc_ipv4addr ipv4addr;
	char resultstring[IPV6CALC_STRING_MAX];
	int db = 0;
	int addr_test_method = IPV6CALC_TEST_NONE;

	if (token == NULL) {
		return (result);
	};

	DEBUGPRINT_WA(DEBUG_libipv4addr, "input: %s", token);

	if (token[0] == '^') {
		DEBUGPRINT_WA(DEBUG_libipv4addr, "found negate prefix in token: %s", token);

		negate = 1;
		offset += 1;
	};

	// typeinfo
	if (strcmp(token + offset, prefix) == 0) {
		/* any */
		if (negate == 1) {
			filter->filter_typeinfo.typeinfo_may_not_have = ~IPV4_ADDR_ANY;
		} else {
			filter->filter_typeinfo.typeinfo_must_have = IPV4_ADDR_ANY;
		};
		filter->filter_typeinfo.active = 1;
		filter->active = 1;
		result = 0;
		goto END_ipv4addr_filter_parse;

	} else if (strncmp(token + offset, prefixdot, strlen(prefixdot)) == 0) {
		/* prefix with dot found */
		offset += strlen(prefixdot);
		result = 2; /* token with prefix, result into problem if not found */

		DEBUGPRINT_WA(DEBUG_libipv4addr, "token with prefix, suffix: %s", token + offset);
	};

	if (strncmp(token + offset, prefixaddreq_le, strlen(prefixaddreq_le)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv4addr, "found 'addr<=' prefix in token: %s", token);
		offset += strlen(prefixaddreq_le);
		addr_test_method = IPV6CALC_TEST_LE;

	} else if (strncmp(token + offset, prefixaddreq_lt, strlen(prefixaddreq_lt)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv4addr, "found 'addr<' prefix in token: %s", token);
		offset += strlen(prefixaddreq_lt);
		addr_test_method = IPV6CALC_TEST_LT;

	} else if (strncmp(token + offset, prefixaddreq_ge, strlen(prefixaddreq_ge)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv4addr, "found 'addr>=' prefix in token: %s", token);
		offset += strlen(prefixaddreq_ge);
		addr_test_method = IPV6CALC_TEST_GE;

	} else if (strncmp(token + offset, prefixaddreq_gt, strlen(prefixaddreq_gt)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv4addr, "found 'addr>' prefix in token: %s", token);
		offset += strlen(prefixaddreq_gt);
		addr_test_method = IPV6CALC_TEST_GT;

	} else if (strncmp(token + offset, prefixaddreq_le2, strlen(prefixaddreq_le2)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv4addr, "found 'addr=le=' prefix in token: %s", token);
		offset += strlen(prefixaddreq_le2);
		addr_test_method = IPV6CALC_TEST_LE;

	} else if (strncmp(token + offset, prefixaddreq_lt2, strlen(prefixaddreq_lt2)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv4addr, "found 'addr=lt=' prefix in token: %s", token);
		offset += strlen(prefixaddreq_lt2);
		addr_test_method = IPV6CALC_TEST_LT;

	} else if (strncmp(token + offset, prefixaddreq_ge2, strlen(prefixaddreq_ge2)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv4addr, "found 'addr=ge=' prefix in token: %s", token);
		offset += strlen(prefixaddreq_ge2);
		addr_test_method = IPV6CALC_TEST_GE;

	} else if (strncmp(token + offset, prefixaddreq_gt2, strlen(prefixaddreq_gt2)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv4addr, "found 'addr=gt=' prefix in token: %s", token);
		offset += strlen(prefixaddreq_gt2);
		addr_test_method = IPV6CALC_TEST_GT;

	} else if (strncmp(token + offset, prefixaddreq, strlen(prefixaddreq)) == 0) {
		/* prefixaddr with = found */
		DEBUGPRINT_WA(DEBUG_libipv4addr, "found 'addr=' prefix in token: %s", token);
		offset += strlen(prefixaddreq);
		addr_test_method = IPV6CALC_TEST_PREFIX;

	} else if (strncmp(token + offset, prefixdbdot, strlen(prefixdbdot)) == 0) {
		/* prefixdb with dot found */
		DEBUGPRINT_WA(DEBUG_libipv4addr, "found 'db.' prefix in token: %s", token);
		db = 1;

	} else if (strstr(token, ".") != NULL) {
		/* other prefix */
		DEBUGPRINT_WA(DEBUG_libipv4addr, "prefix did not match: %s", token + offset);
		return(1);
	};


	if ((db == 0) && (addr_test_method == IPV6CALC_TEST_NONE)) {
		// typeinfo token
		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_ipv4addrtypestrings); i++ ) {
			DEBUGPRINT_WA(DEBUG_libipv4addr, "check token against: %s", ipv6calc_ipv4addrtypestrings[i].token);

			if (strcmp(ipv6calc_ipv4addrtypestrings[i].token, token + offset) == 0) {
				DEBUGPRINT_WA(DEBUG_libipv4addr, "token match: %s", ipv6calc_ipv4addrtypestrings[i].token);

				if (negate == 1) {
					filter->filter_typeinfo.typeinfo_may_not_have |= ipv6calc_ipv4addrtypestrings[i].number;
				} else {
					filter->filter_typeinfo.typeinfo_must_have |= ipv6calc_ipv4addrtypestrings[i].number;
				};
				filter->filter_typeinfo.active = 1;
				filter->active = 1;
				result = 0;
				break;
			};
		};
	};

	if (db == 1) {
		// DB CC filter
		r = libipv6calc_db_cc_filter_parse(&filter->filter_db_cc, token + offset, negate);
		if (r == 0) {
			result = 0;
			filter->active = 1;
		};

		// DB ASN filter
		r = libipv6calc_db_asn_filter_parse(&filter->filter_db_asn, token + offset, negate);
		if (r == 0) {
			result = 0;
			filter->active = 1;
		};

		// DB Registry filter
		r = libipv6calc_db_registry_filter_parse(&filter->filter_db_registry, token + offset, negate);
		if (r == 0) {
			result = 0;
			filter->active = 1;
		};
	};

	if (addr_test_method != IPV6CALC_TEST_NONE) {
		DEBUGPRINT_WA(DEBUG_libipv4addr, "try to parse IPv4 address: %s", token + offset);

		r = addr_to_ipv4addrstruct(token + offset, resultstring, sizeof(resultstring), &ipv4addr);

		if (r == 0) {
			DEBUGPRINT_WA(DEBUG_libipv4addr, "successfully parsed IPv4 address: %s", token + offset);

			// store address test method
			ipv4addr.test_mode = addr_test_method;

			if (negate == 1) {
				if (filter->filter_addr.addr_may_not_have_max < IPV6CALC_FILTER_IPV4ADDR) {
					ipv4addr_copy(&filter->filter_addr.ipv4addr_may_not_have[filter->filter_addr.addr_may_not_have_max], &ipv4addr);
        				filter->filter_addr.addr_may_not_have_max++;
					filter->filter_addr.active = 1;
					filter->active = 1;
					result = 0;
				} else {
					ERRORPRINT_WA("filter token 'addr=' maxmimum reached for 'may not have': %d", filter->filter_addr.addr_may_not_have_max);
				};
			} else {
				if (filter->filter_addr.addr_must_have_max < IPV6CALC_FILTER_IPV4ADDR) {
					ipv4addr_copy(&filter->filter_addr.ipv4addr_must_have[filter->filter_addr.addr_must_have_max], &ipv4addr); 
        				filter->filter_addr.addr_must_have_max++;
					filter->filter_addr.active = 1;
					filter->active = 1;
					result = 0;
				} else {
					ERRORPRINT_WA("filter token 'addr=' maxmimum reached for 'must have': %d", filter->filter_addr.addr_must_have_max);
				};
			};
		};
	};

	if (result != 0) {
		DEBUGPRINT_WA(DEBUG_libipv4addr, "token not supported: %s",token);
		return (result);
	};

END_ipv4addr_filter_parse:
	return (result);
};


/*
 * check filter IPv4
 *
 * in : *filter    = filter structure
 * ret: 0:ok 1:problem
 */
int ipv4addr_filter_check(const s_ipv6calc_filter_ipv4addr *filter) {
	int result = 0, r, i;
	char resultstring[IPV6CALC_STRING_MAX];

	DEBUGPRINT_WA(DEBUG_libipv4addr, "ipv4 filter general active         : %d", filter->active);

	DEBUGPRINT_WA(DEBUG_libipv4addr, "ipv4 filter 'typeinfo' active      : %d", filter->filter_typeinfo.active);
	if (filter->filter_typeinfo.active > 0) {
		DEBUGPRINT_WA(DEBUG_libipv4addr, "ipv4 filter 'typeinfo/must_have'   : 0x%08x", filter->filter_typeinfo.typeinfo_must_have);
		DEBUGPRINT_WA(DEBUG_libipv4addr, "ipv4 filter 'typeinfo/may_not_have': 0x%08x", filter->filter_typeinfo.typeinfo_may_not_have);
	};

	DEBUGPRINT_WA(DEBUG_libipv4addr, "ipv4 filter 'addr' active          : %d", filter->filter_addr.active);
	if (filter->filter_addr.active > 0) {
		if (filter->filter_addr.addr_must_have_max > 0) {
			for (i = 0; i < filter->filter_addr.addr_must_have_max; i++) {
				libipv4addr_ipv4addrstruct_to_string(&filter->filter_addr.ipv4addr_must_have[i], resultstring, sizeof(resultstring), 0);
				DEBUGPRINT_WA(DEBUG_libipv4addr, "ipv4 filter 'addr/must_have'       : %s", resultstring);
			};
		};
		if (filter->filter_addr.addr_may_not_have_max > 0) {
			for (i = 0; i < filter->filter_addr.addr_may_not_have_max; i++) {
				libipv4addr_ipv4addrstruct_to_string(&filter->filter_addr.ipv4addr_may_not_have[i], resultstring, sizeof(resultstring), 0);
				DEBUGPRINT_WA(DEBUG_libipv4addr, "ipv4 filter 'addr/may_not_have'    : %s", resultstring);
			};
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv4addr, "ipv4 filter 'db.cc' active         : %d", filter->filter_db_cc.active);
	if (filter->filter_db_cc.active > 0) {
		r = libipv6calc_db_cc_filter_check(&filter->filter_db_cc, IPV6CALC_PROTO_IPV4);
		if (r > 0) { result = 1; };
	};

	DEBUGPRINT_WA(DEBUG_libipv4addr, "ipv4 filter 'db.asn' active        : %d", filter->filter_db_asn.active);
	if (filter->filter_db_asn.active > 0) {
		r = libipv6calc_db_asn_filter_check(&filter->filter_db_asn, IPV6CALC_PROTO_IPV4);
		if (r > 0) { result = 1; };
	};

	DEBUGPRINT_WA(DEBUG_libipv4addr, "ipv4 filter 'db.registry' active   : %d", filter->filter_db_registry.active);
	if (filter->filter_db_registry.active > 0) {
		r = libipv6calc_db_registry_filter_check(&filter->filter_db_registry, IPV6CALC_PROTO_IPV4);
		if (r > 0) { result = 1; };
	};

//END_ipv4addr_filter_check:
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
	int result = 0, r, i, t;

	if (filter->active == 0) {
		DEBUGPRINT_NA(DEBUG_libipv4addr, "no filter active (SKIP)");
		return (1);
	};

	DEBUGPRINT_NA(DEBUG_libipv4addr, "start");

	if (filter->filter_typeinfo.active > 0) {
		if (ipv4addrp->flag_typeinfo == 0) {
			fprintf(stderr, "FATAL error, typeinfo not valid - FIX CODE of caller\n");
			exit(2);
		};

		DEBUGPRINT_WA(DEBUG_libipv4addr, "compare typeinfo against must_have: 0x%08x/0x%08x", ipv4addrp->typeinfo, filter->filter_typeinfo.typeinfo_must_have);

		if ((ipv4addrp->typeinfo & filter->filter_typeinfo.typeinfo_must_have) != filter->filter_typeinfo.typeinfo_must_have) {
			/* no match */
			result = 1;
		} else {
			if ((ipv4addrp->typeinfo & filter->filter_typeinfo.typeinfo_may_not_have) != 0) {
				result = 1;
			};
		};
	};

	if (filter->filter_addr.active > 0) {
		if (filter->filter_addr.addr_must_have_max > 0) {
			DEBUGPRINT_NA(DEBUG_libipv4addr, "compare against ipv4addr/must_have");
			r = 1;
			for (i = 0; i < filter->filter_addr.addr_must_have_max; i++) {
				t = ipv4addr_compare(ipv4addrp, &filter->filter_addr.ipv4addr_must_have[i],
					(filter->filter_addr.ipv4addr_must_have[i].test_mode == IPV6CALC_TEST_PREFIX) ? 1 : 0);

				switch (filter->filter_addr.ipv4addr_must_have[i].test_mode) {
					case IPV6CALC_TEST_PREFIX:
						if (t != 0) { r = 0; }; break;

					case IPV6CALC_TEST_LE:
						if (t >  0) { r = 0; }; break;

					case IPV6CALC_TEST_LT:
						if (t >= 0) { r = 0; }; break;

					case IPV6CALC_TEST_GE:
						if (t <  0) { r = 0; }; break;

					case IPV6CALC_TEST_GT:
						if (t <= 0) { r = 0; }; break;

					default:
						ERRORPRINT_WA("unsupported test mode (FIX CODE): %d", filter->filter_addr.ipv4addr_must_have[i].test_mode);
						break;
				};
			};
			if (r == 0) {
				/* no match */
				result = 1;
			};
		};
		if (filter->filter_addr.addr_may_not_have_max > 0) {
			DEBUGPRINT_NA(DEBUG_libipv4addr, "compare against ipv4addr/may_not_have");
			r = 0;
			for (i = 0; i < filter->filter_addr.addr_may_not_have_max; i++) {
				t = ipv4addr_compare(ipv4addrp, &filter->filter_addr.ipv4addr_may_not_have[i],
					(filter->filter_addr.ipv4addr_may_not_have[i].test_mode == IPV6CALC_TEST_PREFIX) ? 1 : 0);

				switch (filter->filter_addr.ipv4addr_may_not_have[i].test_mode) {
					case IPV6CALC_TEST_PREFIX:
						if (t == 0) { r = 1; }; break;

					case IPV6CALC_TEST_LE:
						if (t <= 0) { r = 1; }; break;

					case IPV6CALC_TEST_LT:
						if (t <  0) { r = 1; }; break;

					case IPV6CALC_TEST_GE:
						if (t >= 0) { r = 1; }; break;

					case IPV6CALC_TEST_GT:
						if (t >  0) { r = 1; }; break;

					default:
						ERRORPRINT_WA("unsupported test mode (FIX CODE): %d", filter->filter_addr.ipv4addr_may_not_have[i].test_mode);
						break;
				};
			};
			if (r == 1) {
				/* match may_not_have*/
				result = 1;
			};
		};
	};

	if (filter->filter_db_cc.active > 0) {
		uint16_t cc_index = libipv4addr_cc_index_by_addr(ipv4addrp, NULL);

		if (libipv6calc_db_cc_filter(cc_index, &filter->filter_db_cc) > 0) {
			/* no match */
			result = 1;
		};
	};

	if (filter->filter_db_asn.active > 0) {
		uint32_t asn = libipv4addr_as_num32_by_addr(ipv4addrp, NULL);

		if (filter->filter_db_asn.active > 0) {
			if (libipv6calc_db_asn_filter(asn, &filter->filter_db_asn) > 0) {
				/* no match */
				result = 1;
			};
		};
	};

	if (filter->filter_db_registry.active > 0) {
		int registry = libipv4addr_registry_num_by_addr(ipv4addrp);

		if (filter->filter_db_registry.active > 0) {
			if (libipv6calc_db_registry_filter(registry, &filter->filter_db_registry) > 0) {
				/* no match */
				result = 1;
			};
		};
	};

	return (result);
};


/*
 * country code index of IPv4 address
 *
 * in : *ipv4addrp = IPv4 address structure
 * out: country code index
 */
uint16_t libipv4addr_cc_index_by_addr(const ipv6calc_ipv4addr *ipv4addrp, unsigned int *data_source_ptr) {
	uint16_t cc_index = COUNTRYCODE_INDEX_UNKNOWN;
	ipv6calc_ipaddr ipaddr;

	DEBUGPRINT_NA(DEBUG_libipv4addr, "start");

	if ((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED) != 0) {
		cc_index = ipv4addr_anonymized_get_cc_index(ipv4addrp);
	} else if ((ipv4addrp->typeinfo & IPV4_ADDR_RESERVED) != 0) {
		cc_index = COUNTRYCODE_INDEX_UNKNOWN;
	} else if ((ipv4addrp->typeinfo & IPV4_ADDR_GLOBAL) == 0) {
		cc_index = COUNTRYCODE_INDEX_UNKNOWN;
	} else {
		if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_CC) == 1) {
			CONVERT_IPV4ADDRP_IPADDR(ipv4addrp, ipaddr);
			cc_index = libipv6calc_db_wrapper_cc_index_by_addr(&ipaddr, data_source_ptr);
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv4addr, "cc_index=%d (0x%03x)", cc_index, cc_index);
	return(cc_index);
};


/*
 * 32-bit AS number of IPv4 address
 *
 * in : *ipv4addrp = IPv4 address structure
 * out: 32-bit AS number
 */
uint32_t libipv4addr_as_num32_by_addr(const ipv6calc_ipv4addr *ipv4addrp, unsigned int *data_source_ptr) {
	uint32_t as_num32 = ASNUM_AS_UNKNOWN;
	ipv6calc_ipaddr ipaddr;

	DEBUGPRINT_WA(DEBUG_libipv4addr, "typeinfo=%08x", ipv4addrp->typeinfo);

	if ((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED) != 0) {
		as_num32 = ipv4addr_anonymized_get_as_num32(ipv4addrp);
	} else {
		if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_AS) == 1) {
			CONVERT_IPV4ADDRP_IPADDR(ipv4addrp, ipaddr);
			as_num32 = libipv6calc_db_wrapper_as_num32_by_addr(&ipaddr, data_source_ptr);
		};
	};

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
	uint32_t as_num32;
	uint16_t cc_index;

	if ((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED) != 0) {
		if( (ipv4addrp->typeinfo & IPV4_ADDR_LISP) != 0) {
			DEBUGPRINT_NA(DEBUG_libipv4addr, "IPv4 is anonymized LISP, extract registry from anonymized data");
			// extract registry
			uint32_t ipv4addr = ipv4addr_getdword(ipv4addrp);
			registry = (ipv4addr >> 13) & 0x7;
		} else if( (ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED_GEONAMEID) != 0) {
			// GeonameID can't save Registry (exception: LISP)
		} else {
			DEBUGPRINT_NA(DEBUG_libipv4addr, "IPv4 is anonymized, extract registry from anonymized data");
			// ASN -> Registry
			// CC  -> Registry

			/* retrieve registry via AS number from anonymized address (simple) */
			as_num32 = libipv4addr_as_num32_by_addr(ipv4addrp, NULL);
			if (as_num32 != ASNUM_AS_UNKNOWN) {
				registry = libipv6calc_db_wrapper_registry_num_by_as_num32(as_num32);
			};
			if ((as_num32 == ASNUM_AS_UNKNOWN) || (registry == IPV4_ADDR_REGISTRY_ARIN)) {
				/* retrieve registry via cc_index from anonymized address (simple, fallback) */
				cc_index = libipv4addr_cc_index_by_addr(ipv4addrp, NULL);
				registry = libipv6calc_db_wrapper_registry_num_by_cc_index(cc_index);
			};
		};
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


/*
 * GeonameID of IPv4 address
 *
 * in : *ipv4addrp = IPv4 address structure
 * mod: GeonameID_type_ptr
 * out: GeonameID
 */
uint32_t libipv4addr_GeonameID_by_addr(const ipv6calc_ipv4addr *ipv4addrp, unsigned int *data_source_ptr, unsigned int *GeonameID_type_ptr) {
	uint32_t GeonameID = IPV6CALC_DB_GEO_GEONAMEID_UNKNOWN;
	unsigned int GeonameID_type = IPV6CALC_DB_GEO_GEONAMEID_TYPE_UNKNOWN;
	unsigned int data_source = IPV6CALC_DB_SOURCE_UNKNOWN;
	ipv6calc_ipaddr ipaddr;
	uint32_t ipv4 = ipv4addr_getdword(ipv4addrp);

	if ((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED) != 0) {
		if (((ipv4addrp->typeinfo & IPV4_ADDR_ANONYMIZED_GEONAMEID) != 0) \
		    && ((ipv4addrp->typeinfo & IPV4_ADDR_LISP) == 0)) {
			// GeonameID included
			GeonameID = ipv4 & 0x00ffffff; // 24 bit LSB
			GeonameID_type = (ipv4 & 0x07000000) >> 24;
			DEBUGPRINT_WA(DEBUG_libipv4addr, "result of anonymized IPv4 GeonameID retrievement: %d (0x%08x) (source: %02x) (ipv4=%08x)", GeonameID, GeonameID, GeonameID_type, ipv4);
		};
	} else {
		if (libipv6calc_db_wrapper_has_features(IPV6CALC_DB_IPV4_TO_GEONAMEID) == 1) {
			// get GeonameID
			CONVERT_IPV4ADDRP_IPADDR(ipv4addrp, ipaddr);
			GeonameID = libipv6calc_db_wrapper_GeonameID_by_addr(&ipaddr, &data_source, &GeonameID_type);
			DEBUGPRINT_WA(DEBUG_libipv4addr, "result of GeonameID retrievement: %d (0x%08x) (source: %02x)", GeonameID, GeonameID, GeonameID_type);
		} else {
			DEBUGPRINT_NA(DEBUG_libipv4addr, "No support available for IPV6CALC_DB_IPV4_TO_GEONAMEID");
		};
	};

	DEBUGPRINT_WA(DEBUG_libipv4addr, "GeonameID=%d (0x%x)", GeonameID, GeonameID);

	if (data_source_ptr != NULL) {
		*data_source_ptr = data_source;
	};

	if (GeonameID_type_ptr != NULL) {
		*GeonameID_type_ptr = GeonameID_type;
	};

	return(GeonameID);
};
