/*
 * Project    : ipv6calc
 * File       : libipv6addr.h
 * Version    : $Id: libipv6addr.h,v 1.8 2002/03/02 22:06:53 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
 *
 * Information:
 *  Header file for libipv6addr.c
 */ 

#include "ipv6calc.h"


/* typedefs */

#ifndef _libipv6addr_h

#define _libipv6addr_h 1

typedef struct {
	struct in6_addr in6_addr;	/* in6_addr structure */
	unsigned short prefixlength;	/* prefix length (0-128) 8 bit*/
	int flag_prefixuse;		/* =1 prefix length in use */
	unsigned int scope;		/* address scope value 16 bit*/
	unsigned short bit_start;	/* start of bit */
	unsigned short bit_end;		/* end of bit */
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

#define IPV6_NEW_ADDR_6TO4			0x10000U
#define IPV6_NEW_ADDR_6BONE			0x20000U
#define IPV6_NEW_ADDR_AGU			0x40000U
#define IPV6_NEW_ADDR_UNSPECIFIED		0x80000U
#define IPV6_NEW_ADDR_SOLICITED_NODE		0x100000U
#define IPV6_NEW_ADDR_ISATAP			0x200000U

/* text representations */
#define TXT_IPV6_ADDR_ANY			"unknown"
#define TXT_IPV6_ADDR_UNICAST			"unicast"
#define TXT_IPV6_ADDR_MULTICAST			"multicast"
#define TXT_IPV6_ADDR_ANYCAST			"anycast"
#define TXT_IPV6_ADDR_LOOPBACK			"loopback"
#define TXT_IPV6_ADDR_LINKLOCAL			"link-local"
#define TXT_IPV6_ADDR_SITELOCAL			"site-local"
#define TXT_IPV6_ADDR_COMPATv4			"compat-v4"
#define TXT_IPV6_ADDR_MAPPED			"mapped"
#define TXT_IPV6_ADDR_RESERVED			"reserved"
#define TXT_IPV6_NEW_ADDR_6TO4			"6to4"
#define TXT_IPV6_NEW_ADDR_6BONE			"6bone"
#define TXT_IPV6_NEW_ADDR_AGU			"aggregatable-global-unicast"
#define TXT_IPV6_NEW_ADDR_UNSPECIFIED		"unspecified"
#define TXT_IPV6_NEW_ADDR_SOLICITED_NODE	"solicited-node"
#define TXT_IPV6_NEW_ADDR_ISATAP		"ISATAP"

/* array of numerical types */
static const unsigned int ipv6addr_typesnum[] = {
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
	IPV6_NEW_ADDR_UNSPECIFIED,
	IPV6_NEW_ADDR_SOLICITED_NODE,
	IPV6_NEW_ADDR_ISATAP
};

static const char *ipv6addr_typesstring[] = {
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
	TXT_IPV6_NEW_ADDR_UNSPECIFIED,
	TXT_IPV6_NEW_ADDR_SOLICITED_NODE,
	TXT_IPV6_NEW_ADDR_ISATAP
};
#endif


/* prototypes */
extern unsigned int ipv6addr_getoctett(ipv6calc_ipv6addr *ipv6addrp, int numoctett);
extern unsigned int ipv6addr_getword(ipv6calc_ipv6addr *ipv6addrp, int numword);
extern unsigned int ipv6addr_getdword(ipv6calc_ipv6addr *ipv6addrp, int numdword);

extern void ipv6addr_setoctett(ipv6calc_ipv6addr *ipv6addrp, int numocett, unsigned int value);
extern void ipv6addr_setword(ipv6calc_ipv6addr *ipv6addrp, int numword, unsigned int value);
extern void ipv6addr_setdword(ipv6calc_ipv6addr *ipv6addrp, int numdword, unsigned int value);

extern void ipv6addr_clear(ipv6calc_ipv6addr *ipv6addrp);
extern void ipv6addr_clearall(ipv6calc_ipv6addr *ipv6addrp);

extern unsigned int ipv6addr_gettype(ipv6calc_ipv6addr *ipv6addrp);

extern int  addr_to_ipv6addrstruct(char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);

extern int  libipv6addr_ipv6addrstruct_to_uncompaddr(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, unsigned int formatoptions); /* going obsolete */
extern int  ipv6addrstruct_to_uncompaddr(ipv6calc_ipv6addr *ipv6addrp, char *resultstring); 

extern int  ipv6addrstruct_to_uncompaddrprefix(ipv6calc_ipv6addr *ipv6addrp, char *resultstring);
extern int  ipv6addrstruct_to_uncompaddrsuffix(ipv6calc_ipv6addr *ipv6addrp, char *resultstring);

extern int  libipv6addr_ipv6addrstruct_to_fulluncompaddr(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, unsigned int formatoptions);
extern int  ipv6addrstruct_to_fulluncompaddr(ipv6calc_ipv6addr *ipv6addrp, char *resultstring); /* going obsolete */

extern void ipv6addrstruct_maskprefix(ipv6calc_ipv6addr *ipv6addrp);
extern void ipv6addrstruct_masksuffix(ipv6calc_ipv6addr *ipv6addrp);

extern int  identifier_to_ipv6addrstruct(char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp);
