/*
 * Project    : ipv6calc/lib
 * File       : libipv4addr.h
 * Version    : $Id: libipv4addr.h,v 1.22 2012/03/20 06:36:30 peter Exp $
 * Copyright  : 2002-2012 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
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
	{ IPV4_ADDR_UNKNOWN		, "unknown" },
	{ IPV4_ADDR_RESERVED		, "reserved" },
	{ IPV4_ADDR_ZEROCONF		, "zeroconf" },
	{ IPV4_ADDR_SITELOCAL		, "site-local" },
	{ IPV4_ADDR_6TO4RELAY		, "6to4relay" }
};


/* Registries */
#define IPV4_ADDR_REGISTRY_IANA		0x02
#define IPV4_ADDR_REGISTRY_APNIC	0x03
#define IPV4_ADDR_REGISTRY_ARIN		0x04
#define IPV4_ADDR_REGISTRY_RIPE		0x05
#define IPV4_ADDR_REGISTRY_LACNIC	0x06
#define IPV4_ADDR_REGISTRY_AFRINIC	0x07
#define IPV4_ADDR_REGISTRY_RESERVED	0x0e
#define IPV4_ADDR_REGISTRY_UNKNOWN	0x0f




#endif


/* prototypes */
extern uint8_t  ipv4addr_getoctet(const ipv6calc_ipv4addr *ipv4addrp, const unsigned int numoctett);
extern uint16_t ipv4addr_getword(const ipv6calc_ipv4addr *ipv4addrp, const unsigned int numword);
extern uint32_t ipv4addr_getdword(const ipv6calc_ipv4addr *ipv4addrp);

extern void ipv4addr_setoctet(ipv6calc_ipv4addr *ipv4addrp, const unsigned int numocett, const unsigned int value);
extern void ipv4addr_setword(ipv6calc_ipv4addr *ipv4addrp, const unsigned int numword, const unsigned int value);
extern void ipv4addr_setdword(ipv6calc_ipv4addr *ipv4addrp, const unsigned int value);

extern void ipv4addr_clear(ipv6calc_ipv4addr *ipv4addrp);
extern void ipv4addr_clearall(ipv6calc_ipv4addr *ipv4addrp);

extern uint32_t ipv4addr_gettype(const ipv6calc_ipv4addr *ipv4addrp);

extern int ipv4addr_getregistry(const ipv6calc_ipv4addr *ipv4addrp);

extern int addr_to_ipv4addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv4addr *ipv4addrp);
extern int addrhex_to_ipv4addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv4addr *ipv4addrp, int flag_reverse);

extern int libipv4addr_ipv4addrstruct_to_string(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions);

extern int libipv4addr_get_registry_string(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring);

extern int libipv4addr_to_reversestring(ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions);

extern int libipv4addr_to_octal(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions);
extern int libipv4addr_to_hex(const ipv6calc_ipv4addr *ipv4addrp, char *resultstring, const uint32_t formatoptions);

extern void libipv4addr_anonymize(ipv6calc_ipv4addr *ipv4addrp, unsigned int mask);

extern int ipv4addr_filter(const ipv6calc_ipv4addr *ipv4addrp, const s_ipv6calc_filter_ipv4addr *filter);
extern int ipv4addr_filter_parse(s_ipv6calc_filter_ipv4addr *filter, const char *token);
extern void ipv4addr_filter_clear(s_ipv6calc_filter_ipv4addr *filter);
