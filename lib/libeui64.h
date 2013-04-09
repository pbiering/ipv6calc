/*
 * Project    : ipv6calc
 * File       : libeui64.h
 * Version    : $Id: libeui64.h,v 1.3 2013/04/09 20:09:33 ds6peter Exp $
 * Copyright  : 2001-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libeu64.c
 */ 

#include "libipv6addr.h"
#include "libmac.h"

#ifndef _libeui64_h

#define _libeui64_h

typedef struct {
	uint8_t addr[8];        /* EUI-64 address in 8x 8 bit */
	int flag_valid;         /* address structure filled */
} ipv6calc_eui64addr;

#endif

/* prototypes */
extern int create_eui64_from_mac(ipv6calc_ipv6addr *ipv6addrp, ipv6calc_macaddr *macaddrp);
extern int libeui64_addr_to_eui64addrstruct(char *addrstring, char *resultstring, ipv6calc_eui64addr *eui64addrp);
