/*
 * Project    : ipv6calc
 * File       : libipv6addr.h
 * Version    : $Id$
 * Copyright  : 2001-2021 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
 * License    : GNU GPL v2
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
 * xxxx:xxxx:xxxx:xxxC  (C = 4-bit checksum)
 *
 * a9p9 4941 0000 000C  -> RFC 4941 anonymized privacy extension Interface ID
 *
 * a9p9 4291 4xxx xxxC  -> RFC 4291 anonymized EUI-48 Interface ID, xxx xxx = converted OUI
 * a9p9 4291 5xxx xxxC  -> RFC 4291 anonymized EUI-48 Interface ID, xxx xxx = converted mapped IAB/OUI-36/OUI-28
 * a9p9 4291 6xxx xxxC  -> RFC 4291 anonymized EUI-64 Interface ID, xxx xxx = converted OUI
 * a9p9 4291 7xxx xxxC  -> RFC 4291 anonymized EUI-64 Interface ID, xxx xxx = converted mapped IAB/OUI-36/OUI-28
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
 *   p = number of nibbles anonymized in prefix
 *   		0 : no nibble of prefix is anonymized
 *   		f : Prefix anonymization with method=kp|kg
 *
 * Prefix anonymization, but skipped for
 *  - LISP anycast
 * in case of method=kp: p=0x0f
 * a909:ccca:aaaa:aaaC  (C = 4-bit checksum)
 *                      ccc      -> 10-bit Country Code mapping [A-Z]*[A-Z0-9] (936)
 *                                   0x3FD = LISP
 *                                   0x3FE = unknown country
 *                                   0x3FF - 16 + REGISTRY_6BONE && ASN=0 = 6bone
 *                                   0x000-0x3A7: c1= c / 36, c2 = c % 36
 *                               -> 2 leftmost bit are |0 0|
 *                      aaaaaaaa -> 32-bit ASN
 *
 * in case of method=kg: p=0x0f
 * a909:sssg:gggg:gggC  (C = 4-bit checksum)
 *                      sss      -> 10-bit source mapping
 *                                   GeonameID source information
 *                                   0x000 = unknown
 *			             0x001 = continent  (prio: lowest)
 *                                   0x002 = country
 *                                   0x003 = state/prov
 *                                   0x004 = district
 *                                   0x005 = city       (prio: highest)
 *                                   0x006 = (reserved)
 *                                   0x007 = LISP
 *                                   0x0r. = registry
 *                               -> 2 leftmost bit are |0 1|
 *                      gggggggg -> 32-bit GeonameID
 *                                   0x11800 = LISP
 */
#define ANON_TOKEN_VALUE_00_31		(uint32_t) 0xa9090000u
#define ANON_TOKEN_MASK_00_31		(uint32_t) 0xff0f0000u

#define ANON_IID_PREFIX_NIBBLES_MASK	(uint32_t) 0x0000000fu
#define ANON_IID_PREFIX_NIBBLES_SHIFT	20

#define ANON_CHECKSUM_MASK_32_63	(uint32_t) 0x0000000fu

#define ANON_IID_RANDOM_VALUE_00_31	(uint32_t) 0x00004941u
#define ANON_IID_RANDOM_MASK_00_31	(uint32_t) 0x0000ffffu
#define ANON_IID_RANDOM_VALUE_32_63	(uint32_t) 0x00000000u
#define ANON_IID_RANDOM_MASK_32_63	(uint32_t) 0xfffffff0u

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
#define ANON_IID_EUI48_MASK_32_63	(uint32_t) 0xe0000000u
#define ANON_IID_EUI48_PAYLOAD_32_63	(uint32_t) 0x1ffffff0u
#define ANON_IID_EUI48_PAYLOAD_SHIFT	4
#define ANON_IID_EUI48_PAYLOAD_LENGTH	24	

#define ANON_IID_EUI64_VALUE_00_31	(uint32_t) 0x00004291u
#define ANON_IID_EUI64_MASK_00_31	(uint32_t) 0x0000ffffu
#define ANON_IID_EUI64_VALUE_32_63	(uint32_t) 0x60000000u
#define ANON_IID_EUI64_MASK_32_63	(uint32_t) 0xe0000000u
#define ANON_IID_EUI64_PAYLOAD_32_63	(uint32_t) 0x1ffffff0u
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

// Prefix anonymization on method=kp
#define ANON_METHOD_KEEPTYPEASNCC_IPV6_REQ_DB	(IPV6CALC_DB_IPV6_TO_CC | IPV6CALC_DB_IPV6_TO_AS)
// Prefix anonymization on method=kg
#define ANON_METHOD_KEEPTYPEGEONAMEID_IPV6_REQ_DB	(IPV6CALC_DB_IPV6_TO_GEONAMEID)

#define ANON_PREFIX_CCINDEX_DWORD	0
#define ANON_PREFIX_CCINDEX_SHIFT	4
#define ANON_PREFIX_CCINDEX_MASK	0x3ff
#define ANON_PREFIX_CCINDEX_XOR		0x0

#define ANON_PREFIX_GEONAMEID_TYPE_DWORD	0
#define ANON_PREFIX_GEONAMEID_TYPE_SHIFT	4
#define ANON_PREFIX_GEONAMEID_TYPE_MASK		0x3ff
#define ANON_PREFIX_GEONAMEID_TYPE_XOR		0x0

#define ANON_PREFIX_ASN32_MSB_DWORD	0
#define ANON_PREFIX_ASN32_MSB_SHIFT	0
#define ANON_PREFIX_ASN32_MSB_AMOUNT	4
#define ANON_PREFIX_ASN32_MSB_MASK	((1 << ANON_PREFIX_ASN32_MSB_AMOUNT) - 1)
#define ANON_PREFIX_ASN32_MSB_XOR	0x0000000a

#define ANON_PREFIX_ASN32_LSB_DWORD	1
#define ANON_PREFIX_ASN32_LSB_SHIFT	4
#define ANON_PREFIX_ASN32_LSB_AMOUNT	28
#define ANON_PREFIX_ASN32_LSB_MASK	((1 << ANON_PREFIX_ASN32_LSB_AMOUNT) - 1)
#define ANON_PREFIX_ASN32_LSB_XOR	0x09090000

#define ANON_PREFIX_GEONAMEID_MSB_DWORD	0
#define ANON_PREFIX_GEONAMEID_MSB_SHIFT	0
#define ANON_PREFIX_GEONAMEID_MSB_AMOUNT	4
#define ANON_PREFIX_GEONAMEID_MSB_MASK	((1 << ANON_PREFIX_GEONAMEID_MSB_AMOUNT) - 1)
#define ANON_PREFIX_GEONAMEID_MSB_XOR	0x0000000a

#define ANON_PREFIX_GEONAMEID_LSB_DWORD	1
#define ANON_PREFIX_GEONAMEID_LSB_SHIFT	4
#define ANON_PREFIX_GEONAMEID_LSB_AMOUNT	28
#define ANON_PREFIX_GEONAMEID_LSB_MASK	((1 << ANON_PREFIX_GEONAMEID_LSB_AMOUNT) - 1)
#define ANON_PREFIX_GEONAMEID_LSB_XOR	0x09090000

#define ANON_PREFIX_FLAGS_DWORD		0
#define ANON_PREFIX_FLAGS_SHIFT		14
#define ANON_PREFIX_FLAGS_MASK		0x3
#define ANON_PREFIX_FLAGS_XOR		0x0

#define ANON_PREFIX_TOKEN_DWORD		0
#define ANON_PREFIX_TOKEN_SHIFT		16
#define ANON_PREFIX_TOKEN_MASK		0xffff
#define ANON_PREFIX_TOKEN_XOR		0x0
#define ANON_PREFIX_TOKEN_VALUE		0xa909		// fix

// Payload selector
#define ANON_PREFIX_PAYLOAD_FLAGS		0
#define ANON_PREFIX_PAYLOAD_CCINDEX		1
#define ANON_PREFIX_PAYLOAD_ASN32		2
#define ANON_PREFIX_PAYLOAD_GEONAMEID		3
#define ANON_PREFIX_PAYLOAD_GEONAMEID_TYPE	4


/* IPv6 address storage structure */
typedef struct {
	struct   in6_addr in6_addr;	/* in6_addr structure */
	uint8_t  prefixlength;		/* prefix length (0-128) 8 bit */
	int      flag_prefixuse;	/* =1: prefix length in use */
	uint32_t typeinfo;		/* address typeinfo/scope */
	uint32_t typeinfo2;		/* address typeinfo2 */
	int8_t   flag_typeinfo;		/* =1: typeinfo valid */
	uint8_t  bit_start;		/* start of bit */
	uint8_t  bit_end;		/* end of bit */
	int8_t   flag_startend_use;	/* =1: start or end of bit in use */
	int8_t   flag_valid;		/* address structure filled */
	char     scopeid[IPV6CALC_SCOPEID_STRING_MAX];	/* scope ID value */
	int8_t   flag_scopeid;		/* =1: scope ID value (above) set */
	uint8_t  prefix2length;		/* prefix 2 length (0-128) 8 bit (usage depends on typeinfo/typeinfo2)*/
	uint8_t  test_mode;		/* address test mode */
} ipv6calc_ipv6addr;

/* IPv6 Address filter structure */
typedef struct {
        int active;
        int addr_must_have_max;
        int addr_may_not_have_max;
        ipv6calc_ipv6addr ipv6addr_must_have[IPV6CALC_FILTER_IPV4ADDR];
        ipv6calc_ipv6addr ipv6addr_may_not_have[IPV6CALC_FILTER_IPV6ADDR];
} s_ipv6calc_filter_addr_ipv6;

/* IPv6 filter structure */
typedef struct {
	int active;
	s_ipv6calc_filter_typeinfo    filter_typeinfo;
	s_ipv6calc_filter_typeinfo    filter_typeinfo2;
	s_ipv6calc_filter_db_cc       filter_db_cc;        // Country Code filter
	s_ipv6calc_filter_db_asn      filter_db_asn;       // Autonomous System Number filter
	s_ipv6calc_filter_db_registry filter_db_registry;  // Registry filter
	s_ipv6calc_filter_addr_ipv6   filter_addr;         // IPv6 Address filter
	/* others may come next */
} s_ipv6calc_filter_ipv6addr;


/* IID statistics */
typedef struct {
	float hexdigit;
	float lls_residual;		// lls: linear least square
	int   digit_blocks[16];		// block length of same digit
	int   digit_blocks_hexdigit[16];// hex digits used in blocks of same digit
	int   digit_amount[16]; 	// amount of digits
	int   digit_delta[31];  	// delta of digits
	int   digit_delta_amount;  	// amount of delta of digits
} s_iid_statistics;


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
#define IPV6_ADDR_ANONYMIZED_PREFIX		(uint32_t) 0x00000008U	/* anonymized IPv6 address (prefix) */

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
#define IPV6_ADDR_ANONYMIZED_IID		(uint32_t) 0x00008000U	/* anonymized IPv6 address (IID) */

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

#define IPV6_NEW_ADDR_IID_RANDOM		(uint32_t) 0x10000000U	/* possible RFC 4941 (ex 3041) */
#define IPV6_NEW_ADDR_IID			(uint32_t) 0x20000000U	/* IPv6 address with IID inside */
#define IPV6_NEW_ADDR_IID_LOCAL			(uint32_t) 0x40000000U	/* IPv6 address with local generated IID */
#define IPV6_NEW_ADDR_IID_GLOBAL		(uint32_t) 0x80000000U	/* IPv6 address with global IID */

#define IPV6_ADDR_HAS_PUBLIC_IPV4_IN_PREFIX	(IPV6_NEW_ADDR_6TO4 | IPV6_NEW_ADDR_TEREDO)
#define IPV6_ADDR_HAS_PUBLIC_IPV4_IN_IID	(IPV6_NEW_ADDR_NAT64 | IPV6_NEW_ADDR_TEREDO | IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED | IPV6_NEW_ADDR_6TO4_MICROSOFT)
#define IPV6_ADDR_HAS_PUBLIC_IPV4		(IPV6_ADDR_HAS_PUBLIC_IPV4_IN_IID | IPV6_ADDR_HAS_PUBLIC_IPV4_IN_PREFIX)

#define IPV6_ADDR_GLOBAL			(IPV6_NEW_ADDR_PRODUCTIVE)

// typeinfo2
#define IPV6_ADDR_TYPE2_6RD			(uint32_t) 0x00000001U	/* IPv6 Rapid Deployment address (RFC5569), not autodetectable (triggered by --6rd_prefixlength) */
#define IPV6_ADDR_TYPE2_LISP			(uint32_t) 0x00010000U	/* IPv6 LISP (RFC6830) */
#define IPV6_ADDR_TYPE2_LISP_PETR		(uint32_t) 0x00020000U	/* IPv6 LISP Proxy Egress Tunnel Routers (PETRY) Anycast */
#define IPV6_ADDR_TYPE2_LISP_MAP_RESOLVER	(uint32_t) 0x00040000U	/* IPv6 LISP Map Resolver Anycast */
#define IPV6_ADDR_TYPE2_ANON_MASKED_PREFIX	(uint32_t) 0x00080000U	/* IPv6 partially masked prefix */
#define IPV6_ADDR_TYPE2_ANONYMIZED_GEONAMEID	(uint32_t) 0x00100000U  /* IPv6 prefix anonymized contains GeonameID */

// IPv4 address extractor selector
#define IPV6_ADDR_SELECT_IPV4_DEFAULT		0
#define IPV6_ADDR_SELECT_IPV4_TEREDO_SERVER	1
#define IPV6_ADDR_SELECT_IPV4_PREFIX2_LENGTH	2

/* text representations -> libipv6addr.c */

/* Registries */
#include "libipv6calc.h"

#define IPV6_ADDR_REGISTRY_6BONE	REGISTRY_6BONE
#define IPV6_ADDR_REGISTRY_IANA		REGISTRY_IANA
#define IPV6_ADDR_REGISTRY_APNIC	REGISTRY_APNIC
#define IPV6_ADDR_REGISTRY_ARIN		REGISTRY_ARIN
#define IPV6_ADDR_REGISTRY_RIPENCC	REGISTRY_RIPENCC
#define IPV6_ADDR_REGISTRY_LACNIC	REGISTRY_LACNIC
#define IPV6_ADDR_REGISTRY_AFRINIC	REGISTRY_AFRINIC
#define IPV6_ADDR_REGISTRY_RESERVED	REGISTRY_RESERVED
#define IPV6_ADDR_REGISTRY_UNKNOWN	REGISTRY_UNKNOWN

#endif


/* references */
extern const s_type ipv6calc_ipv6addrtypestrings[];
extern const s_type ipv6calc_ipv6addr_type2_strings[];

extern const int ipv6calc_ipv6addrtypestrings_entries;
extern const int ipv6calc_ipv6addr_type2_strings_entries;


/* prototypes */
extern uint8_t  ipv6addr_getoctet(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numoctet);
extern uint16_t ipv6addr_getword(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numword);
extern uint32_t ipv6addr_getdword(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numdword);

extern void ipv6addr_setoctet(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numoctet, unsigned int value);
extern void ipv6addr_setword(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numword, unsigned int value);
extern void ipv6addr_setdword(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numdword, unsigned int value);

extern void ipv6addr_clear(ipv6calc_ipv6addr *ipv6addrp);
extern void ipv6addr_clearall(ipv6calc_ipv6addr *ipv6addrp);
extern void ipv6addr_copy(ipv6calc_ipv6addr *ipv6addrp_dst, const ipv6calc_ipv6addr *ipv6addrp_src);

extern int ipv6addr_compare(const ipv6calc_ipv6addr *ipv6addrp1, const ipv6calc_ipv6addr *ipv6addrp2, const uint16_t compare_flags);

extern void ipv6addr_settype(ipv6calc_ipv6addr *ipv6addrp);

extern int  addr_to_ipv6addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv6addr *ipv6addrp);
extern int  addrliteral_to_ipv6addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv6addr *ipv6addrp);

extern int  libipv6addr_ipv6addrstruct_to_uncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions);
extern int  libipv6addr_to_octal(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions);
extern int  libipv6addr_to_hex(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions);

extern void ipv6addrstruct_maskprefix(ipv6calc_ipv6addr *ipv6addrp);
extern void ipv6addrstruct_masksuffix(ipv6calc_ipv6addr *ipv6addrp);

extern int  identifier_to_ipv6addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv6addr *ipv6addrp);

extern int  tokenlsb64_to_ipv6addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv6addr *ipv6addrp);
extern int  libipv6addr_ipv6addrstruct_to_tokenlsb64(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions);

extern int      libipv6addr_anonymize(ipv6calc_ipv6addr *ipv6addrp, const s_ipv6calc_anon_set *ipv6calc_anon_set);
extern uint32_t ipv6addr_get_payload_anonymized_iid(const ipv6calc_ipv6addr *ipv6addrp, const uint32_t typeinfo);
extern int      ipv6addr_get_payload_anonymized_prefix(const ipv6calc_ipv6addr *ipv6addrp, const int payload_selector, uint32_t *result_ptr);

extern int ipv6addr_iidrandomdetection(const ipv6calc_ipv6addr *ipv6addrp, s_iid_statistics *variancesp);

extern int  ipv6addr_filter(const ipv6calc_ipv6addr *ipv6addrp, const s_ipv6calc_filter_ipv6addr *filter);
extern int  ipv6addr_filter_parse(s_ipv6calc_filter_ipv6addr *filter, const char *token);
extern int  ipv6addr_filter_check(const s_ipv6calc_filter_ipv6addr *filter);
extern void ipv6addr_filter_clear(s_ipv6calc_filter_ipv6addr *filter);

extern int  libipv6addr_get_included_ipv4addr(const ipv6calc_ipv6addr *ipv6addrp, ipv6calc_ipv4addr *ipv4addrp, const int selector);

extern uint16_t libipv6addr_cc_index_by_addr(const ipv6calc_ipv6addr *ipv6addrp, unsigned int *data_source_ptr);
extern uint32_t libipv6addr_as_num32_by_addr(const ipv6calc_ipv6addr *ipv6addrp, unsigned int *data_source_ptr);
extern uint32_t libipv6addr_GeonameID_by_addr(const ipv6calc_ipv6addr *ipv6addrp, unsigned int *data_source_ptr, unsigned int *GeonameID_type_ptr);
extern int libipv6addr_registry_num_by_addr(const ipv6calc_ipv6addr *ipv6addrp);
