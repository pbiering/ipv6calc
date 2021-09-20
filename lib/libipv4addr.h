/*
 * Project    : ipv6calc/lib
 * File       : libipv4addr.h
 * Version    : $Id$
 * Copyright  : 2002-2021 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
 * License    : GNU GPL v2
 *
 * Information:
 *  Header file for libipv4addr.c
 */ 

#include "ipv6calctypes.h"
#include <netinet/in.h>

/* typedefs */
#ifndef _libipv4addr_h

#define _libipv4addr_h 1

/* IPv4 address structure */
typedef struct {
	struct   in_addr in_addr;	/* in_addr structure */
	uint8_t  prefixlength;		/* prefix length (0-32) 8 bit */
	int      flag_prefixuse;	/* =1: prefix length in use (CIDR notation) */
	uint32_t typeinfo;		/* address typeinfo/scope */
	uint32_t typeinfo2;		/* address typeinfo/scope #2 */
	int8_t   flag_typeinfo;		/* =1: valid typeinfo */
	int8_t   flag_valid;		/* address structure filled */
	uint8_t  test_mode;		/* test mode */
} ipv6calc_ipv4addr;

/* IPv4 Address filter structure */
typedef struct {
        int active;
        int addr_must_have_max;
        int addr_may_not_have_max;
        ipv6calc_ipv4addr ipv4addr_must_have[IPV6CALC_FILTER_IPV4ADDR];
        ipv6calc_ipv4addr ipv4addr_may_not_have[IPV6CALC_FILTER_IPV6ADDR];
} s_ipv6calc_filter_addr_ipv4;

/* IPv4 filter structure */
typedef struct {
	int active;
	s_ipv6calc_filter_typeinfo    filter_typeinfo;
	s_ipv6calc_filter_db_cc       filter_db_cc;        // Country Code filter
	s_ipv6calc_filter_db_asn      filter_db_asn;       // Autonomous System Number filter
	s_ipv6calc_filter_db_registry filter_db_registry;  // Registry filter
	s_ipv6calc_filter_addr_ipv4   filter_addr;         // IPv4 Address filter
	/* others may come next */
} s_ipv6calc_filter_ipv4addr;


/*
 * IPv4 address type definitions 
 */

#define IPV4_ADDR_ANY				(uint32_t) 0x00000000U

#define IPV4_ADDR_UNICAST			(uint32_t) 0x00000001U
#define IPV4_ADDR_MULTICAST			(uint32_t) 0x00000002U
#define IPV4_ADDR_ANYCAST			(uint32_t) 0x00000004U
#define IPV4_ADDR_BROADCAST			(uint32_t) 0x00000008U
#define IPV4_ADDR_LOOPBACK			(uint32_t) 0x00000010U
#define IPV4_ADDR_UNSPECIFIED			(uint32_t) 0x00000020U
#define IPV4_ADDR_ANONYMIZED_GEONAMEID		(uint32_t) 0x00000080U
#define IPV4_ADDR_ANONYMIZED			(uint32_t) 0x00000100U
#define IPV4_ADDR_GLOBAL			(uint32_t) 0x00000200U
#define IPV4_ADDR_UNKNOWN			(uint32_t) 0x00000400U
#define IPV4_ADDR_RESERVED			(uint32_t) 0x00000800U

#define IPV4_ADDR_ZEROCONF			(uint32_t) 0x00001000U
#define IPV4_ADDR_SITELOCAL			(uint32_t) 0x00002000U

#define IPV4_ADDR_6TO4RELAY			(uint32_t) 0x01000000U

#define IPV4_ADDR_LISP				(uint32_t) 0x00010000U	/* IPv6 LISP (RFC6830) */
#define IPV4_ADDR_LISP_PETR			(uint32_t) 0x00020000U	/* IPv6 LISP Proxy Egress Tunnel Routers (PETRY) Anycast */
#define IPV4_ADDR_LISP_MAP_RESOLVER		(uint32_t) 0x00040000U	/* IPv6 LISP Map Resolver Anycast */

/* Registries */
#define IPV4_ADDR_REGISTRY_IANA		REGISTRY_IANA
#define IPV4_ADDR_REGISTRY_APNIC	REGISTRY_APNIC
#define IPV4_ADDR_REGISTRY_ARIN		REGISTRY_ARIN
#define IPV4_ADDR_REGISTRY_RIPENCC	REGISTRY_RIPENCC
#define IPV4_ADDR_REGISTRY_LACNIC	REGISTRY_LACNIC
#define IPV4_ADDR_REGISTRY_AFRINIC	REGISTRY_AFRINIC
#define IPV4_ADDR_REGISTRY_RESERVED	REGISTRY_RESERVED
#define IPV4_ADDR_REGISTRY_UNKNOWN	REGISTRY_UNKNOWN

/* IPv4 address anonymization type "keep-type-asn-cc"
 *  Global IPv4 addresses are anonymized by storing country code and AS number
 *   and using prefix of experimental range (240-255.x.y.z)
 *
 *  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 
 *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  1 1 1 1
 * |p p p p|
 *  Prefix                
 *    0xF
 *
 * Parity Bit (odd parity)
 *         |P|
 *
 * Mapping of Country Code
 *           |c c c c c c c c c c|
 *            10-bit Country Code mapping [A-Z]*[A-Z0-9] (936)
 *            0x3FF = unknown country
 *            0x000-0x3A7: c1= c / 36, c2 = c % 36
 *
 * Mapping of ASN <= 65535
 *                                0|a a a a a a a a a a a a a a a a|
 *                                    16-bit ASN
 *
 * Mapping of ASN >= 65536, ASN & 0xE000 == 0 (12-bit LSB is ok) 
 *                                1|r r r|0|l l l l l l l l l l l l|
 *                                           13-bit LSB of ASN
 *
 * Mapping of ASN >= 65536, ASN & 0xE000 != 0 (12-bit LSB overflow) => unspecified
 *                                1|r r r|1|0 0 0 0 0 0 0 0 0 0 0 0|
 *
 * Mapping of ASN registry for ASN >= 65536
 *                                  0 0 0 = "unknown ASN registry"
 *                                  0 1 0 = APNIC
 *                                  0 1 1 = RIPE
 *                                  1 0 0 = LACNIC
 *                                  1 0 1 = AFRINIC
 *                                  1 1 0 = ARIN
 *
 * Mapping of LISP
 *                                1|r r r|1|1|0 0 0 0 0 0 0 0 0 0 0|
 */

/* IPv4 address anonymization type "keep-type-geonameid"
 *  Global IPv4 addresses are anonymized by storing GeonameID
 *   and using prefix of experimental range (240-255.x.y.z)
 *
 *  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
 *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  1 1 1 1
 * |p p p p|
 *  Prefix
 *    0xF
 *
 * Parity Bit (even parity)
 *         |P|
 *
 * Mapping of GeonameID source
 *           |s s s|
 *            3-bit GeonameID source information
 *            0 0 0 (0x0) = unknown
 *            0 0 1 (0x1) = continent  (prio: lowest)
 *            0 1 0 (0x2) = country
 *            0 1 1 (0x3) = state/prov
 *            1 0 0 (0x4) = district
 *            1 0 1 (0x5) = city       (prio: highest)
 *            1 1 0 (0x6) = (reserved)
 *            1 1 1 (0x7) = LISP
 *
 * Mapping of GeonameID (limited to IDs < 2^24, in worst case ID from source with lower prio is used)
 *                 |g g g g g g g g g g g g g g g g g g g g g g g g|
 *
 * Mapping of LISP registry
 *                                  0 0 0 = "unknown ASN registry"
 *                                  0 1 0 = APNIC
 *                                  0 1 1 = RIPE
 *                                  1 0 0 = LACNIC
 *                                  1 0 1 = AFRINIC
 *                                  1 1 0 = ARIN
 *                 |0 0 0 0 0 0 0 1 r r r 1 1 0 0 0 0 0 0 0 0 0 0 0|
 */

// Prefix anonymization on method=kp
#define ANON_METHOD_KEEPTYPEASNCC_IPV4_REQ_DB   (IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_IPV4_TO_AS)
// Prefix anonymization on method=kg
#define ANON_METHOD_KEEPTYPEGEONAMEID_IPV4_REQ_DB   (IPV6CALC_DB_IPV4_TO_GEONAMEID)

#endif

/* references */
extern const s_type ipv6calc_ipv4addrtypestrings[];

extern const int ipv6calc_ipv4addrtypestrings_entries;


/* prototypes */
extern uint8_t  ipv4addr_getoctet(const ipv6calc_ipv4addr *ipv4addrp, const unsigned int numoctet);
extern uint16_t ipv4addr_getword(const ipv6calc_ipv4addr *ipv4addrp, const unsigned int numword);
extern uint32_t ipv4addr_getdword(const ipv6calc_ipv4addr *ipv4addrp);

extern void ipv4addr_setoctet(ipv6calc_ipv4addr *ipv4addrp, const unsigned int numoctet, const uint8_t value);
extern void ipv4addr_setword(ipv6calc_ipv4addr *ipv4addrp, const unsigned int numword, const uint16_t value);
extern void ipv4addr_setdword(ipv6calc_ipv4addr *ipv4addrp, const uint32_t value);

extern void ipv4addr_clear(ipv6calc_ipv4addr *ipv4addrp);
extern void ipv4addr_clearall(ipv6calc_ipv4addr *ipv4addrp);
extern void ipv4addr_copy(ipv6calc_ipv4addr *ipv4addrp_dst, const ipv6calc_ipv4addr *ipv4addrp_src);

extern int ipv4addr_compare(const ipv6calc_ipv4addr *ipv4addrp1, const ipv6calc_ipv4addr *ipv4addrp2, const uint16_t compare_flags);

extern uint32_t ipv4addr_gettype(const ipv6calc_ipv4addr *ipv4addrp);
extern void ipv4addr_settype(ipv6calc_ipv4addr *ipv4addrp, int flag_reset);

extern int addr_to_ipv4addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv4addr *ipv4addrp);
extern int addrhex_to_ipv4addrstruct(const char *addrstring, char *resultstring, const size_t resultstring_length, ipv6calc_ipv4addr *ipv4addrp, const int flag_reverse);

extern int libipv4addr_ipv4addrstruct_to_string(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions);

extern int libipv4addr_to_reversestring(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions);

extern int libipv4addr_to_octal(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions);
extern int libipv4addr_to_hex(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions);

void ipv4addrstruct_maskprefix(ipv6calc_ipv4addr *ipv4addrp);
void ipv4addrstruct_masksuffix(ipv6calc_ipv4addr *ipv4addrp);

extern int libipv4addr_anonymize(ipv6calc_ipv4addr *ipv4addrp, const unsigned int mask, const int method);
extern uint32_t ipv4addr_anonymized_get_as_num32(const ipv6calc_ipv4addr *ipv4addrp);
extern uint16_t ipv4addr_anonymized_get_cc_index(const ipv6calc_ipv4addr *ipv4addrp);

extern int ipv4addr_filter(const ipv6calc_ipv4addr *ipv4addrp, const s_ipv6calc_filter_ipv4addr *filter);
extern int ipv4addr_filter_parse(s_ipv6calc_filter_ipv4addr *filter, const char *token);
extern int ipv4addr_filter_check(const s_ipv6calc_filter_ipv4addr *filter);
extern void ipv4addr_filter_clear(s_ipv6calc_filter_ipv4addr *filter);

extern uint16_t libipv4addr_cc_index_by_addr(const ipv6calc_ipv4addr *ipv4addrp, unsigned int *data_source_ptr);
extern uint32_t libipv4addr_as_num32_by_addr(const ipv6calc_ipv4addr *ipv4addrp, unsigned int *data_source_ptr);
extern uint32_t libipv4addr_GeonameID_by_addr(const ipv6calc_ipv4addr *ipv4addrp, unsigned int *data_source_ptr, unsigned int *GeonameID_type_ptr);
extern int libipv4addr_registry_num_by_addr(const ipv6calc_ipv4addr *ipv4addrp);
