/*
 * Project    : ipv6calc
 * File       : libipv4addr.h
 * Version    : $Id: libipv4addr.h,v 1.4 2002/03/11 19:27:09 peter Exp $
 * Copyright  : 2002 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
 *
 * Information:
 *  Header file for libipv4addr.c
 */ 

#include "ipv6calc.h"
#include <netinet/in.h>


/* typedefs */
#ifndef _libipv4addr_h

#define _libipv4addr_h 1

typedef struct {
	struct in_addr in_addr;		/* in_addr structure */
	unsigned short prefixlength;    /* prefix length (0-32) 8 bit */
	int flag_prefixuse;		/* =1 prefix length in use (CIDR notation) */
	unsigned int scope;		/* address scope value 16 bit*/
	int flag_valid;			/* address structure filled */
} ipv6calc_ipv4addr;

#endif


/* prototypes */
extern unsigned int ipv4addr_getoctett(ipv6calc_ipv4addr *ipv4addrp, int numoctett);
extern unsigned int ipv4addr_getword(ipv6calc_ipv4addr *ipv4addrp, int numword);
extern unsigned int ipv4addr_getdword(ipv6calc_ipv4addr *ipv4addrp);

extern void ipv4addr_setoctett(ipv6calc_ipv4addr *ipv4addrp, int numocett, unsigned int value);
extern void ipv4addr_setword(ipv6calc_ipv4addr *ipv4addrp, int numword, unsigned int value);
extern void ipv4addr_setdword(ipv6calc_ipv4addr *ipv4addrp, unsigned int value);

extern void ipv4addr_clear(ipv6calc_ipv4addr *ipv4addrp);
extern void ipv4addr_clearall(ipv6calc_ipv4addr *ipv4addrp);

extern unsigned int ipv4addr_gettype(ipv6calc_ipv4addr *ipv4addrp);

extern int addr_to_ipv4addrstruct(char *addrstring, char *resultstring, ipv6calc_ipv4addr *ipv4addrp);
extern int libipv4addr_ipv4addrstruct_to_string(ipv6calc_ipv4addr *ipv4addrp, char *resultstring, unsigned long formatoptions);
