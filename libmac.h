/*
 * Project    : ipv6calc
 * File       : libmac.h
 * Version    : $Id: libmac.h,v 1.2 2002/02/27 23:07:15 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file for libmac.c
 */ 

/* typedefs */

#ifndef _libmac_h

#define _libmac_h

typedef struct {
	unsigned int addr[6];	/* MAC address in 6x 8 bit */
} ipv6calc_macaddr;

#endif

/* prototypes */
extern int mac_to_macaddrstruct(char *addrstring, char *resultstring, ipv6calc_macaddr *macaddrp);
