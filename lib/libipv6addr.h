/*
 * Project    : ipv6calc/lib
 * File       : libipv6addr.h
 * Version    : $Id: libipv6addr.h,v 1.44 2012/03/27 19:15:14 peter Exp $
 * Copyright  : 2001-2012 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
 *
 * Information:
 *  Header file for libipv6addr.c
 */ 

#include "ipv6calctypes.h"
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
	char scopeid[NI_MAXHOST];	/* scope ID value */
	int flag_scopeid;		/* =1: scope ID value set */
} ipv6calc_ipv6addr;

/* IPv6 address assignment structure */
typedef struct {
	const uint32_t ipv6addr_00_31;
	const uint32_t ipv6addr_32_63;
	const uint32_t ipv6mask_00_31;
	const uint32_t ipv6mask_32_63;
	const uint8_t  prefixlength;	/* prefix length (0-128) 8 bit*/
	const char *string_registry;
} s_ipv6addr_assignment;

/* IID statistics */
typedef struct {
	float hexdigit;
	float bits_simple[4];
	float bits_permuted[4];
	float average;
} s_iid_statistics;

/* spread values to align peaks */
static const s_iid_statistics s_iid_statistics_spread = {
	1,
	{ 0, 0, 0, 0 }, // bits_simple blocksize (4), 8, 16, 32
	{ 0, 0, 0, 0 }, // bits_permuted (4), 8, 16, 32
	1
};

/*
static const s_iid_statistics s_iid_statistics_spread = {
	2.89,
	{ 0, 3.045, 1.2, 0.44, }, // bits_simple blocksize (4), 8, 16, 32
	{ 0, 1.147, 0.698, 0.44 }, // bits_permuted (4), 8, 16, 32
	1
};
*/

/* shift values to align peaks: >0: shift right  <0: shift left */
static const s_iid_statistics s_iid_statistics_shift = {
	0,
	{ 0, 0, 0, 0 }, // bits_simple blocksize 4, 8, 16, 32
	{ 0, 0, 0, 0 }, // bits_permuted (4), 8, 16, 32
	0
};

/*
static const s_iid_statistics s_iid_statistics_shift = {
	0.25,
	{ 0, -1.5, 0.375, 1.5 }, // bits_simple blocksize 4, 8, 16, 32
	{ 0, -1.375, 0.375, 1.5 }, // bits_permuted (4), 8, 16, 32
	0
};
*/

#define IPV6_IID_PRIVACY_AVG_LIMIT		5.5	// 99% probability
#define IPV6_IID_PRIVACY_AVG_SIMPLE_START	1
#define IPV6_IID_PRIVACY_AVG_PERMUTED_START	1

/* filter */
typedef struct {
	int active;
	uint32_t typeinfo_must_have;
	uint32_t typeinfo_may_not_have;

	/* iid variance */
	float	iid_var_min;
	int	iid_var_min_active;
	float	iid_var_max;
	int	iid_var_max_active;

	/* others coming next */
} s_ipv6calc_filter_ipv6addr;


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

#define IPV6_ADDR_ANY				(uint32_t) 0x00000000U

#define IPV6_ADDR_UNICAST			(uint32_t) 0x00000001U	
#define IPV6_ADDR_MULTICAST			(uint32_t) 0x00000002U	
#define IPV6_ADDR_ANYCAST			(uint32_t) 0x00000004U

#define IPV6_ADDR_LOOPBACK			(uint32_t) 0x00000010U
#define IPV6_ADDR_LINKLOCAL			(uint32_t) 0x00000020U
#define IPV6_ADDR_SITELOCAL			(uint32_t) 0x00000040U

#define IPV6_ADDR_COMPATv4			(uint32_t) 0x00000080U

#define IPV6_ADDR_SCOPE_MASK			(uint32_t) 0x000000f0U

#define IPV6_NEW_ADDR_IID_TEREDO		(uint32_t) 0x00000100U	/* RFC xxxx */
#define IPV6_NEW_ADDR_IID_ISATAP		(uint32_t) 0x00000200U	/* RFC 4214 */
#define IPV6_NEW_ADDR_IID_EUI48			(uint32_t) 0x00000400U
#define IPV6_NEW_ADDR_IID_EUI64			(uint32_t) 0x00000800U

#define IPV6_ADDR_MAPPED			(uint32_t) 0x00001000U
#define IPV6_ADDR_RESERVED			(uint32_t) 0x00002000U	/* reserved address space */
#define IPV6_ADDR_ULUA				(uint32_t) 0x00004000U	/* Unique Local Unicast Address */

#define IPV6_NEW_ADDR_6TO4			(uint32_t) 0x00010000U
#define IPV6_NEW_ADDR_6BONE			(uint32_t) 0x00020000U
#define IPV6_NEW_ADDR_AGU			(uint32_t) 0x00040000U
#define IPV6_NEW_ADDR_UNSPECIFIED		(uint32_t) 0x00080000U
#define IPV6_NEW_ADDR_SOLICITED_NODE		(uint32_t) 0x00100000U

#define IPV6_NEW_ADDR_PRODUCTIVE		(uint32_t) 0x00400000U
#define IPV6_NEW_ADDR_6TO4_MICROSOFT		(uint32_t) 0x00800000U
#define IPV6_NEW_ADDR_TEREDO			(uint32_t) 0x01000000U
#define IPV6_NEW_ADDR_ORCHID			(uint32_t) 0x02000000U  /* RFC 4843 */
#define IPV6_NEW_ADDR_LINKLOCAL_TEREDO		(uint32_t) 0x04000000U
#define IPV6_NEW_ADDR_NAT64			(uint32_t) 0x08000000U	/* RFC 6052 */

#define IPV6_NEW_ADDR_IID_PRIVACY		(uint32_t) 0x10000000U	/* RFC 4941 (ex 3041) */
#define IPV6_NEW_ADDR_IID			(uint32_t) 0x20000000U	/* IPv6 address with IID inside */
#define IPV6_NEW_ADDR_IID_LOCAL			(uint32_t) 0x40000000U	/* IPv6 address with local generated IID */
#define IPV6_NEW_ADDR_IID_GLOBAL		(uint32_t) 0x80000000U	/* IPv6 address with global IID */

/* text representations */
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
	{ IPV6_ADDR_ULUA		, "unique-local-unicast" },
	{ IPV6_NEW_ADDR_6TO4		, "6to4" },
	{ IPV6_NEW_ADDR_6BONE		, "6bone" },
	{ IPV6_NEW_ADDR_AGU		, "global-unicast" },
	{ IPV6_NEW_ADDR_UNSPECIFIED	, "unspecified" },
	{ IPV6_NEW_ADDR_SOLICITED_NODE	, "solicited-node" },
	{ IPV6_NEW_ADDR_PRODUCTIVE	, "productive" },
	{ IPV6_NEW_ADDR_6TO4_MICROSOFT	, "6to4-microsoft" },
	{ IPV6_NEW_ADDR_TEREDO		, "teredo" },
	{ IPV6_NEW_ADDR_ORCHID		, "orchid" },
	{ IPV6_NEW_ADDR_LINKLOCAL_TEREDO, "link-local-teredo" },
	{ IPV6_NEW_ADDR_NAT64		, "nat64" },
	{ IPV6_NEW_ADDR_IID_PRIVACY	, "iid-privacy" },
	{ IPV6_NEW_ADDR_IID		, "iid" },
	{ IPV6_NEW_ADDR_IID_LOCAL	, "iid-local" },
	{ IPV6_NEW_ADDR_IID_GLOBAL	, "iid-global" },
	{ IPV6_NEW_ADDR_IID_TEREDO	, "iid-teredo" },
	{ IPV6_NEW_ADDR_IID_EUI48	, "iid-eui48" },
	{ IPV6_NEW_ADDR_IID_EUI64	, "iid-eui64" },
	{ IPV6_NEW_ADDR_IID_ISATAP	, "iid-isatap" }
};


/* Registries */
#define IPV6_ADDR_REGISTRY_6BONE	0x01
#define IPV6_ADDR_REGISTRY_IANA		0x02
#define IPV6_ADDR_REGISTRY_APNIC	0x03
#define IPV6_ADDR_REGISTRY_ARIN		0x04
#define IPV6_ADDR_REGISTRY_RIPE		0x05
#define IPV6_ADDR_REGISTRY_LACNIC	0x06
#define IPV6_ADDR_REGISTRY_AFRINIC	0x07
#define IPV6_ADDR_REGISTRY_RESERVED	0x0e
#define IPV6_ADDR_REGISTRY_UNKNOWN	0x0f

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
	{ IPV6_ADDR_REGISTRY_LACNIC	, "LACNIC"	, "LACNIC"	},
	{ IPV6_ADDR_REGISTRY_AFRINIC	, "AFRINIC"	, "AFRINIC"	},
	{ IPV6_ADDR_REGISTRY_RESERVED	, "reserved"	, "reserved"	},
	{ IPV6_ADDR_REGISTRY_UNKNOWN	, "unknown"	, "unknown"	}
};
#endif


/* prototypes */
extern uint8_t  ipv6addr_getoctet(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numoctet);
extern uint16_t ipv6addr_getword(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numword);
extern uint32_t ipv6addr_getdword(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numdword);

extern void ipv6addr_setoctet(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numocett, unsigned int value);
extern void ipv6addr_setword(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numword, unsigned int value);
extern void ipv6addr_setdword(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numdword, unsigned int value);

extern void ipv6addr_clear(ipv6calc_ipv6addr *ipv6addrp);
extern void ipv6addr_clearall(ipv6calc_ipv6addr *ipv6addrp);
extern void ipv6addr_copy(ipv6calc_ipv6addr *ipv6addrp_dst, const ipv6calc_ipv6addr *ipv6addrp_src);

extern uint32_t ipv6addr_gettype(const ipv6calc_ipv6addr *ipv6addrp);
extern int ipv6addr_getregistry(const ipv6calc_ipv6addr *ipv6addrp);
extern int libipv6addr_get_registry_string(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring);

extern int  addr_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);
extern int  addrliteral_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);

extern int  libipv6addr_ipv6addrstruct_to_uncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions);
extern int  libipv6addr_to_octal(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions);
extern int  libipv6addr_to_hex(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions);

extern void ipv6addrstruct_maskprefix(ipv6calc_ipv6addr *ipv6addrp);
extern void ipv6addrstruct_masksuffix(ipv6calc_ipv6addr *ipv6addrp);

extern int  identifier_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);

extern int  tokenlsb64_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);
extern int  libipv6addr_ipv6addrstruct_to_tokenlsb64(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions);

extern void libipv6addr_anonymize(ipv6calc_ipv6addr *ipv6addrp, unsigned int mask_iid, unsigned int mask_ipv6, unsigned int mask_ipv4);

extern int ipv6addr_privacyextensiondetection(const ipv6calc_ipv6addr *ipv6addrp, s_iid_statistics *variancesp);

extern int ipv6addr_filter(const ipv6calc_ipv6addr *ipv6addrp, const s_ipv6calc_filter_ipv6addr *filter);
extern int ipv6addr_filter_parse(s_ipv6calc_filter_ipv6addr *filter, const char *token);
extern void ipv6addr_filter_clear(s_ipv6calc_filter_ipv6addr *filter);
