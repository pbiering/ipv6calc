/*
 * Project    : ipv6calc/lib
 * File       : libipv4addr.h
 * Version    : $Id: libipv4addr.h,v 1.30 2014/02/02 17:08:22 ds6peter Exp $
 * Copyright  : 2002-2014 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
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
	struct in_addr in_addr;		/* in_addr structure */
	uint8_t prefixlength;		/* prefix length (0-32) 8 bit */
	int flag_prefixuse;		/* =1 prefix length in use (CIDR notation) */
	uint32_t scope;			/* address typeinfo/scope */
	int flag_valid;			/* address structure filled */
} ipv6calc_ipv4addr;


/* IPv4 address assignment structure */
typedef struct {
	const uint32_t ipv4addr;
	const uint32_t ipv4mask;
	const uint8_t  prefixlength;	/* prefix length (0-32) 8 bit */
	const char *string_registry;
} s_ipv4addr_assignment;

/* IPv4 address assignment hint structure */
typedef struct {
	const uint8_t  octet_msb;
	const unsigned int start;
	const unsigned int end;
} s_ipv4addr_assignment_hint;


typedef struct {
	int active;	
	uint32_t typeinfo_must_have;
	uint32_t typeinfo_may_not_have;
	/* others coming next */
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
#define IPV4_ADDR_ANONYMIZED			(uint32_t) 0x00000100U
#define IPV4_ADDR_GLOBAL			(uint32_t) 0x00000200U
#define IPV4_ADDR_UNKNOWN			(uint32_t) 0x00000400U
#define IPV4_ADDR_RESERVED			(uint32_t) 0x00000800U

#define IPV4_ADDR_ZEROCONF			(uint32_t) 0x00001000U
#define IPV4_ADDR_SITELOCAL			(uint32_t) 0x00002000U

#define IPV4_ADDR_6TO4RELAY			(uint32_t) 0x01000000U


/* text representation */
/*@unused@*/ static const s_type ipv6calc_ipv4addrtypestrings[] = {
	{ IPV4_ADDR_ANY			, "any" },
	{ IPV4_ADDR_UNICAST		, "unicast" },
	{ IPV4_ADDR_MULTICAST		, "multicast" },
	{ IPV4_ADDR_ANYCAST		, "anycast" },
	{ IPV4_ADDR_BROADCAST		, "broadcast" },
	{ IPV4_ADDR_LOOPBACK		, "loopback" },
	{ IPV4_ADDR_UNSPECIFIED		, "unspecified" },
	{ IPV4_ADDR_UNKNOWN		, "unknown" },
	{ IPV4_ADDR_RESERVED		, "reserved" },
	{ IPV4_ADDR_ZEROCONF		, "zeroconf" },
	{ IPV4_ADDR_SITELOCAL		, "site-local" },
	{ IPV4_ADDR_ANONYMIZED		, "anonymized" },
	{ IPV4_ADDR_GLOBAL		, "global" },
	{ IPV4_ADDR_6TO4RELAY		, "6to4relay" }
};


/* Registries */
#define IPV4_ADDR_REGISTRY_IANA		REGISTRY_IANA
#define IPV4_ADDR_REGISTRY_APNIC	REGISTRY_APNIC
#define IPV4_ADDR_REGISTRY_ARIN		REGISTRY_ARIN
#define IPV4_ADDR_REGISTRY_RIPE		REGISTRY_RIPE
#define IPV4_ADDR_REGISTRY_LACNIC	REGISTRY_LACNIC
#define IPV4_ADDR_REGISTRY_AFRINIC	REGISTRY_AFRINIC
#define IPV4_ADDR_REGISTRY_RESERVED	REGISTRY_RESERVED
#define IPV4_ADDR_REGISTRY_UNKNOWN	REGISTRY_UNKNOWN

/* IPv4 address anonymization
 *  Global IPv4 addresses are anoymized by storing country code and AS number
 *   and using prefix of experimental range (240-255.x.y.z)
 *
 *  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 
 *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  1 1 1 1
 * |p p p p|
 *  Prefix                
 *    0xF
 *
 * Pariy Bit (odd parity)
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
 */

#define ANON_METHOD_KEEPTYPEASNCC_IPV4_REQ_DB   (IPV6CALC_DB_IPV4_TO_CC | IPV6CALC_DB_IPV4_TO_AS)

#endif


/* prototypes */
extern uint8_t  ipv4addr_getoctet(const ipv6calc_ipv4addr *ipv4addrp, const unsigned int numoctett);
extern uint16_t ipv4addr_getword(const ipv6calc_ipv4addr *ipv4addrp, const unsigned int numword);
extern uint32_t ipv4addr_getdword(const ipv6calc_ipv4addr *ipv4addrp);

extern void ipv4addr_setoctet(ipv6calc_ipv4addr *ipv4addrp, const unsigned int numocett, const uint8_t value);
extern void ipv4addr_setword(ipv6calc_ipv4addr *ipv4addrp, const unsigned int numword, const uint16_t value);
extern void ipv4addr_setdword(ipv6calc_ipv4addr *ipv4addrp, const uint32_t value);

extern void ipv4addr_clear(ipv6calc_ipv4addr *ipv4addrp);
extern void ipv4addr_clearall(ipv6calc_ipv4addr *ipv4addrp);
extern void ipv4addr_copy(ipv6calc_ipv4addr *ipv4addrp_dst, const ipv6calc_ipv4addr *ipv4addrp_src);

extern uint32_t ipv4addr_gettype(const ipv6calc_ipv4addr *ipv4addrp);

extern int addr_to_ipv4addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv4addr *ipv4addrp);
extern int addrhex_to_ipv4addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv4addr *ipv4addrp, int flag_reverse);

extern int libipv4addr_ipv4addrstruct_to_string(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions);

extern int libipv4addr_to_reversestring(ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions);

extern int libipv4addr_to_octal(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions);
extern int libipv4addr_to_hex(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions);

extern int libipv4addr_anonymize(ipv6calc_ipv4addr *ipv4addrp, const unsigned int mask, const int method);
extern uint32_t ipv4addr_anonymized_get_as_num32(const ipv6calc_ipv4addr *ipv4addrp);
extern uint16_t ipv4addr_anonymized_get_cc_index(const ipv6calc_ipv4addr *ipv4addrp);

extern int ipv4addr_filter(const ipv6calc_ipv4addr *ipv4addrp, const s_ipv6calc_filter_ipv4addr *filter);
extern int ipv4addr_filter_parse(s_ipv6calc_filter_ipv4addr *filter, const char *token);
extern void ipv4addr_filter_clear(s_ipv6calc_filter_ipv4addr *filter);
