/*
 * libipv6addr: Function libary for IPv6 storage
 *
 * Version:		$Id: libipv6addr.c,v 1.2 2001/10/09 07:03:10 peter Exp $
 * 
 * Author:		Peter Bieringer <pb@bieringer.de>
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "libipv6addr.h"

/* array of numerical types */
unsigned int ipv6addr_typesnum[IPV6INFO_NUM] = {
    IPV6_ADDR_UNICAST,
	IPV6_ADDR_MULTICAST,
	IPV6_ADDR_ANYCAST,
	IPV6_ADDR_LOOPBACK,
	IPV6_ADDR_LINKLOCAL,
	IPV6_ADDR_SITELOCAL,
	IPV6_ADDR_COMPATv4,
	IPV6_ADDR_MAPPED,
	IPV6_ADDR_RESERVED,
	IPV6_NEW_ADDR_6TO4,
	IPV6_NEW_ADDR_6BONE, 
	IPV6_NEW_ADDR_AGU,
	IPV6_NEW_ADDR_UNSPECIFIED
};

char *ipv6addr_typesstring[IPV6INFO_NUM] = {
    TXT_IPV6_ADDR_UNICAST,
	TXT_IPV6_ADDR_MULTICAST,
	TXT_IPV6_ADDR_ANYCAST,
	TXT_IPV6_ADDR_LOOPBACK,
	TXT_IPV6_ADDR_LINKLOCAL,
	TXT_IPV6_ADDR_SITELOCAL,
	TXT_IPV6_ADDR_COMPATv4,
	TXT_IPV6_ADDR_MAPPED,
	TXT_IPV6_ADDR_RESERVED,
	TXT_IPV6_NEW_ADDR_6TO4,
	TXT_IPV6_NEW_ADDR_6BONE,
	TXT_IPV6_NEW_ADDR_AGU,
	TXT_IPV6_NEW_ADDR_UNSPECIFIED
};


/* function returns an octett of an IPv6 address
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 * in: numoctett  = number of octett (0 = MSB, 15 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_getoctett"
unsigned int ipv6addr_getoctett(ipv6calc_ipv6addr *ipv6addrp, int numoctett) {
	unsigned int retval;
	
	if ( ( numoctett < 0 ) || ( numoctett > 15 ) ) {
		fprintf(stderr, "%s: given ocett number '%d' is out of range!\n", DEBUG_function_name, numoctett);
		exit(2);
	};

	retval = ipv6addrp->in6_addr.s6_addr[numoctett];

	return (retval);
};
#undef DEBUG_function_name


/* function returns a word of an IPv6 address
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 * in: numword   = number of word (0 = MSB, 7 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_getword"
unsigned int ipv6addr_getword(ipv6calc_ipv6addr *ipv6addrp, int numword) {
	unsigned int retval;
	
	if ( ( numword < 0 ) || ( numword > 7 ) ) {
		fprintf(stderr, "%s: given word number '%d' is out of range!\n", DEBUG_function_name, numword);
		exit(2);
	};

	retval = ( ipv6addrp->in6_addr.s6_addr[numword * 2] << 8 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numword * 2 + 1] );

	return (retval);
};
#undef DEBUG_function_name


/* function returns a dword of an IPv6 address
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 * in: numdword  = number of word (0 = MSB, 3 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_getdword"
unsigned int ipv6addr_getdword(ipv6calc_ipv6addr *ipv6addrp, int numdword) {
	unsigned int retval;
	
	if ( ( numdword < 0 ) || ( numdword > 3 ) ) {
		fprintf(stderr, "%s: given dword number '%d' is out of range!\n", DEBUG_function_name, numdword);
		exit(2);
	};

	retval = ( ipv6addrp->in6_addr.s6_addr[numdword * 4] << 24 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numdword * 4 + 1] << 16 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numdword * 4 + 2] << 8 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numdword * 4 + 3] ); 

	return (retval);
};
#undef DEBUG_function_name


/* function sets an octett of an IPv6 address
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 * in: numoctett   = number of word (0 = MSB, 15 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_setoctett"
void ipv6addr_setoctett(ipv6calc_ipv6addr *ipv6addrp, int numoctett, unsigned int value) {
	
	if ( ( numoctett < 0 ) || ( numoctett > 7 ) ) {
		fprintf(stderr, "%s: given octett number '%d' is out of range!\n", DEBUG_function_name, numoctett);
		exit(2);
	};
	
	if ( ( value < 0 ) || ( value > 0x0000000ff ) ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(2);
	}; 

	ipv6addrp->in6_addr.s6_addr[numoctett] = value;

	return;
};


#undef DEBUG_function_name
/* function sets a word of an IPv6 address
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 * in: numword   = number of word (0 = MSB, 7 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_setword"
void ipv6addr_setword(ipv6calc_ipv6addr *ipv6addrp, int numword, unsigned int value) {
	
	if ( ( numword < 0 ) || ( numword > 7 ) ) {
		fprintf(stderr, "%s: given word number '%d' is out of range!\n", DEBUG_function_name, numword);
		exit(2);
	};
	
	if ( ( value < 0 ) || ( value > 0x0000ffff ) ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(2);
	}; 

	ipv6addrp->in6_addr.s6_addr[numword * 2    ] = ( value & 0x0000ff00 ) >>  8;
	ipv6addrp->in6_addr.s6_addr[numword * 2 + 1] = ( value & 0x000000ff )      ;

	return;
};
#undef DEBUG_function_name


/* function sets a dword of an IPv6 address
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 * in: numdword  = number of word (0 = MSB, 3 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_setdword"
void ipv6addr_setdword(ipv6calc_ipv6addr *ipv6addrp, int numdword, unsigned int value) {
	
	if ( ( numdword < 0 ) || ( numdword > 3 ) ) {
		fprintf(stderr, "%s: given dword number '%d' is out of range!\n", DEBUG_function_name, numdword);
		exit(2);
	};

	if ( ( value < 0 ) || ( value > 0xffffffff ) ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(2);
	}; 

	ipv6addrp->in6_addr.s6_addr[numdword * 4    ] = ( value & 0xff000000 ) >> 24;
	ipv6addrp->in6_addr.s6_addr[numdword * 4 + 1] = ( value & 0x00ff0000 ) >> 16;
	ipv6addrp->in6_addr.s6_addr[numdword * 4 + 2] = ( value & 0x0000ff00 ) >>  8;
	ipv6addrp->in6_addr.s6_addr[numdword * 4 + 3] = ( value & 0x000000ff )      ;

	return;
};
#undef DEBUG_function_name


/* function clears the IPv6 structure
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_clear"
void ipv6addr_clear(ipv6calc_ipv6addr *ipv6addrp) {
	int i;

	for (i = 0; i <= 15; i++) {
		ipv6addrp->in6_addr.s6_addr[i] = 0;
	};
	
	/* Clear IPv6 address scope */
	ipv6addrp->scope = 0;
	
	return;
};
#undef DEBUG_function_name


/* function gets type of an IPv6 address
 *
 * with credits to kernel and USAGI developer team
 * basic code was taken from "kernel/net/ipv6/addrconf.c"
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 * in: numdword  = number of word (0 = MSB, 3 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */

unsigned int ipv6addr_gettype(ipv6calc_ipv6addr *ipv6addrp) {
	unsigned int type = 0;
	unsigned int st, st1, st2, st3;

	st = ipv6addr_getdword(ipv6addrp, 0); /* 32 MSB */
	st1 = ipv6addr_getdword(ipv6addrp, 1);
	st2 = ipv6addr_getdword(ipv6addrp, 2);
	st3 = ipv6addr_getdword(ipv6addrp, 3); /* 32 LSB */

	/* unspecified address */
	if ( (st == 0) & (st1 == 0) & (st2 == 0) & (st3 == 0) ) {
		type |= IPV6_NEW_ADDR_UNSPECIFIED;
		return (type);
	};

	/* address space information  */
	if ((st & (0xFFFF0000)) == (0x3FFE0000)) {
		/* 3ffe:... experimental 6bone*/
		type |= IPV6_NEW_ADDR_6BONE;
	};

	if ((st & (0xFFFF0000)) == (0x20020000)) {
		/* 2002:... 6to4 tunneling */
		type |= IPV6_NEW_ADDR_6TO4;
	};

	/* Consider all addresses with the first three bits different of
	   000 and 111 as unicasts.
	 */

	/* original from kernel, only | and return changed */	
	if ((st & (0xE0000000)) != (0x00000000) &&
	    (st & (0xE0000000)) != (0xE0000000)) {
		type |= IPV6_ADDR_UNICAST;
		return (type);
	};

	if ((st & (0xFF000000)) == (0xFF000000)) {
		type |= IPV6_ADDR_MULTICAST;

		switch((st & (0x00FF0000))) {
			case (0x00010000):
				type |= IPV6_ADDR_LOOPBACK;
				break;

			case (0x00020000):
				type |= IPV6_ADDR_LINKLOCAL;
				break;

			case (0x00050000):
				type |= IPV6_ADDR_SITELOCAL;
				break;
		};
		return (type);
	}
	
	if ((st & (0xFFC00000)) == (0xFE800000)) {
		type |=  IPV6_ADDR_LINKLOCAL | IPV6_ADDR_UNICAST;
		return (type);
	};

	if ((st & (0xFFC00000)) == (0xFEC00000)) {
		type |= IPV6_ADDR_SITELOCAL | IPV6_ADDR_UNICAST;
		return (type);
	}; 

	if ((st | st1) == 0) {
		if (st2 == 0) {
			if (st3 == 0) {
				type |= IPV6_ADDR_ANY;
				return (type);
			};

			if (st3 == (0x00000001)) {
				type |= IPV6_ADDR_LOOPBACK | IPV6_ADDR_UNICAST;
				return (type);
			};

			type |= IPV6_ADDR_COMPATv4 | IPV6_ADDR_UNICAST;
			return (type);
		}

		if (st2 == (0x0000ffff))
			type |= IPV6_ADDR_MAPPED;
			return (type);
	}

	type |= IPV6_ADDR_RESERVED;
	return (type);
};
