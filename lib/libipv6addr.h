/*
 * Project    : ipv6calc
 * File       : libipv6addr.h
 * Version    : $Id: libipv6addr.h,v 1.3 2002/03/20 23:35:51 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
 *
 * Information:
 *  Header file for libipv6addr.c
 */ 

#include "libipv6calc.h"
#include <netinet/in.h>
#include <ctype.h>

/* typedefs */

#ifndef _libipv6addr_h

#define _libipv6addr_h 1

/* IPv6 address storage structure */
typedef struct {
	struct in6_addr in6_addr;	/* in6_addr structure */
	uint8_t prefixlength;	/* prefix length (0-128) 8 bit*/
	int flag_prefixuse;		/* =1 prefix length in use */
	unsigned int scope;		/* address scope value 16 bit*/
	uint8_t bit_start;	/* start of bit */
	uint8_t bit_end;		/* end of bit */
	int flag_startend_use;		/* =1 start or end of bit in use */
	int flag_valid;			/* address structure filled */
} ipv6calc_ipv6addr;

/* IPv6 address type definitions 
 * with credits to kernel and USAGI developer team
 * basic information was taken from "kernel/include/net/ipv6.h"
 */

/*
 *	Addr type
 *	
 *	type	-	unicast | multicast | anycast
 *	scope	-	local	| site	    | global
 *	v4	-	compat
 *	v4mapped
 *	any
 *	loopback
 */

#define IPV6_ADDR_ANY				0x0000U

#define IPV6_ADDR_UNICAST			0x0001U	
#define IPV6_ADDR_MULTICAST			0x0002U	
#define IPV6_ADDR_ANYCAST			0x0004U

#define IPV6_ADDR_LOOPBACK			0x0010U
#define IPV6_ADDR_LINKLOCAL			0x0020U
#define IPV6_ADDR_SITELOCAL			0x0040U

#define IPV6_ADDR_COMPATv4			0x0080U

#define IPV6_ADDR_SCOPE_MASK			0x00f0U

#define IPV6_ADDR_MAPPED			0x1000U
#define IPV6_ADDR_RESERVED			0x2000U	/* reserved address space */

#define IPV6_NEW_ADDR_6TO4			0x00010000U
#define IPV6_NEW_ADDR_6BONE			0x00020000U
#define IPV6_NEW_ADDR_AGU			0x00040000U
#define IPV6_NEW_ADDR_UNSPECIFIED		0x00080000U
#define IPV6_NEW_ADDR_SOLICITED_NODE		0x00100000U
#define IPV6_NEW_ADDR_ISATAP			0x00200000U
#define IPV6_NEW_ADDR_PRODUCTIVE		0x00400000U

/* Registries */
#define IPV6_ADDR_REGISTRY_6BONE	1
#define IPV6_ADDR_REGISTRY_IANA		2
#define IPV6_ADDR_REGISTRY_APNIC	3
#define IPV6_ADDR_REGISTRY_ARIN		4
#define IPV6_ADDR_REGISTRY_RIPE		5
#define IPV6_ADDR_REGISTRY_RESERVED	6

/* text representations */
typedef struct {
	unsigned int number;
	char *token;
} s_type;

/*@unused@*/ static const s_type ipv6calc_ipv6addrtypestrings[] = {
	{ IPV6_ADDR_ANY			, "unknown" },
	{ IPV6_ADDR_UNICAST		, "unicast" },
	{ IPV6_ADDR_MULTICAST		, "multicast" },
	{ IPV6_ADDR_ANYCAST		, "anycast" },
	{ IPV6_ADDR_LOOPBACK		, "loopback" },
	{ IPV6_ADDR_LINKLOCAL		, "link-local" },
	{ IPV6_ADDR_SITELOCAL		, "site-local" },
	{ IPV6_ADDR_COMPATv4		, "compat-v4" },
	{ IPV6_ADDR_MAPPED		, "mapped" },
	{ IPV6_ADDR_RESERVED		, "reserved" },
	{ IPV6_NEW_ADDR_6TO4		, "6to4" },
	{ IPV6_NEW_ADDR_6BONE		, "6bone" },
	{ IPV6_NEW_ADDR_AGU		, "aggregatable-global-unicast" },
	{ IPV6_NEW_ADDR_UNSPECIFIED	, "unspecified" },
	{ IPV6_NEW_ADDR_SOLICITED_NODE	, "solicited-node" },
	{ IPV6_NEW_ADDR_ISATAP		, "ISATAP" },
	{ IPV6_NEW_ADDR_PRODUCTIVE	, "productive" }
};

typedef struct {
	int number;
	char *token;
	char *tokensimple;
} s_type2;

/*@unused@*/ static const s_type2 ipv6calc_ipv6addrregistry[] = {
	{ IPV6_ADDR_REGISTRY_6BONE	, "6BONE"	, "6bone" 	},
	{ IPV6_ADDR_REGISTRY_IANA	, "IANA"	, "iana" 	},
	{ IPV6_ADDR_REGISTRY_APNIC	, "APNIC"	, "apnic" 	},
	{ IPV6_ADDR_REGISTRY_ARIN	, "ARIN" 	, "arin"	},
	{ IPV6_ADDR_REGISTRY_RIPE	, "RIPE NCC"	, "ripe"	},
	{ IPV6_ADDR_REGISTRY_RESERVED	, "reserved"	, "reserved"	}
};
#endif


/* prototypes */
extern uint8_t  ipv6addr_getoctett(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numoctett);
extern uint16_t ipv6addr_getword(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numword);
extern uint32_t ipv6addr_getdword(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numdword);

extern void ipv6addr_setoctett(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numocett, unsigned int value);
extern void ipv6addr_setword(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numword, unsigned int value);
extern void ipv6addr_setdword(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numdword, unsigned int value);

extern void ipv6addr_clear(ipv6calc_ipv6addr *ipv6addrp);
extern void ipv6addr_clearall(ipv6calc_ipv6addr *ipv6addrp);
extern void ipv6addr_copy(ipv6calc_ipv6addr *ipv6addrp_dst, const ipv6calc_ipv6addr *ipv6addrp_src);

extern unsigned int ipv6addr_gettype(const ipv6calc_ipv6addr *ipv6addrp);
extern int ipv6addr_getregistry(const ipv6calc_ipv6addr *ipv6addrp);

extern int  addr_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);


extern int  libipv6addr_ipv6addrstruct_to_uncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const int formatoptions); /* going obsolete */
extern int  ipv6addrstruct_to_uncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring); 

extern int  ipv6addrstruct_to_uncompaddrprefix(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring);
extern int  ipv6addrstruct_to_uncompaddrsuffix(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring);

extern int  libipv6addr_ipv6addrstruct_to_fulluncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const int formatoptions);
extern int  ipv6addrstruct_to_fulluncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring); /* going obsolete */

extern void ipv6addrstruct_maskprefix(ipv6calc_ipv6addr *ipv6addrp);
extern void ipv6addrstruct_masksuffix(ipv6calc_ipv6addr *ipv6addrp);

extern int  identifier_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);

extern int  tokenlsb64_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);
extern int  libipv6addr_ipv6addrstruct_to_tokenlsb64(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const int formatoptions);
