/*
 * Project    : ipv6calc/lib
 * File       : libipv6addr.h
 * Version    : $Id: libipv6addr.h,v 1.62 2013/04/07 17:52:29 ds6peter Exp $
 * Copyright  : 2001-2013 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
 *
 * Information:
 *  Header file for libipv6addr.c
 */ 

#include "ipv6calctypes.h"
#include <netinet/in.h>
#include <ctype.h>

/* typedefs & defines */

#ifndef _libipv6addr_h

#define _libipv6addr_h 1

/* IPv6 anonymization */
/* 
 * IID anonymization is done by replacing with related information (64-bit)
 * xxxx:xxxx:xxxx:xxxx
 *
 * a9p9 4941 0000 000C  -> RFC 4941 anonymized privacy extension Interface ID
 *
 * a9p9 4291 4xxx xxxC  -> RFC 4291 anonymized EUI-48 Interface ID, xxx xxx = converted OUI
 * a9p9 4291 6xxx xxxC  -> RFC 4291 anonymized EUI-64 Interface ID, xxx xxx = converted OUI
 * a9p9 4291 8xxx xxxC  -> RFC 4291 embedded anonymized IPv4 address, xxx xxx = first 24 bit of included (anonymized) IPv4 address
 * a9p9 4291 c02d 5d1C  -> RFC 4291 Chapter 2.5.1 anonymized static Interface ID
 *
 * a9p9 5214 4xxx xxxC  -> RFC 5214 anonymized ISATAP Interface ID, xxx xxx = first 24 bit of included (anonymized) IPv4 address (local scope)
 * a9p9 5214 5xxx xxxC  -> RFC 5214 anonymized ISATAP Interface ID, xxx xxx = first 24 bit of included (anonymized) IPv4 address (global scope)
 * a9p9 5214 cxx0 000C  -> RFC 5214 anonymized ISATAP Interface ID, xx      = first  8 bit of included vendor ID (local scope)
 * a9p9 5214 dxx0 000C  -> RFC 5214 anonymized ISATAP Interface ID, xx      = first  8 bit of included vendor ID (global scope)
 * a9p9 5214 exxx xxxC  -> RFC 5214 anonymized ISATAP Interface ID, xxx xxx = first 24 bit of included extension ID (local scope)
 * a9p9 5214 fxxx xxxC  -> RFC 5214 anonymized ISATAP Interface ID, xxx xxx = first 24 bit of included extension ID (global scope)
 *
 * a9p9 4843 0000 000C  -> RFC 4843 anonymized ORCHID hash
 *
 * SLA/NLA prefix part anonymization is done by replacing with pattern a909a909
 *   p = number of nibbles anonymized
 */
#define ANON_TOKEN_VALUE_00_31		(uint32_t) 0xa9090000u
#define ANON_TOKEN_MASK_00_31		(uint32_t) 0xff0f0000u

#define ANON_CHECKSUM_MASK_32_63	(uint32_t) 0x0000000fu

#define ANON_IID_PRIVACY_VALUE_00_31	(uint32_t) 0x00004941u
#define ANON_IID_PRIVACY_MASK_00_31	(uint32_t) 0x0000ffffu
#define ANON_IID_PRIVACY_VALUE_32_63	(uint32_t) 0x00000000u
#define ANON_IID_PRIVACY_MASK_32_63	(uint32_t) 0xfffffff0u

#define ANON_IID_STATIC_VALUE_00_31	(uint32_t) 0x00004291u
#define ANON_IID_STATIC_MASK_00_31	(uint32_t) 0x0000ffffu
#define ANON_IID_STATIC_VALUE_32_63	(uint32_t) 0xc02d5d10u
#define ANON_IID_STATIC_MASK_32_63	(uint32_t) 0xfffffff0u

#define ANON_ORCHID_VALUE_00_31		(uint32_t) 0x00004843u
#define ANON_ORCHID_STATIC_MASK_00_31	(uint32_t) 0x0000ffffu
#define ANON_ORCHID_VALUE_32_63		(uint32_t) 0x00000000u
#define ANON_ORCHID_STATIC_MASK_32_63	(uint32_t) 0xfffffff0u

#define ANON_IID_EUI48_VALUE_00_31	(uint32_t) 0x00004291u
#define ANON_IID_EUI48_MASK_00_31	(uint32_t) 0x0000ffffu
#define ANON_IID_EUI48_VALUE_32_63	(uint32_t) 0x40000000u
#define ANON_IID_EUI48_MASK_32_63	(uint32_t) 0xf0000000u
#define ANON_IID_EUI48_PAYLOAD_32_63	(uint32_t) 0x0ffffff0u
#define ANON_IID_EUI48_PAYLOAD_SHIFT	4
#define ANON_IID_EUI48_PAYLOAD_LENGTH	24	

#define ANON_IID_EUI64_VALUE_00_31	(uint32_t) 0x00004291u
#define ANON_IID_EUI64_MASK_00_31	(uint32_t) 0x0000ffffu
#define ANON_IID_EUI64_VALUE_32_63	(uint32_t) 0x60000000u
#define ANON_IID_EUI64_MASK_32_63	(uint32_t) 0xf0000000u
#define ANON_IID_EUI64_PAYLOAD_32_63	(uint32_t) 0x0ffffff0u
#define ANON_IID_EUI64_PAYLOAD_SHIFT	4
#define ANON_IID_EUI64_PAYLOAD_LENGTH	24	

#define ANON_IID_IPV4_VALUE_00_31	(uint32_t) 0x00004291u
#define ANON_IID_IPV4_MASK_00_31	(uint32_t) 0x0000ffffu
#define ANON_IID_IPV4_VALUE_32_63	(uint32_t) 0x80000000u
#define ANON_IID_IPV4_MASK_32_63	(uint32_t) 0xf0000000u
#define ANON_IID_IPV4_PAYLOAD_32_63	(uint32_t) 0x0ffffff0u
#define ANON_IID_IPV4_PAYLOAD_SHIFT	4
#define ANON_IID_IPV4_PAYLOAD_LENGTH	24	

#define ANON_IID_EUIxx_SCOPE_MASK	(uint32_t) 0x00200000u
#define ANON_IID_EUIxx_SCOPE_GLOBAL	(uint32_t) 0x00200000u
#define ANON_IID_EUIxx_SCOPE_LOCAL	(uint32_t) 0x00000000u

#define ANON_IID_ISATAP_VALUE_00_31			(uint32_t) 0x00005214u
#define ANON_IID_ISATAP_MASK_00_31			(uint32_t) 0x0000ffffu
#define ANON_IID_ISATAP_PAYLOAD_SHIFT			4
#define ANON_IID_ISATAP_PAYLOAD_LENGTH			24

#define ANON_IID_ISATAP_SCOPE_MASK			(uint32_t) 0x10000000u
#define ANON_IID_ISATAP_SCOPE_LOCAL			(uint32_t) 0x00000000u
#define ANON_IID_ISATAP_SCOPE_GLOBAL			(uint32_t) 0x10000000u

#define ANON_IID_ISATAP_TYPE_MASK_32_63			(uint32_t) 0xe0000000u
#define ANON_IID_ISATAP_TYPE_IPV4_VALUE_32_63		(uint32_t) 0x40000000u
#define ANON_IID_ISATAP_TYPE_IPV4_PAYLOAD_32_63		(uint32_t) 0x0ffffff0u
#define ANON_IID_ISATAP_TYPE_VENDOR_VALUE_32_63		(uint32_t) 0xc0000000u
#define ANON_IID_ISATAP_TYPE_VENDOR_PAYLOAD_32_63	(uint32_t) 0x0ff00000u
#define ANON_IID_ISATAP_TYPE_EXTID_VALUE_32_63		(uint32_t) 0xe0000000u
#define ANON_IID_ISATAP_TYPE_EXTID_PAYLOAD_32_63	(uint32_t) 0x0ffffff0u

#define ANON_CHECKSUM_FLAG_CREATE	1
#define ANON_CHECKSUM_FLAG_VERIFY	2


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
	float lls_residual;		// lls: linear least square
	int   digit_blocks[16];		// block length of same digit
	int   digit_amount[16]; 	// amount of digits
	int   digit_delta[31];  	// delta of digits
	int   digit_delta_amount;  	// amount of delta of digits
} s_iid_statistics;

/* IID privacy limits */
// this filter values detects 999.744 from 1.000.000 generated privacy extension IIDs (256 are not detected)
static const s_iid_statistics s_iid_statistics_ok_min = {
	0.249,		// fits to 100% of 1 million tested
	6.275,		// fits to 100% of 1 million tested
	{  4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	// fit to 100% of 1 million tested
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	// default
	{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // default
	6	// fit to 1 million
};

static const s_iid_statistics s_iid_statistics_ok_max = {
	2.5,		// fits to 1 million - 90 tested (100%: 4.016)
	26.042,		// fits to 100% of 1 million tested
	{ 16, 6, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	// fit to 1 million - 3 tested
	{  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},	// fit to 1 million - 38 tested
	{  3, 4, 3, 3, 4, 5, 4, 5, 5, 5, 6, 6, 6, 7, 7, 6, 6, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 4, 4, 3, 2},	// fit to 1 million
	15	// fit to 1 million
};


/* filter */
typedef struct {
	int active;
	uint32_t typeinfo_must_have;
	uint32_t typeinfo_may_not_have;

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
#define IPV6_NEW_ADDR_IID_ISATAP		(uint32_t) 0x00000200U	/* RFC 5214 (ex 4214) */
#define IPV6_NEW_ADDR_IID_EUI48			(uint32_t) 0x00000400U
#define IPV6_NEW_ADDR_IID_EUI64			(uint32_t) 0x00000800U

#define IPV6_ADDR_MAPPED			(uint32_t) 0x00001000U
#define IPV6_ADDR_RESERVED			(uint32_t) 0x00002000U	/* reserved address space */
#define IPV6_ADDR_ULUA				(uint32_t) 0x00004000U	/* Unique Local Unicast Address */
#define IPV6_ADDR_ANONYMIZED			(uint32_t) 0x00008000U	/* anonymized IPv6 address */

#define IPV6_NEW_ADDR_6TO4			(uint32_t) 0x00010000U
#define IPV6_NEW_ADDR_6BONE			(uint32_t) 0x00020000U
#define IPV6_NEW_ADDR_AGU			(uint32_t) 0x00040000U
#define IPV6_NEW_ADDR_UNSPECIFIED		(uint32_t) 0x00080000U

#define IPV6_NEW_ADDR_SOLICITED_NODE		(uint32_t) 0x00100000U
#define IPV6_ADDR_IID_32_63_HAS_IPV4		(uint32_t) 0x00200000U
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
	{ IPV6_ADDR_ANONYMIZED		, "anonymized" },
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
	{ IPV6_NEW_ADDR_IID_ISATAP	, "iid-isatap" },
	{ IPV6_ADDR_IID_32_63_HAS_IPV4	, "iid-includes-ipv4" }
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

extern void libipv6addr_anonymize(ipv6calc_ipv6addr *ipv6addrp, const s_ipv6calc_anon_set *ipv6calc_anon_set);
extern uint32_t ipv6addr_get_payload_anonymized_iid(const ipv6calc_ipv6addr *ipv6addrp, const uint32_t typeinfo);

extern int ipv6addr_privacyextensiondetection(const ipv6calc_ipv6addr *ipv6addrp, s_iid_statistics *variancesp);

extern int ipv6addr_filter(const ipv6calc_ipv6addr *ipv6addrp, const s_ipv6calc_filter_ipv6addr *filter);
extern int ipv6addr_filter_parse(s_ipv6calc_filter_ipv6addr *filter, const char *token);
extern void ipv6addr_filter_clear(s_ipv6calc_filter_ipv6addr *filter);
