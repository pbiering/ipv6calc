/*
 * Version:     $Id: libipv6addr.h,v 1.1.1.1 2001/10/07 14:47:40 peter Exp $
 *
 * Header file for libipv6addr.c libary
 *
 * Author:      Peter Bieringer <pb@bieringer.de>
 *
 */ 

/* prototypes */
#include "ipv6calc.h"

extern unsigned int ipv6addr_getoctett(ipv6calc_ipv6addr *ipv6addrp, int numoctett);
extern unsigned int ipv6addr_getword(ipv6calc_ipv6addr *ipv6addrp, int numword);
extern unsigned int ipv6addr_getdword(ipv6calc_ipv6addr *ipv6addrp, int numdword);

extern void ipv6addr_setoctett(ipv6calc_ipv6addr *ipv6addrp, int numocett, unsigned int value);
extern void ipv6addr_setword(ipv6calc_ipv6addr *ipv6addrp, int numword, unsigned int value);
extern void ipv6addr_setdword(ipv6calc_ipv6addr *ipv6addrp, int numdword, unsigned int value);

extern void ipv6addr_clear(ipv6calc_ipv6addr *ipv6addrp);

extern unsigned int ipv6addr_gettype(ipv6calc_ipv6addr *ipv6addrp);

extern unsigned int ipv6addr_typesnum[];
extern char *ipv6addr_typesstring[];


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


#ifndef _libipv6addr_h

#define _libipv6addr_h 1

#define IPV6_ADDR_ANY				0x0000U

#define IPV6_ADDR_UNICAST      		0x0001U	
#define IPV6_ADDR_MULTICAST    		0x0002U	
#define IPV6_ADDR_ANYCAST			0x0004U

#define IPV6_ADDR_LOOPBACK			0x0010U
#define IPV6_ADDR_LINKLOCAL			0x0020U
#define IPV6_ADDR_SITELOCAL			0x0040U

#define IPV6_ADDR_COMPATv4			0x0080U

#define IPV6_ADDR_SCOPE_MASK		0x00f0U

#define IPV6_ADDR_MAPPED			0x1000U
#define IPV6_ADDR_RESERVED			0x2000U	/* reserved address space */

#define IPV6_NEW_ADDR_6TO4			0x10000U
#define IPV6_NEW_ADDR_6BONE			0x20000U
#define IPV6_NEW_ADDR_AGU			0x40000U
#define IPV6_NEW_ADDR_UNSPECIFIED	0x80000U

/* text representations */
#define TXT_IPV6_ADDR_ANY			"unknown"
#define TXT_IPV6_ADDR_UNICAST		"unicast"
#define TXT_IPV6_ADDR_MULTICAST		"multicast"
#define TXT_IPV6_ADDR_ANYCAST		"anycast"
#define TXT_IPV6_ADDR_LOOPBACK		"loopback"
#define TXT_IPV6_ADDR_LINKLOCAL		"link-local"
#define TXT_IPV6_ADDR_SITELOCAL		"site-local"
#define TXT_IPV6_ADDR_COMPATv4		"compat-v4"
#define TXT_IPV6_ADDR_MAPPED		"mapped"
#define TXT_IPV6_ADDR_RESERVED		"reserved"
#define TXT_IPV6_NEW_ADDR_6TO4		"6to4"
#define TXT_IPV6_NEW_ADDR_6BONE		"6bone"
#define TXT_IPV6_NEW_ADDR_AGU		"aggregatable-global-unicast"
#define TXT_IPV6_NEW_ADDR_UNSPECIFIED	"unspecified"

#define IPV6INFO_NUM 13

#endif
