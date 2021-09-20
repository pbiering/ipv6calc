/*
 * Project    : ipv6calc
 * File       : libipaddr.h
 * Version    : $Id$
 * Copyright  : 2014-2021 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libipaddr.c
 */ 

#ifndef _libipaddr_h

#define _libipaddr_h 1


#include "ipv6calc_inttypes.h"
#include "libipv4addr.h"
#include "libipv6addr.h"


/**************
 * Defines
 * ************/

/* IPv4/6 address storage structure */
typedef struct {
	uint32_t addr[4];		/* 4x 32-bit, IPv4: only addr[0] used, addr[1-3]=0 */
	uint32_t scope;			/* address typeinfo/scope */
	uint8_t  proto;			/* protocol */
	uint8_t  flag_valid;		/* address structure filled */
	uint8_t  prefixlength;		/* prefix length (0-128) 8 bit*/
	int      flag_prefixuse;	/* =1 prefix length in use */
	uint32_t typeinfo1;		/* typeinfo 1 */
	uint32_t typeinfo2;		/* typeinfo 2 */
} ipv6calc_ipaddr;


/**************
 * Macros
 * ************/

/* ipv4addr ptr -> ipaddr */
#define CONVERT_IPV4ADDRP_IPADDR(ipv4addrp, ipaddr) \
	ipaddr.typeinfo1	= (*ipv4addrp).typeinfo; \
	ipaddr.typeinfo2	= (*ipv4addrp).typeinfo2; \
	ipaddr.prefixlength	= (*ipv4addrp).prefixlength; \
	ipaddr.flag_prefixuse	= (*ipv4addrp).flag_prefixuse; \
	ipaddr.addr[0]		= ipv4addr_getdword(ipv4addrp); \
	ipaddr.addr[1]		= 0; \
	ipaddr.addr[2]		= 0; \
	ipaddr.addr[3]		= 0; \
	ipaddr.flag_valid	= 1; \
	ipaddr.proto		= IPV6CALC_PROTO_IPV4;

/* ipv4addr ptr -> ipaddr ptr */
#define CONVERT_IPV4ADDRP_IPADDRP(ipv4addrp, ipaddrp) \
	ipaddrp->typeinfo1	= ipv4addrp->typeinfo; \
	ipaddrp->typeinfo2	= ipv4addrp->typeinfo2; \
	ipaddrp->prefixlength	= ipv4addrp->prefixlength; \
	ipaddrp->flag_prefixuse	= ipv4addrp->flag_prefixuse; \
	ipaddrp->addr[0]	= ipv4addr_getdword(ipv4addrp); \
	ipaddrp->addr[1]	= 0; \
	ipaddrp->addr[2]	= 0; \
	ipaddrp->addr[3]	= 0; \
	ipaddrp->flag_valid	= 1; \
	ipaddrp->proto		= IPV6CALC_PROTO_IPV4;

/* ipv6addr ptr -> ipaddr */
#define CONVERT_IPV6ADDRP_IPADDR(ipv6addrp, ipaddr) \
	ipaddr.typeinfo1	= (*ipv6addrp).typeinfo; \
	ipaddr.typeinfo2	= (*ipv6addrp).typeinfo2; \
	ipaddr.prefixlength	= (*ipv6addrp).prefixlength; \
	ipaddr.flag_prefixuse	= (*ipv6addrp).flag_prefixuse; \
	ipaddr.addr[0]		= ipv6addr_getdword(ipv6addrp, 0); \
	ipaddr.addr[1]		= ipv6addr_getdword(ipv6addrp, 1); \
	ipaddr.addr[2]		= ipv6addr_getdword(ipv6addrp, 2); \
	ipaddr.addr[3]		= ipv6addr_getdword(ipv6addrp, 3); \
	ipaddr.flag_valid	= 1; \
	ipaddr.proto		= IPV6CALC_PROTO_IPV6;

/* ipv6addr ptr -> ipaddr ptr */
#define CONVERT_IPV6ADDRP_IPADDRP(ipv6addrp, ipaddrp) \
	ipaddrp->typeinfo1	= ipv6addrp->typeinfo; \
	ipaddrp->typeinfo2	= ipv6addrp->typeinfo2; \
	ipaddrp->prefixlength	= ipv6addrp->prefixlength; \
	ipaddrp->flag_prefixuse	= ipv6addrp->flag_prefixuse; \
	ipaddrp->addr[0]	= ipv6addr_getdword(ipv6addrp, 0); \
	ipaddrp->addr[1]	= ipv6addr_getdword(ipv6addrp, 1); \
	ipaddrp->addr[2]	= ipv6addr_getdword(ipv6addrp, 2); \
	ipaddrp->addr[3]	= ipv6addr_getdword(ipv6addrp, 3); \
	ipaddrp->flag_valid	= 1; \
	ipaddrp->proto		= IPV6CALC_PROTO_IPV6;


/* ipaddr ptr -> ipv4addr */
#define CONVERT_IPADDRP_IPV4ADDR(ipaddrp, ipv4addr) \
	ipv4addr_clearall(&ipv4addr); \
	ipv4addr_setdword(&ipv4addr, ipaddrp->addr[0]); \
	ipv4addr.typeinfo   = ipaddrp->typeinfo1; \
	ipv4addr.typeinfo2  = ipaddrp->typeinfo2; \
	ipv4addr.prefixlength = ipaddrp->prefixlength; \
	ipv4addr.flag_prefixuse = ipaddrp->flag_prefixuse; \
	ipv4addr.flag_valid = 1;

/* ipaddr ptr -> ipv6addr */
#define CONVERT_IPADDRP_IPV6ADDR(ipaddrp, ipv6addr) \
	ipv6addr_clearall(&ipv6addr); \
	ipv6addr_setdword(&ipv6addr, 0, ipaddrp->addr[0]); \
	ipv6addr_setdword(&ipv6addr, 1, ipaddrp->addr[1]); \
	ipv6addr_setdword(&ipv6addr, 2, ipaddrp->addr[2]); \
	ipv6addr_setdword(&ipv6addr, 3, ipaddrp->addr[3]); \
	ipv6addr.typeinfo   = ipaddrp->typeinfo1; \
	ipv6addr.typeinfo2  = ipaddrp->typeinfo2; \
	ipv6addr.prefixlength = ipaddrp->prefixlength; \
	ipv6addr.flag_prefixuse = ipaddrp->flag_prefixuse; \
	ipv6addr.flag_valid = 1;

#endif


extern int libipaddr_ipaddrstruct_to_string(const ipv6calc_ipaddr *ipaddrp, char *resultstring, const size_t resultstring_length, const uint32_t formatoptions);
extern void libipaddr_clearall(ipv6calc_ipaddr *ipaddrp);
