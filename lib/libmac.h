/*
 * Project    : ipv6calc
 * File       : libmac.h
 * Version    : $Id: libmac.h,v 1.4 2002/04/20 15:38:15 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libmac.c
 */ 

/* typedefs */

#if defined(__NetBSD__) || defined (__OpenBSD) || defined (__FreeBSD__)
#include <inttypes.h>
#else
#include <stdint.h>
#endif

#ifndef _libmac_h

#define _libmac_h

typedef struct {
	uint8_t addr[6];	/* MAC address in 6x 8 bit */
	int flag_valid;		/* address structure filled */
} ipv6calc_macaddr;

#endif

/* prototypes */
extern int mac_to_macaddrstruct(char *addrstring, char *resultstring, ipv6calc_macaddr *macaddrp);
extern void mac_clearall(ipv6calc_macaddr *macaddrp);
