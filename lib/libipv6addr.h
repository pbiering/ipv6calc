/*
 * Project    : ipv6calc
 * File       : libipv6addr.h
 * Version    : $Id: libipv6addr.h,v 1.6 2002/04/05 21:31:51 peter Exp $
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
	uint8_t prefixlength;		/* prefix length (0-128) 8 bit*/
	int flag_prefixuse;		/* =1 prefix length in use */
	uint32_t scope;			/* address typeinfo/scope */
	uint8_t bit_start;		/* start of bit */
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

#define IPV6_ADDR_ANY				(uint32_t) 0x0000U

#define IPV6_ADDR_UNICAST			(uint32_t) 0x0001U	
#define IPV6_ADDR_MULTICAST			(uint32_t) 0x0002U	
#define IPV6_ADDR_ANYCAST			(uint32_t) 0x0004U

#define IPV6_ADDR_LOOPBACK			(uint32_t) 0x0010U
#define IPV6_ADDR_LINKLOCAL			(uint32_t) 0x0020U
#define IPV6_ADDR_SITELOCAL			(uint32_t) 0x0040U

#define IPV6_ADDR_COMPATv4			(uint32_t) 0x0080U

#define IPV6_ADDR_SCOPE_MASK			(uint32_t) 0x00f0U

#define IPV6_ADDR_MAPPED			(uint32_t) 0x1000U
#define IPV6_ADDR_RESERVED			(uint32_t) 0x2000U	/* reserved address space */

#define IPV6_NEW_ADDR_6TO4			(uint32_t) 0x00010000U
#define IPV6_NEW_ADDR_6BONE			(uint32_t) 0x00020000U
#define IPV6_NEW_ADDR_AGU			(uint32_t) 0x00040000U
#define IPV6_NEW_ADDR_UNSPECIFIED		(uint32_t) 0x00080000U
#define IPV6_NEW_ADDR_SOLICITED_NODE		(uint32_t) 0x00100000U
#define IPV6_NEW_ADDR_ISATAP			(uint32_t) 0x00200000U
#define IPV6_NEW_ADDR_PRODUCTIVE		(uint32_t) 0x00400000U

/* Registries */
#define IPV6_ADDR_REGISTRY_6BONE	1
#define IPV6_ADDR_REGISTRY_IANA		2
#define IPV6_ADDR_REGISTRY_APNIC	3
#define IPV6_ADDR_REGISTRY_ARIN		4
#define IPV6_ADDR_REGISTRY_RIPE		5
#define IPV6_ADDR_REGISTRY_RESERVED	6

/* text representations */
typedef struct {
	const uint32_t number;
	const char *token;
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
	const int number;
	const char *token;
	const char *tokensimple;
} s_type2;

/*@unused@*/ static const s_type2 ipv6calc_ipv6addrregistry[] = {
	{ IPV6_ADDR_REGISTRY_6BONE	, "6BONE"	, "6BONE" 	},
	{ IPV6_ADDR_REGISTRY_IANA	, "IANA"	, "IANA" 	},
	{ IPV6_ADDR_REGISTRY_APNIC	, "APNIC"	, "APNIC" 	},
	{ IPV6_ADDR_REGISTRY_ARIN	, "ARIN" 	, "ARIN"	},
	{ IPV6_ADDR_REGISTRY_RIPE	, "RIPE NCC"	, "RIPENCC"	},
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

extern uint32_t ipv6addr_gettype(const ipv6calc_ipv6addr *ipv6addrp);
extern int ipv6addr_getregistry(const ipv6calc_ipv6addr *ipv6addrp);
extern int libipv6addr_get_registry_string(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring);

extern int  addr_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);


extern int  libipv6addr_ipv6addrstruct_to_uncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions); /* going obsolete */
extern int  ipv6addrstruct_to_uncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring); 

extern int  ipv6addrstruct_to_uncompaddrprefix(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring);
extern int  ipv6addrstruct_to_uncompaddrsuffix(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring);

extern int  libipv6addr_ipv6addrstruct_to_fulluncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions);
extern int  ipv6addrstruct_to_fulluncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring); /* going obsolete */

extern void ipv6addrstruct_maskprefix(ipv6calc_ipv6addr *ipv6addrp);
extern void ipv6addrstruct_masksuffix(ipv6calc_ipv6addr *ipv6addrp);

extern int  identifier_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);

extern int  tokenlsb64_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);
extern int  libipv6addr_ipv6addrstruct_to_tokenlsb64(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions);
